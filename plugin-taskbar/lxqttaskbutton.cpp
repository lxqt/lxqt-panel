/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
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


#include <QDebug>
#include <XdgIcon>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>

#include "lxqttaskbutton.h"
#include <LXQt/XfitMan>
#include <QX11Info>

using namespace LxQt;

/************************************************

************************************************/
void ElidedButtonStyle::drawItemText(QPainter* painter, const QRect& rect,
                    int flags, const QPalette & pal, bool enabled,
                  const QString & text, QPalette::ColorRole textRole) const
{
    QString s = painter->fontMetrics().elidedText(text, Qt::ElideRight, rect.width());
    QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, s, textRole);
}


/************************************************

************************************************/
LxQtTaskButton::LxQtTaskButton(const Window window, QWidget *parent) :
    QToolButton(parent),
    mWindow(window)
{
    setCheckable(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMinimumWidth(1);
    setMinimumHeight(1);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setAcceptDrops(true);

    updateText();
    updateIcon();

    connect(this, SIGNAL(toggled(bool)), this, SLOT(checkedChanged(bool)));

    XWindowAttributes oldAttr;
    XGetWindowAttributes(QX11Info::display(), mWindow, &oldAttr);

    XSetWindowAttributes newAttr;
    newAttr.event_mask = oldAttr.your_event_mask | PropertyChangeMask;
    XChangeWindowAttributes(QX11Info::display(), mWindow, CWEventMask, &newAttr);

    setStyle(&mStyle);
}


/************************************************

************************************************/
LxQtTaskButton::~LxQtTaskButton()
{
    if (mCheckedBtn == this)
        mCheckedBtn = 0;
}


/************************************************

 ************************************************/
void LxQtTaskButton::updateText()
{
    QString title = xfitMan().getWindowTitle(mWindow);
    setText(title.replace("&", "&&"));
    setToolTip(title);
}


/************************************************

 ************************************************/
void LxQtTaskButton::updateIcon()
{
    QIcon ico;
    if (xfitMan().getClientIcon(mWindow, &ico))
        setIcon(ico);
    else
        setIcon(XdgIcon::defaultApplicationIcon());
}


/************************************************

 ************************************************/
void LxQtTaskButton::setShowOnlyCurrentDesktopTasks(bool value)
{
    mShowOnlyCurrentDesktopTasks = value;
}


/************************************************

 ************************************************/
void LxQtTaskButton::setCloseOnMiddleClick(bool value)
{
    mCloseOnMiddleClick = value;
}

/************************************************

 ************************************************/
void LxQtTaskButton::nextCheckState()
{
    setChecked(xfitMan().getActiveAppWindow() == mWindow);
}


/************************************************

 ************************************************/
void LxQtTaskButton::dragEnterEvent(QDragEnterEvent *event)
{
    mDraggableMimeData = event->mimeData();
    QTimer::singleShot(1000, this, SLOT(activateWithDraggable()));
}


/************************************************

 ************************************************/
void LxQtTaskButton::dragLeaveEvent(QDragLeaveEvent *event)
{
    mDraggableMimeData = NULL;
}


/************************************************

 ************************************************/
void LxQtTaskButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (this->isChecked())
        {
            minimizeApplication();
        }
        else
        {
            raiseApplication();
        }
    }
    else if (mCloseOnMiddleClick && event->button() == Qt::MidButton)
    {
        closeApplication();
    }
}

/************************************************

 ************************************************/
void LxQtTaskButton::checkedChanged(bool checked)
{

    if (checked)
    {
        if (mCheckedBtn != 0 && mCheckedBtn != this)
            mCheckedBtn->setChecked(false);

        mCheckedBtn = this;
    }
}


/************************************************

 ************************************************/
void LxQtTaskButton::unCheckAll()
{
    if (mCheckedBtn)
        mCheckedBtn->setChecked(false);
}


/************************************************

 ************************************************/
bool LxQtTaskButton::isAppHidden() const
{
    return xfitMan().isHidden(mWindow);
}


/************************************************

 ************************************************/
bool LxQtTaskButton::isApplicationActive() const
{
    return xfitMan().getActiveAppWindow() == mWindow;
}


/************************************************

 ************************************************/
void LxQtTaskButton::activateWithDraggable()
{
    if (!mDraggableMimeData)
        return;
    // raise app in any time when there is a drag
    // in progress to allow drop it into an app
    raiseApplication();
}

/************************************************

 ************************************************/
void LxQtTaskButton::raiseApplication()
{
    int winDesktop = xfitMan().getWindowDesktop(mWindow);
    if (xfitMan().getActiveDesktop() != winDesktop)
        xfitMan().setActiveDesktop(winDesktop);
    xfitMan().raiseWindow(mWindow);

    setUrgencyHint(false);
}


/************************************************

 ************************************************/
void LxQtTaskButton::minimizeApplication()
{
    xfitMan().minimizeWindow(mWindow);
}


/************************************************

 ************************************************/
void LxQtTaskButton::maximizeApplication()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    if (act->data().toInt() == XfitMan::MaximizeHoriz)
        xfitMan().maximizeWindow(mWindow, XfitMan::MaximizeHoriz);

    else if (act->data().toInt() == XfitMan::MaximizeVert)
        xfitMan().maximizeWindow(mWindow, XfitMan::MaximizeVert);

    else
        xfitMan().maximizeWindow(mWindow, XfitMan::MaximizeBoth);
}


/************************************************

 ************************************************/
void LxQtTaskButton::deMaximizeApplication()
{
    XfitMan().raiseWindow(mWindow);
    xfitMan().deMaximizeWindow(mWindow);
}


/************************************************

 ************************************************/
void LxQtTaskButton::shadeApplication()
{
    xfitMan().shadeWindow(mWindow, true);
}


/************************************************

 ************************************************/
void LxQtTaskButton::unShadeApplication()
{
    xfitMan().shadeWindow(mWindow, false);
}


/************************************************

 ************************************************/
void LxQtTaskButton::closeApplication()
{
    xfitMan().closeWindow(mWindow);
}


/************************************************

 ************************************************/
void LxQtTaskButton::setApplicationLayer()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    if (act->data().toInt() == XfitMan::LayerAbove)
        xfitMan().setWindowLayer(mWindow, XfitMan::LayerAbove);

    else if (act->data().toInt() == XfitMan::LayerBelow)
        xfitMan().setWindowLayer(mWindow, XfitMan::LayerBelow);

    else
        xfitMan().setWindowLayer(mWindow, XfitMan::LayerNormal);
}


/************************************************

 ************************************************/
void LxQtTaskButton::moveApplicationToDesktop()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    bool ok;
    int desk = act->data().toInt(&ok);

    if (!ok)
        return;

    xfitMan().moveWindowToDesktop(mWindow, desk);
}


/************************************************

 ************************************************/
void LxQtTaskButton::contextMenuEvent(QContextMenuEvent* event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        event->ignore();
        return;
    }


    XfitMan xf = xfitMan();

    WindowAllowedActions allow = xf.getAllowedActions(mWindow);
    WindowState state = xf.getWindowState(mWindow);

//    qDebug() << "Context menu " << xfitMan().getName(mWindow);
//    qDebug() << "  Allowed Actions:";
//    qDebug() << "    * Move          " << allow.Move;
//    qDebug() << "    * Resize        " << allow.Resize;
//    qDebug() << "    * Minimize      " << allow.Minimize;
//    qDebug() << "    * Shade         " << allow.Shade;
//    qDebug() << "    * Stick         " << allow.Stick;
//    qDebug() << "    * MaximizeHoriz " << allow.MaximizeHoriz;
//    qDebug() << "    * MaximizeVert  " << allow.MaximizeVert;
//    qDebug() << "    * FullScreen    " << allow.FullScreen;
//    qDebug() << "    * ChangeDesktop " << allow.ChangeDesktop;
//    qDebug() << "    * Close         " << allow.Close;
//    qDebug() << "    * AboveLayer    " << allow.AboveLayer;
//    qDebug() << "    * BelowLayer    " << allow.BelowLayer;
//    qDebug();
//    qDebug() << "  Window State:";
//    qDebug() << "    * Modal         " << state.Modal;
//    qDebug() << "    * Sticky        " << state.Sticky;
//    qDebug() << "    * MaximizedVert " << state.MaximizedVert;
//    qDebug() << "    * MaximizedHoriz" << state.MaximizedHoriz;
//    qDebug() << "    * Shaded        " << state.Shaded;
//    qDebug() << "    * SkipTaskBar   " << state.SkipTaskBar;
//    qDebug() << "    * SkipPager     " << state.SkipPager;
//    qDebug() << "    * Hidden        " << state.Hidden;
//    qDebug() << "    * FullScreen    " << state.FullScreen;
//    qDebug() << "    * AboveLayer    " << state.AboveLayer;
//    qDebug() << "    * BelowLayer    " << state.BelowLayer;
//    qDebug() << "    * Attention     " << state.Attention;

    QMenu menu(tr("Application"));
    QAction* a;

    /* KDE menu *******

      + To &Desktop >
      +     &All Desktops
      +     ---
      +     &1 Desktop 1
      +     &2 Desktop 2
      + &To Current Desktop
        &Move
        Re&size
      + Mi&nimize
      + Ma&ximize
      + &Shade
        Ad&vanced >
            Keep &Above Others
            Keep &Below Others
            Fill screen
        &Layer >
            Always on &top
            &Normal
            Always on &bottom
      ---
      + &Close
    */

    // ** Desktop menu **************************
    int deskNum = xf.getNumDesktop();
    if (deskNum > 1)
    {
        int winDesk = xf.getWindowDesktop(mWindow);
        QMenu* deskMenu = menu.addMenu(tr("To &Desktop"));

        a = deskMenu->addAction(tr("&All Desktops"));
        a->setData(-1);
        a->setEnabled(winDesk != -1);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        deskMenu->addSeparator();

        for (int i=0; i<deskNum; ++i)
        {
            a = deskMenu->addAction(tr("Desktop &%1").arg(i+1));
            a->setData(i);
            a->setEnabled( i != winDesk );
            connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        }

        int curDesk = xf.getActiveDesktop();
        a = menu.addAction(tr("&To Current Desktop"));
        a->setData(curDesk);
        a->setEnabled( curDesk != winDesk );
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
    }

    // ** State menu ****************************
    menu.addSeparator();

    a = menu.addAction(tr("Ma&ximize"));
    a->setEnabled((allow.MaximizeHoriz && allow.MaximizeVert) &&
                  (!state.MaximizedHoriz || !state.MaximizedVert /*|| state.Hidden*/));
    a->setData(XfitMan::MaximizeBoth);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

    if (event->modifiers() & Qt::ShiftModifier)
    {

        a = menu.addAction(tr("Maximize vertically"));
        a->setEnabled((allow.MaximizeVert) && (!state.MaximizedVert || state.Hidden));
        a->setData(XfitMan::MaximizeVert);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

        a = menu.addAction(tr("Maximize horizontally"));
        a->setEnabled((allow.MaximizeHoriz) && (!state.MaximizedHoriz || state.Hidden));
        a->setData(XfitMan::MaximizeHoriz);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));
    }

    a = menu.addAction(tr("&Restore"));
    a->setEnabled(state.Hidden || state.MaximizedHoriz || state.MaximizedVert);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(deMaximizeApplication()));

    a = menu.addAction(tr("Mi&nimize"));
    a->setEnabled(allow.Minimize && !state.Hidden);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(minimizeApplication()));

    if (state.Shaded)
    {
        a = menu.addAction(tr("Roll down"));
        a->setEnabled(allow.Shade && !state.Hidden);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(unShadeApplication()));
    }
    else
    {
        a = menu.addAction(tr("Roll up"));
        a->setEnabled(allow.Shade && !state.Hidden);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(shadeApplication()));
    }


    // ** Layer menu ****************************
    menu.addSeparator();

    QMenu* layerMenu = menu.addMenu(tr("&Layer"));

    a = layerMenu->addAction(tr("Always on &top"));
    a->setEnabled(allow.AboveLayer && !state.AboveLayer);
    a->setData(XfitMan::LayerAbove);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("&Normal"));
    a->setEnabled(state.AboveLayer || state.BelowLayer);
    a->setData( XfitMan::LayerNormal);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("Always on &bottom"));
    a->setEnabled(allow.BelowLayer && !state.BelowLayer);
    a->setData(XfitMan::LayerBelow);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));


    // ** Kill menu *****************************
    menu.addSeparator();
    a = menu.addAction(XdgIcon::fromTheme("process-stop"), tr("&Close"));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(closeApplication()));
    menu.exec(mapToGlobal(event->pos()));
}


/************************************************

 ************************************************/
void  LxQtTaskButton::handlePropertyNotify(XEventType* event)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    xcb_property_notify_event_t* prop_event = reinterpret_cast<xcb_property_notify_event_t*>(event);
#else
    XPropertyEvent* prop_event = reinterpret_cast<XPropertyEvent*>(event);
#endif
    // I suppose here that only new/update values need to
    // be promoted here. There is no need to update inof
    // If it's deleted/about to delete. And mainly - it prevents
    // "BadWindow (invalid Window parameter)" errors:
    // Issue #4 BadWindow when a window is closed
    if (prop_event->state == PropertyDelete)
    {
//        qDebug() << "LxQtTaskButton::handlePropertyNotify to delete";
        return;
    }

    if (prop_event->atom == XfitMan::atom("WM_NAME") ||
        prop_event->atom == XfitMan::atom("_NET_WM_VISIBLE_NAME"))
    {
        updateText();
        return;
    }

    if (prop_event->atom == XfitMan::atom("_NET_WM_ICON"))
    {
        updateIcon();
        return;
    }

    if (prop_event->atom == XfitMan::atom("_NET_WM_DESKTOP"))
    {
        if (mShowOnlyCurrentDesktopTasks)
        {
            int desktop = desktopNum();
            setHidden(desktop != -1 && desktop != xfitMan().getActiveDesktop());
        }
        return;
    }

    if (prop_event->atom == XfitMan::atom("WM_HINTS"))
    {
        WMHintsFlags flags = XfitMan().getWMHintsFlags(prop_event->window);
        if (flags & WMUrgencyHint)
        {
            if (prop_event->window != xfitMan().getActiveAppWindow())
                setUrgencyHint(true);
        }
        else
            setUrgencyHint(false);
        return;
    }

//     char* aname = XGetAtomName(QX11Info::display(), event->atom);
//     qDebug() << "** XPropertyEvent ********************";
//     qDebug() << "  atom:       0x" << hex << event->atom
//             << " (" << (aname ? aname : "Unknown") << ')';
//     qDebug() << "  window:      " << event->window;
}


/************************************************

 ************************************************/
void LxQtTaskButton::setUrgencyHint(bool set)
{
    mUrgencyHint = set;
    setProperty("urgent", set);
    style()->unpolish(this);
    style()->polish(this);
    update();
}


/************************************************

 ************************************************/
int LxQtTaskButton::desktopNum() const
{
    return xfitMan().getWindowDesktop(mWindow);
}


LxQtTaskButton* LxQtTaskButton::mCheckedBtn = 0;

bool LxQtTaskButton::mShowOnlyCurrentDesktopTasks = false;
bool LxQtTaskButton::mCloseOnMiddleClick = true;
