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
#include <KF5/KWindowSystem/KWindowSystem>

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

/************************************************

 ************************************************/
KeyPressEventFilter::KeyPressEventFilter(QMenu *menu, QObject *parent):
    QObject(parent),
    mPreviousAction(0)
{
    mMenu = menu;
}

/************************************************

 ************************************************/
bool KeyPressEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        QMenu* menu=0;

        //This finds out which menu received the key event
        if (obj == mMenu)
            menu = mMenu;

        else
            foreach (QAction* action, mMenu->actions())
                if (action->menu() && action->menu() == obj)
                {
                    menu = action->menu();
                    break;
                }

        if (menu == 0)
            return QObject::eventFilter(obj, event);

        QAction* currentAction = 0;

        //If there is another QAction after the previous selected one, it gets selected
        if (menu->actions().indexOf(mPreviousAction) > -1)
            for (int i=menu->actions().indexOf(mPreviousAction)+1; i<menu->actions().count(); i++)
                if (menu->actions().at(i)->text().startsWith(QChar(keyEvent->key()), Qt::CaseInsensitive))
                {
                    currentAction = menu->actions().at(i);
                    break;
                }

        //Else the first matching QAction (if any) inside the menu gets selected
        if (!currentAction)
            foreach (QAction* action, menu->actions())
                if (action->text().startsWith(QChar(keyEvent->key()), Qt::CaseInsensitive))
                {
                    currentAction = action;
                    break;
                }

        if (currentAction)
        {
            mPreviousAction = currentAction;
            menu->setActiveAction(currentAction);
            if (currentAction->menu())
                currentAction->menu()->hide();
        }
    }

    return QObject::eventFilter(obj, event);
}

/************************************************

 ************************************************/
LxQtMainMenu::LxQtMainMenu(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mMenu(0),
    mShortcut(0),
    mLockCascadeChanges(false)
{
#ifdef HAVE_MENU_CACHE
    mMenuCache = NULL;
    mMenuCacheNotify = 0;
#endif

    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mButton.installEventFilter(this);

    connect(&mButton, SIGNAL(clicked()), this, SLOT(showMenu()));

    mPowerManager = new LxQt::PowerManager(this);
    mScreenSaver = new LxQt::ScreenSaver(this);

    settingsChanged();

    connect(mShortcut, SIGNAL(activated()), this, SLOT(showHideMenu()));
    connect(mShortcut, SIGNAL(shortcutChanged(QString,QString)), this, SLOT(shortcutChanged(QString,QString)));
}


/************************************************

 ************************************************/
LxQtMainMenu::~LxQtMainMenu()
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
void LxQtMainMenu::showHideMenu()
{
    if (mMenu && mMenu->isVisible())
        mMenu->hide();
    else
        showMenu();
}


/************************************************

 ************************************************/
void LxQtMainMenu::shortcutChanged(const QString &/*oldShortcut*/, const QString &newShortcut)
{
    if (!newShortcut.isEmpty())
    {
        mLockCascadeChanges = true;

        settings()->setValue("dialog/shortcut", newShortcut);
        settings()->sync();

        mLockCascadeChanges = false;
    }
}


/************************************************

 ************************************************/
void LxQtMainMenu::showMenu()
{
    if (!mMenu)
        return;

    int x=0, y=0;

    switch (panel()->position())
    {
        case ILxQtPanel::PositionTop:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->globalGometry().bottom();
            break;

        case ILxQtPanel::PositionBottom:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->globalGometry().top() - mMenu->sizeHint().height();
            break;

        case ILxQtPanel::PositionLeft:
            x = panel()->globalGometry().right();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;

        case ILxQtPanel::PositionRight:
            x = panel()->globalGometry().left() - mMenu->sizeHint().width();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;
    }

    // Just using Qt`s activateWindow() won't work on some WMs like Kwin.
    // Solution is to execute menu 1ms later using timer
    mButton.activateWindow();
    mMenu->exec(QPoint(x, y));
}

#ifdef HAVE_MENU_CACHE
// static
void LxQtMainMenu::menuCacheReloadNotify(MenuCache* cache, gpointer user_data)
{
    reinterpret_cast<LxQtMainMenu*>(user_data)->buildMenu();
}
#endif

/************************************************

 ************************************************/
void LxQtMainMenu::settingsChanged()
{
    if (mLockCascadeChanges)
        return;

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

    QString mMenuFile = settings()->value("menu_file", "").toString();
    if (mMenuFile.isEmpty())
        mMenuFile = XdgMenu::getMenuFileName();

#ifdef HAVE_MENU_CACHE
    menu_cache_init(0);
    if(mMenuCache)
    {
        menu_cache_remove_reload_notify(mMenuCache, mMenuCacheNotify);
        menu_cache_unref(mMenuCache);
    }
    mMenuCache = menu_cache_lookup(mMenuFile.toLocal8Bit());
    mMenuCacheNotify = menu_cache_add_reload_notify(mMenuCache, (MenuCacheReloadNotify)menuCacheReloadNotify, this);
#else
    mXdgMenu.setEnvironments(QStringList() << "X-LXQT" << "LxQt");
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

    QString shortcut = settings()->value("shortcut", DEFAULT_SHORTCUT).toString();
    if (shortcut.isEmpty())
        shortcut = DEFAULT_SHORTCUT;

    if (!mShortcut)
        mShortcut = GlobalKeyShortcut::Client::instance()->addAction(shortcut, QString("/panel/%1/show_hide").arg(settings()->group()), tr("Show/hide main menu"), this);
    else if (mShortcut->shortcut() != shortcut)
    {
        mShortcut->changeShortcut(shortcut);
    }

    realign();
}


/************************************************

 ************************************************/
void LxQtMainMenu::buildMenu()
{
#ifdef HAVE_MENU_CACHE
    XdgCachedMenu* menu = new XdgCachedMenu(mMenuCache, &mButton);
#else
    XdgMenuWidget *menu = new XdgMenuWidget(mXdgMenu, "", &mButton);
#endif
    menu->setObjectName("TopLevelMainMenu");
    menu->setStyle(&mTopMenuStyle);

    menu->addSeparator();

    QMenu* leaveMenu = menu->addMenu(XdgIcon::fromTheme("system-shutdown"), tr("Leave"));
    leaveMenu->addActions(mPowerManager->availableActions());
    menu->addActions(mScreenSaver->availableActions());

    mEventFilter = new KeyPressEventFilter(menu, this);
    foreach (QAction* action, menu->actions())
    {
        if (action->menu())
            action->menu()->installEventFilter(mEventFilter);
    }

    menu->installEventFilter(mEventFilter);

    QMenu *oldMenu = mMenu;
    mMenu = menu;
    delete oldMenu;

    if(settings()->value("customFont", false).toBool())
    {
        QFont menuFont = mMenu->font();
        menuFont.setPointSize(settings()->value("customFontSize").toInt());

        mMenu->setFont(menuFont);
        QList<QMenu*> subMenuList = mMenu->findChildren<QMenu*>();
        foreach (QMenu* subMenu, subMenuList)
        {
            subMenu->setFont(menuFont);
        }
    }
}


/************************************************

 ************************************************/
QDialog *LxQtMainMenu::configureDialog()
{
    return new LxQtMainMenuConfiguration(*settings(), DEFAULT_SHORTCUT);
}
/************************************************

 ************************************************/
bool LxQtMainMenu::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == &mButton)
    {
        // the application is given a new QStyle
        if(event->type() == QEvent::StyleChange)
        {
            // reset proxy style for the menus so they can apply the new styles
            mTopMenuStyle.setBaseStyle(NULL);
            mMenuStyle.setBaseStyle(NULL);
        }
    }
    return false;
}

#undef DEFAULT_SHORTCUT
