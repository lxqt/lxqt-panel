/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2023 LXQt team
 * Authors:
 *  Filippo Gentile <filippogentile@disroot.org>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "lxqtfancymenu.h"
#include "lxqtfancymenuconfiguration.h"
#include "lxqtfancymenuwindow.h"
#include "../panel/lxqtpanel.h"
#include <QTimer>
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <lxqt-globalkeys.h>
#include <QApplication>

#include <XdgMenuWidget>
#include <XdgIcon>

#include <QStandardPaths>
#include <QClipboard>
#include <QMimeData>
#include <XdgAction>

#include <QDir>

#include "lxqtfancymenushortcututils.h"

#define DEFAULT_SHORTCUT "Alt+F1"

LXQtFancyMenu::LXQtFancyMenu(const ILXQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    mWindow(nullptr),
    mShortcut(nullptr),
    mFilterClear(false)
{
    mWindow = new LXQtFancyMenuWindow(&mButton);
    mWindow->setObjectName(QStringLiteral("TopLevelFancyMenu"));
    mWindow->installEventFilter(this);
    connect(mWindow, &LXQtFancyMenuWindow::aboutToHide, &mHideTimer, QOverload<>::of(&QTimer::start));
    connect(mWindow, &LXQtFancyMenuWindow::aboutToShow, &mHideTimer, &QTimer::stop);
    connect(mWindow, &LXQtFancyMenuWindow::favoritesChanged, this, &LXQtFancyMenu::saveFavorites);

    mDelayedPopup.setSingleShot(true);
    mDelayedPopup.setInterval(200);
    connect(&mDelayedPopup, &QTimer::timeout, this, &LXQtFancyMenu::showHideMenu);
    mHideTimer.setSingleShot(true);
    mHideTimer.setInterval(250);

    mButton.setAutoRaise(true);
    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    //Notes:
    //1. installing event filter to parent widget to avoid infinite loop
    //   (while setting icon we also need to set the style)
    //2. delaying of installEventFilter because in c-tor mButton has no parent widget
    //   (parent is assigned in panel's logic after widget() call)
    QTimer::singleShot(0, mButton.parentWidget(), [this] {
        Q_ASSERT(mButton.parentWidget());
        mButton.parentWidget()->installEventFilter(this);
    });

    connect(&mButton, &QToolButton::clicked, this, &LXQtFancyMenu::showHideMenu);

    QTimer::singleShot(0, this, [this] {
        settingsChanged();
    });

    mShortcut = GlobalKeyShortcut::Client::instance()->addAction(QString{}, QStringLiteral("/panel/%1/show_hide").arg(settings()->group()), LXQtFancyMenu::tr("Show/hide main menu"), this);
    if (mShortcut)
    {
        connect(mShortcut, &GlobalKeyShortcut::Action::shortcutChanged, this, [this](const QString &, const QString & shortcut) {
                mShortcutSeq = shortcut;
        });
        connect(mShortcut, &GlobalKeyShortcut::Action::registrationFinished, this, [this] {
            if (mShortcut->shortcut().isEmpty())
                mShortcut->changeShortcut(QStringLiteral(DEFAULT_SHORTCUT));
            else
                mShortcutSeq = mShortcut->shortcut();
        });
        connect(mShortcut, &GlobalKeyShortcut::Action::activated, this, [this] {
            if (!mHideTimer.isActive())
                // Delay this a little -- if we don't do this, search field
                // won't be able to capture focus
                // See <https://github.com/lxqt/lxqt-panel/pull/131> and
                // <https://github.com/lxqt/lxqt-panel/pull/312>
                mDelayedPopup.start();
        });
    }
}


/************************************************

 ************************************************/
LXQtFancyMenu::~LXQtFancyMenu()
{
    mButton.parentWidget()->removeEventFilter(this);

    delete mWindow;
}


/************************************************

 ************************************************/
void LXQtFancyMenu::showHideMenu()
{
    if(mWindow && mWindow->isVisible())
        mWindow->hide();
    else
        showMenu();
}

/************************************************

 ************************************************/
void LXQtFancyMenu::showMenu()
{
    if (!mWindow)
        return;

    willShowWindow(mWindow);
    // Just using Qt`s activateWindow() won't work on some WMs like Kwin.
    // Solution is to execute menu 1ms later using timer
    mWindow->move(calculatePopupWindowPos(mWindow->sizeHint()).topLeft());

    emit mWindow->aboutToShow();
    mWindow->show();
    mWindow->setSearchEditFocus();
}

/************************************************

 ************************************************/
void LXQtFancyMenu::settingsChanged()
{
    setButtonIcon();
    if (settings()->value(QStringLiteral("showText"), false).toBool())
    {
        mButton.setText(settings()->value(QStringLiteral("text"), QStringLiteral("Start")).toString());
        mButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else
    {
        mButton.setText(QLatin1String(""));
        mButton.setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    mLogDir = settings()->value(QStringLiteral("log_dir"), QString()).toString();

    QString menu_file = settings()->value(QStringLiteral("menu_file"), QString()).toString();
    if (menu_file.isEmpty())
        menu_file = XdgMenu::getMenuFileName();

    if (mMenuFile != menu_file)
    {
        mMenuFile = menu_file;
        mXdgMenu.setEnvironments(QStringList() << QStringLiteral("X-LXQT") << QStringLiteral("LXQt"));
        mXdgMenu.setLogDir(mLogDir);

        bool res = mXdgMenu.read(mMenuFile);
        connect(&mXdgMenu, &XdgMenu::changed, this, &LXQtFancyMenu::buildMenu);
        if (res)
        {
            QTimer::singleShot(1000, this, &LXQtFancyMenu::buildMenu);
        }
        else
        {
            QMessageBox::warning(nullptr, QStringLiteral("Parse error"), mXdgMenu.errorString());
            return;
        }
    }

    loadFavorites();
    setMenuFontSize();

    //clear the search to not leaving the menu in wrong state
    mFilterClear = settings()->value(QStringLiteral("filterClear"), false).toBool();
    mWindow->setFilterClear(mFilterClear);

    bool buttonsAtTop = settings()->value(QStringLiteral("buttonsAtTop"), false).toBool();
    mWindow->setButtonPosition(buttonsAtTop ? LXQtFancyMenuButtonPosition::Top : LXQtFancyMenuButtonPosition::Bottom);

    bool categoriesAtRight = settings()->value(QStringLiteral("categoriesAtRight"), true).toBool();
    mWindow->setCategoryPosition(categoriesAtRight ? LXQtFancyMenuCategoryPosition::Right : LXQtFancyMenuCategoryPosition::Left);

    realign();
}

/************************************************

 ************************************************/
void LXQtFancyMenu::buildMenu()
{
    mWindow->rebuildMenu(mXdgMenu);

    mWindow->doSearch();
    setMenuFontSize();
}

void LXQtFancyMenu::loadFavorites()
{
    bool listChanged = false;

    const QList<QMap<QString, QVariant> > list = settings()->readArray(QStringLiteral("favorites"));
    QStringList fileList;
    for(const QMap<QString, QVariant>& item : list)
    {
        QString file = item.value(QStringLiteral("desktopFile")).toString();
        if(file.isEmpty())
        {
            listChanged = true;
            continue;
        }

        QString canonicalPath = QDir(file).canonicalPath();
        if(canonicalPath != file)
            listChanged = true;

        if(canonicalPath.isEmpty())
            continue;

        if(fileList.contains(canonicalPath))
        {
            // Don't add duplicates
            listChanged = true;
            continue;
        }

        fileList.append(canonicalPath);
    }

    mWindow->setFavorites(fileList);

    if(listChanged)
        saveFavorites();
}

void LXQtFancyMenu::saveFavorites()
{
    const QStringList fileList = mWindow->favorites();

    QList<QMap<QString, QVariant> > list;
    for(const QString& file : fileList)
    {
        QMap<QString, QVariant> item;
        item.insert(QStringLiteral("desktopFile"), file);
        list.append(item);
    }

    settings()->setArray(QStringLiteral("favorites"), list);
}

/************************************************

 ************************************************/
void LXQtFancyMenu::setMenuFontSize()
{
    if (!mWindow)
        return;

    QFont menuFont = mButton.font();
    bool customFont = settings()->value(QStringLiteral("customFont"), false).toBool();
    int customFontSize = settings()->value(QStringLiteral("customFontSize")).toInt();

    if(customFont)
    {
        menuFont = mWindow->font();
        menuFont.setPointSize(customFontSize);
    }

    mWindow->setCustomFont(menuFont);
}

/************************************************

 ************************************************/
void LXQtFancyMenu::setButtonIcon()
{
    if (settings()->value(QStringLiteral("ownIcon"), false).toBool())
    {
        mButton.setStyleSheet(QStringLiteral("#FancyMenu { qproperty-icon: url(%1); }")
                .arg(settings()->value(QLatin1String("icon"), QLatin1String(LXQT_GRAPHICS_DIR"/helix.svg")).toString()));
    } else
    {
        mButton.setStyleSheet(QString());
    }
}

/************************************************

 ************************************************/
QDialog *LXQtFancyMenu::configureDialog()
{
    return new LXQtFancyMenuConfiguration(settings(), mShortcut, QStringLiteral(DEFAULT_SHORTCUT));
}

/************************************************

 ************************************************/
bool LXQtFancyMenu::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == mButton.parentWidget())
    {
        // the application is given a new QStyle
        if(event->type() == QEvent::StyleChange)
        {
            setMenuFontSize();
            setButtonIcon();
            mWindow->updateButtonIconSize();
        }
    }
    else if(obj == mWindow)
    {
        if(event->type() == QEvent::KeyRelease)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (LXQtFancyMenuShortcutUtils::match(keyEvent, mShortcutSeq))
            {
                //TODO: isn't timer already fired by hide() ???
                mHideTimer.start();
                mWindow->hide(); // close the app menu
                return true;
            }
            //TODO: go to item which starts with pressed letter
        }
        else if (event->type() == QEvent::Resize)
        {
            QResizeEvent *e = static_cast<QResizeEvent *>(event);
            if (e->oldSize().isValid() && e->oldSize() != e->size())
            {
                mWindow->move(calculatePopupWindowPos(e->size()).topLeft());
            }
        }
    }
    return false;
}

#undef DEFAULT_SHORTCUT
