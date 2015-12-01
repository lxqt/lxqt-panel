/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "lxqtmainmenu.h"
#include "lxqtmainmenuconfiguration.h"
#include "../panel/lxqtpanel.h"
#include <QDebug>
#include <XdgDesktopFile>
#include <XmlHelper>
#include <QSettings>
#include <QFileInfo>
#include <QAction>
#include <QTimer>
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>
#include <LXQt/PowerManager>
#include <LXQt/ScreenSaver>
#include <lxqt-globalkeys.h>
#include <algorithm> // for find_if()
#include <KWindowSystem/KWindowSystem>

#include <XdgIcon>
#include <XdgDesktopFile>
#include <XdgMenuWidget>

#ifdef HAVE_MENU_CACHE
#include "xdgcachedmenu.h"
#endif

#include <QPixmap>
#include <QStack>
#include <QCursor>

#define DEFAULT_SHORTCUT "Alt+F1"

LXQtMainMenu::LXQtMainMenu(const ILXQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    mMenu(0),
    mShortcut(0)
{
#ifdef HAVE_MENU_CACHE
    mMenuCache = NULL;
    mMenuCacheNotify = 0;
#endif

    mDelayedPopup.setSingleShot(true);
    mDelayedPopup.setInterval(250);
    connect(&mDelayedPopup, &QTimer::timeout, this, &LXQtMainMenu::showHideMenu);
    mHideTimer.setSingleShot(true);
    mHideTimer.setInterval(250);

    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mButton.installEventFilter(this);

    connect(&mButton, &QToolButton::clicked, this, &LXQtMainMenu::showHideMenu);

    settingsChanged();

    mShortcut = GlobalKeyShortcut::Client::instance()->addAction(QString{}, QString("/panel/%1/show_hide").arg(settings()->group()), tr("Show/hide main menu"), this);
    if (mShortcut)
    {
        connect(mShortcut, &GlobalKeyShortcut::Action::registrationFinished, [this] {
            if (mShortcut->shortcut().isEmpty())
                mShortcut->changeShortcut(DEFAULT_SHORTCUT);
        });
        connect(mShortcut, &GlobalKeyShortcut::Action::activated, [this] { if (!mHideTimer.isActive()) mDelayedPopup.start(); });
    }
}


/************************************************

 ************************************************/
LXQtMainMenu::~LXQtMainMenu()
{
    mButton.removeEventFilter(this);
#ifdef HAVE_MENU_CACHE
    if(mMenuCache)
    {
        menu_cache_remove_reload_notify(mMenuCache, mMenuCacheNotify);
        menu_cache_unref(mMenuCache);
    }
#endif
}


/************************************************

 ************************************************/
void LXQtMainMenu::showHideMenu()
{
    if (mMenu && mMenu->isVisible())
        mMenu->hide();
    else
        showMenu();
}

/************************************************

 ************************************************/
void LXQtMainMenu::showMenu()
{
    if (!mMenu)
        return;

    willShowWindow(mMenu);
    // Just using Qt`s activateWindow() won't work on some WMs like Kwin.
    // Solution is to execute menu 1ms later using timer
    mMenu->popup(calculatePopupWindowPos(mMenu->sizeHint()).topLeft());
}

#ifdef HAVE_MENU_CACHE
// static
void LXQtMainMenu::menuCacheReloadNotify(MenuCache* cache, gpointer user_data)
{
    reinterpret_cast<LXQtMainMenu*>(user_data)->buildMenu();
}
#endif

/************************************************

 ************************************************/
void LXQtMainMenu::settingsChanged()
{
    mButton.setIcon(QIcon{settings()->value(QLatin1String("icon"), QLatin1String(LXQT_GRAPHICS_DIR"/helix.svg")).toString()});
    if (settings()->value("showText", false).toBool())
    {
        mButton.setText(settings()->value("text", "Start").toString());
        mButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else
    {
        mButton.setText("");
        mButton.setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    mLogDir = settings()->value("log_dir", "").toString();

    QString menu_file = settings()->value("menu_file", "").toString();
    if (menu_file.isEmpty())
        menu_file = XdgMenu::getMenuFileName();

    if (mMenuFile != menu_file)
    {
        mMenuFile = menu_file;
#ifdef HAVE_MENU_CACHE
        menu_cache_init(0);
        if(mMenuCache)
        {
            menu_cache_remove_reload_notify(mMenuCache, mMenuCacheNotify);
            menu_cache_unref(mMenuCache);
        }
        mMenuCache = menu_cache_lookup(mMenuFile.toLocal8Bit());
        if (menu_cache_get_root_dir(mMenuCache))
            buildMenu();
        mMenuCacheNotify = menu_cache_add_reload_notify(mMenuCache, (MenuCacheReloadNotify)menuCacheReloadNotify, this);
#else
        mXdgMenu.setEnvironments(QStringList() << "X-LXQT" << "LXQt");
        mXdgMenu.setLogDir(mLogDir);

        bool res = mXdgMenu.read(mMenuFile);
        connect(&mXdgMenu, SIGNAL(changed()), this, SLOT(buildMenu()));
        if (res)
        {
            QTimer::singleShot(1000, this, SLOT(buildMenu()));
        }
        else
        {
            QMessageBox::warning(0, "Parse error", mXdgMenu.errorString());
            return;
        }
#endif
    }

    setMenuFontSize();

    realign();
}


/************************************************

 ************************************************/
void LXQtMainMenu::buildMenu()
{
#ifdef HAVE_MENU_CACHE
    XdgCachedMenu* menu = new XdgCachedMenu(mMenuCache, &mButton);
#else
    XdgMenuWidget *menu = new XdgMenuWidget(mXdgMenu, "", &mButton);
#endif
    menu->setObjectName("TopLevelMainMenu");
    menu->setStyle(&mTopMenuStyle);

    menu->addSeparator();

    Q_FOREACH(QAction* action, menu->actions())
    {
        if (action->menu())
            action->menu()->installEventFilter(this);
    }

    menu->installEventFilter(this);
    connect(menu, &QMenu::aboutToHide, &mHideTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(menu, &QMenu::aboutToShow, &mHideTimer, &QTimer::stop);

    QMenu *oldMenu = mMenu;
    mMenu = menu;
    if(oldMenu)
        delete oldMenu;

    setMenuFontSize();
}

/************************************************

 ************************************************/
void LXQtMainMenu::setMenuFontSize()
{
    if (!mMenu)
        return;

    QFont menuFont = mButton.font();
    if(settings()->value("customFont", false).toBool())
    {
        menuFont = mMenu->font();
        menuFont.setPointSize(settings()->value("customFontSize").toInt());
    }

    if (mMenu->font() != menuFont)
    {
        mMenu->setFont(menuFont);
        QList<QMenu*> subMenuList = mMenu->findChildren<QMenu*>();
        foreach (QMenu* subMenu, subMenuList)
        {
            subMenu->setFont(menuFont);
        }
    }

    //icon size the same as the font height
    mTopMenuStyle.setIconSize(QFontMetrics(menuFont).height());
}


/************************************************

 ************************************************/
QDialog *LXQtMainMenu::configureDialog()
{
    return new LXQtMainMenuConfiguration(*settings(), mShortcut, DEFAULT_SHORTCUT);
}
/************************************************

 ************************************************/

// functor used to match a QAction by prefix
struct MatchAction
{
    MatchAction(QString key):key_(key) {}
    bool operator()(QAction* action) { return action->text().startsWith(key_, Qt::CaseInsensitive); }
    QString key_;
};

bool LXQtMainMenu::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == &mButton)
    {
        // the application is given a new QStyle
        if(event->type() == QEvent::StyleChange)
        {
            // reset proxy style for the menus so they can apply the new styles
            mTopMenuStyle.setBaseStyle(NULL);
            setMenuFontSize();
        }
    }
    else if(QMenu* menu = qobject_cast<QMenu*>(obj))
    {
        if(event->type() == QEvent::KeyPress)
        {
            // if our shortcut key is pressed while the menu is open, close the menu
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->modifiers() & ~Qt::ShiftModifier)
            {
                mHideTimer.start();
                mMenu->hide(); // close the app menu
                return true;
            }
            else // go to the menu item starts with the pressed key
            {
                QString key = keyEvent->text();
                if(key.isEmpty())
                    return false;
                QAction* action = menu->activeAction();
                QList<QAction*> actions = menu->actions();
                QList<QAction*>::iterator it = qFind(actions.begin(), actions.end(), action);
                it = std::find_if(it + 1, actions.end(), MatchAction(key));
                if(it == actions.end())
                    it = std::find_if(actions.begin(), it, MatchAction(key));
                if(it != actions.end())
                    menu->setActiveAction(*it);
            }
        }
    }
    return false;
}

#undef DEFAULT_SHORTCUT
