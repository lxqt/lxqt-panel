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


#include "lxqttaskbarbackend_x11.h"

#include <KX11Extras>
#include <KWindowSystem>
#include <KWindowInfo>

// Necessary for closeApplication()
#include <NETWM>

#include <QGuiApplication>
#include <QScreen>

#include <QTimer>

//NOTE: Xlib.h defines Bool which conflicts with QJsonValue::Type enum
#include <X11/Xlib.h>
#undef Bool

LXQtTaskbarX11Backend::LXQtTaskbarX11Backend(QObject *parent)
    : ILXQtTaskbarAbstractBackend(parent)
{
    auto *x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    Q_ASSERT_X(x11Application, "LXQtTaskbarX11Backend", "Constructed without X11 connection");
    m_X11Display = x11Application->display();
    m_xcbConnection = x11Application->connection();

    connect(KX11Extras::self(), &KX11Extras::windowChanged, this, &LXQtTaskbarX11Backend::onWindowChanged);
    connect(KX11Extras::self(), &KX11Extras::windowAdded, this, &LXQtTaskbarX11Backend::onWindowAdded);
    connect(KX11Extras::self(), &KX11Extras::windowRemoved, this, &LXQtTaskbarX11Backend::onWindowRemoved);

    connect(KX11Extras::self(), &KX11Extras::numberOfDesktopsChanged, this, &ILXQtTaskbarAbstractBackend::workspacesCountChanged);
    connect(KX11Extras::self(), &KX11Extras::currentDesktopChanged, this, &ILXQtTaskbarAbstractBackend::currentWorkspaceChanged);

    connect(KX11Extras::self(), &KX11Extras::activeWindowChanged,   this, &ILXQtTaskbarAbstractBackend::activeWindowChanged);
}

/************************************************
 *   Model slots
 ************************************************/
void LXQtTaskbarX11Backend::onWindowChanged(WId windowId, NET::Properties prop, NET::Properties2 prop2)
{
    if(!m_windows.contains(windowId))
    {
        // If already known window changes its property in a way
        // it's now acceptable, add it again to taskbar
        if(acceptWindow(windowId))
            onWindowAdded(windowId);
        return;
    }

    if(!acceptWindow(windowId))
    {
        // If already known window changes its property in a way
        // it's not anymore accepted, remove it from taskbar
        onWindowRemoved(windowId);
        return;
    }

    if (prop.testFlag(NET::WMGeometry))
    {
        emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::Geometry));
    }

    if (prop2.testFlag(NET::WM2WindowClass))
    {
        emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::WindowClass));
    }

    // window changed virtual desktop
    if (prop.testFlag(NET::WMDesktop) || prop.testFlag(NET::WMGeometry))
    {
        emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::Workspace));
    }

    if (prop.testFlag(NET::WMVisibleName) || prop.testFlag(NET::WMName))
        emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::Title));

    // XXX: we are setting window icon geometry -> don't need to handle NET::WMIconGeometry
    // Icon of the button can be based on windowClass
    if (prop.testFlag(NET::WMIcon) || prop2.testFlag(NET::WM2WindowClass))
        emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::Icon));

    bool update_urgency = false;
    if (prop2.testFlag(NET::WM2Urgency))
    {
        update_urgency = true;
    }

    if (prop.testFlag(NET::WMState))
    {
        update_urgency = true;

        emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::State));
    }

    if (update_urgency)
        emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::Urgency));
}

void LXQtTaskbarX11Backend::onWindowAdded(WId windowId)
{
    if(m_windows.contains(windowId))
        return;

    if (!acceptWindow(windowId))
        return;

    addWindow_internal(windowId);
}

void LXQtTaskbarX11Backend::onWindowRemoved(WId windowId)
{
    const int row = m_windows.indexOf(windowId);
    if(row == -1)
        return;

    m_windows.removeAt(row);

    emit windowRemoved(windowId);
}

/************************************************
 *   Model private functions
 ************************************************/
bool LXQtTaskbarX11Backend::acceptWindow(WId windowId) const
{
    QFlags<NET::WindowTypeMask> ignoreList;
    ignoreList |= NET::DesktopMask;
    ignoreList |= NET::DockMask;
    ignoreList |= NET::SplashMask;
    ignoreList |= NET::ToolbarMask;
    ignoreList |= NET::MenuMask;
    ignoreList |= NET::PopupMenuMask;
    ignoreList |= NET::NotificationMask;

    KWindowInfo info(windowId, NET::WMWindowType | NET::WMState, NET::WM2TransientFor);
    if (!info.valid())
        return false;

    if (NET::typeMatchesMask(info.windowType(NET::AllTypesMask), ignoreList))
        return false;

    if (info.state() & NET::SkipTaskbar)
        return false;

    // WM_TRANSIENT_FOR hint not set - normal window
    WId transFor = info.transientFor();

    WId appRootWindow = XDefaultRootWindow(m_X11Display);

    if (transFor == 0 || transFor == windowId || transFor == appRootWindow)
        return true;

    info = KWindowInfo(transFor, NET::WMWindowType);

    QFlags<NET::WindowTypeMask> normalFlag;
    normalFlag |= NET::NormalMask;
    normalFlag |= NET::DialogMask;
    normalFlag |= NET::UtilityMask;

    return !NET::typeMatchesMask(info.windowType(NET::AllTypesMask), normalFlag);
}

void LXQtTaskbarX11Backend::addWindow_internal(WId windowId, bool emitAdded)
{
    m_windows.append(windowId);
    if(emitAdded)
        emit windowAdded(windowId);
}


/************************************************
 *   Windows function
 ************************************************/
bool LXQtTaskbarX11Backend::supportsAction(WId windowId, LXQtTaskBarBackendAction action) const
{
    NET::Action x11Action;

    switch (action)
    {
    case LXQtTaskBarBackendAction::Move:
        x11Action = NET::ActionMove;
        break;

    case LXQtTaskBarBackendAction::Resize:
        x11Action = NET::ActionResize;
        break;

    case LXQtTaskBarBackendAction::Maximize:
        x11Action = NET::ActionMax;
        break;

    case LXQtTaskBarBackendAction::MaximizeVertically:
        x11Action = NET::ActionMaxVert;
        break;

    case LXQtTaskBarBackendAction::MaximizeHorizontally:
        x11Action = NET::ActionMaxHoriz;
        break;

    case LXQtTaskBarBackendAction::Minimize:
        x11Action = NET::ActionMinimize;
        break;

    case LXQtTaskBarBackendAction::RollUp:
        x11Action = NET::ActionShade;
        break;

    case LXQtTaskBarBackendAction::FullScreen:
        x11Action = NET::ActionFullScreen;
        break;

    case LXQtTaskBarBackendAction::DesktopSwitch:
        return true;

    default:
        return false;
    }

    KWindowInfo info(windowId, NET::Properties(), NET::WM2AllowedActions);
    return info.actionSupported(x11Action);
}

bool LXQtTaskbarX11Backend::reloadWindows()
{
    QVector<WId> oldWindows;
    qSwap(oldWindows, m_windows);

    // Just add new windows to groups, deleting is up to the groups
    const auto x11windows = KX11Extras::stackingOrder();
    for (auto const windowId: x11windows)
    {
        if (acceptWindow(windowId))
        {
            bool emitAdded = !oldWindows.contains(windowId);
            addWindow_internal(windowId, emitAdded);
        }
    }

    //emulate windowRemoved if known window not reported by KWindowSystem
    for (auto i = oldWindows.begin(), i_e = oldWindows.end(); i != i_e; i++)
    {
        WId windowId = *i;
        if (!m_windows.contains(windowId))
        {
            //TODO: more efficient method?
            emit windowRemoved(windowId);
        }
    }

    //TODO: refreshPlaceholderVisibility()
    emit reloaded();

    return true;
}

QVector<WId> LXQtTaskbarX11Backend::getCurrentWindows() const
{
    return m_windows;
}

QString LXQtTaskbarX11Backend::getWindowTitle(WId windowId) const
{
    KWindowInfo info(windowId, NET::WMVisibleName | NET::WMName);
    QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();
    return title;
}

bool LXQtTaskbarX11Backend::applicationDemandsAttention(WId windowId) const
{
    WId appRootWindow = XDefaultRootWindow(m_X11Display);
    return NETWinInfo(m_xcbConnection, windowId, appRootWindow, NET::Properties{}, NET::WM2Urgency).urgency()
           || KWindowInfo{windowId, NET::WMState}.hasState(NET::DemandsAttention);
}

QIcon LXQtTaskbarX11Backend::getApplicationIcon(WId windowId, int devicePixels) const
{
    return KX11Extras::icon(windowId, devicePixels, devicePixels);
}

QString LXQtTaskbarX11Backend::getWindowClass(WId windowId) const
{
    KWindowInfo info(windowId, NET::Properties(), NET::WM2WindowClass);
    return QString::fromUtf8(info.windowClassClass());
}

LXQtTaskBarWindowLayer LXQtTaskbarX11Backend::getWindowLayer(WId windowId) const
{
    NET::States state = KWindowInfo(windowId, NET::WMState).state();
    if(state.testFlag(NET::KeepAbove))
        return LXQtTaskBarWindowLayer::KeepAbove;
    else if(state.testFlag(NET::KeepBelow))
        return LXQtTaskBarWindowLayer::KeepBelow;
    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtTaskbarX11Backend::setWindowLayer(WId windowId, LXQtTaskBarWindowLayer layer)
{
    switch(layer)
    {
    case LXQtTaskBarWindowLayer::KeepAbove:
        KX11Extras::clearState(windowId, NET::KeepBelow);
        KX11Extras::setState(windowId, NET::KeepAbove);
        break;

    case LXQtTaskBarWindowLayer::KeepBelow:
        KX11Extras::clearState(windowId, NET::KeepAbove);
        KX11Extras::setState(windowId, NET::KeepBelow);
        break;

    default:
        KX11Extras::clearState(windowId, NET::KeepBelow);
        KX11Extras::clearState(windowId, NET::KeepAbove);
        break;
    }

    return true;
}

LXQtTaskBarWindowState LXQtTaskbarX11Backend::getWindowState(WId windowId) const
{
    KWindowInfo info(windowId,NET::WMState | NET::XAWMState);
    if(info.isMinimized())
        return LXQtTaskBarWindowState::Minimized;

    NET::States state = info.state();
    if(state.testFlag(NET::Hidden))
        return LXQtTaskBarWindowState::Hidden;
    if(state.testFlag(NET::Max))
        return LXQtTaskBarWindowState::Maximized;
    if(state.testFlag(NET::MaxHoriz))
        return LXQtTaskBarWindowState::MaximizedHorizontally;
    if(state.testFlag(NET::MaxVert))
        return LXQtTaskBarWindowState::MaximizedVertically;
    if(state.testFlag(NET::Shaded))
        return LXQtTaskBarWindowState::RolledUp;
    if(state.testFlag(NET::FullScreen))
        return LXQtTaskBarWindowState::FullScreen;

    return LXQtTaskBarWindowState::Normal;
}

bool LXQtTaskbarX11Backend::setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set)
{
    // NOTE: window activation is left to the caller

    NET::State x11State;

    switch (state)
    {
    case LXQtTaskBarWindowState::Minimized:
    {
        if(set)
            KX11Extras::minimizeWindow(windowId);
        else
            KX11Extras::unminimizeWindow(windowId);
        return true;
    }
    case LXQtTaskBarWindowState::Maximized:
    {
        x11State = NET::Max;
        break;
    }
    case LXQtTaskBarWindowState::MaximizedVertically:
    {
        x11State = NET::MaxVert;
        break;
    }
    case LXQtTaskBarWindowState::MaximizedHorizontally:
    {
        x11State = NET::MaxHoriz;
        break;
    }
    case LXQtTaskBarWindowState::Normal:
    {
        x11State = NET::Max; //TODO: correct?
        break;
    }
    case LXQtTaskBarWindowState::RolledUp:
    {
        x11State = NET::Shaded;
        break;
    }
    default:
        return false;
    }

    if(set)
        KX11Extras::setState(windowId, x11State);
    else
        KX11Extras::clearState(windowId, x11State);

    return true;
}

bool LXQtTaskbarX11Backend::isWindowActive(WId windowId) const
{
    return KX11Extras::activeWindow() == windowId;
}

bool LXQtTaskbarX11Backend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    if (onCurrentWorkSpace && getWindowState(windowId) == LXQtTaskBarWindowState::Minimized)
    {
        setWindowOnWorkspace(windowId, getCurrentWorkspace());
    }
    else
    {
        setCurrentWorkspace(getWindowWorkspace(windowId));
    }

    // bypass focus stealing prevention
    KX11Extras::forceActiveWindow(windowId);

    // Clear urgency flag
    emit windowPropertyChanged(windowId, int(LXQtTaskBarWindowProperty::Urgency));

    return true;
}

bool LXQtTaskbarX11Backend::closeWindow(WId windowId)
{
    // FIXME: Why there is no such thing in KWindowSystem??
    NETRootInfo(m_xcbConnection, NET::CloseWindow).closeWindowRequest(windowId);
    return true;
}

WId LXQtTaskbarX11Backend::getActiveWindow() const
{
    return KX11Extras::activeWindow();
}


/************************************************
 *   Workspaces
 ************************************************/
int LXQtTaskbarX11Backend::getWorkspacesCount() const
{
    return KX11Extras::numberOfDesktops();
}

QString LXQtTaskbarX11Backend::getWorkspaceName(int idx) const
{
    return KX11Extras::desktopName(idx);
}

int LXQtTaskbarX11Backend::getCurrentWorkspace() const
{
    return KX11Extras::currentDesktop();
}

bool LXQtTaskbarX11Backend::setCurrentWorkspace(int idx)
{
    if(KX11Extras::currentDesktop() == idx)
        return true;

    KX11Extras::setCurrentDesktop(idx);
    return true;
}

int LXQtTaskbarX11Backend::getWindowWorkspace(WId windowId) const
{
    KWindowInfo info(windowId, NET::WMDesktop);
    return info.desktop();
}

bool LXQtTaskbarX11Backend::setWindowOnWorkspace(WId windowId, int idx)
{
    KX11Extras::setOnDesktop(windowId, idx);
    return true;
}

void LXQtTaskbarX11Backend::moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop)
{
    KWindowInfo info(windowId, NET::WMDesktop);
    if (!info.isOnCurrentDesktop())
        KX11Extras::setCurrentDesktop(info.desktop());

    if (getWindowState(windowId) == LXQtTaskBarWindowState::Minimized)
        KX11Extras::unminimizeWindow(windowId);

    KX11Extras::forceActiveWindow(windowId);

    const QRect& windowGeometry = KWindowInfo(windowId, NET::WMFrameExtents).frameGeometry();
    QList<QScreen *> screens = QGuiApplication::screens();
    if (screens.size() > 1)
    {
        for (int i = 0; i < screens.size(); ++i)
        {
            QRect screenGeometry = screens[i]->geometry();
            if (screenGeometry.intersects(windowGeometry))
            {
                int targetScreen = i + (next ? 1 : -1);
                if (targetScreen < 0)
                    targetScreen += screens.size();
                else if (targetScreen >= screens.size())
                    targetScreen -= screens.size();

                QRect targetScreenGeometry = screens[targetScreen]->geometry();
                int X = windowGeometry.x() - screenGeometry.x() + targetScreenGeometry.x();
                int Y = windowGeometry.y() - screenGeometry.y() + targetScreenGeometry.y();
                NET::States state = KWindowInfo(windowId, NET::WMState).state();

                //      NW geometry |     y/x      |  from panel
                const int flags = 1 | (0b011 << 8) | (0b010 << 12);
                KX11Extras::clearState(windowId, NET::MaxHoriz | NET::MaxVert | NET::Max | NET::FullScreen);
                NETRootInfo(m_xcbConnection, NET::Properties(), NET::WM2MoveResizeWindow).moveResizeWindowRequest(windowId, flags, X, Y, 0, 0);
                QTimer::singleShot(200, this, [this, windowId, state, raiseOnCurrentDesktop]
                                   {
                                       KX11Extras::setState(windowId, state);
                                       raiseWindow(windowId, raiseOnCurrentDesktop);
                                   });
                break;
            }
        }
    }
}

bool LXQtTaskbarX11Backend::isWindowOnScreen(QScreen *screen, WId windowId) const
{
    //TODO: old code was:
    //return QApplication::desktop()->screenGeometry(parentTaskBar()).intersects(KWindowInfo(mWindow, NET::WMFrameExtents).frameGeometry());

    if(!screen)
        return true;

    QRect r = KWindowInfo(windowId, NET::WMFrameExtents).frameGeometry();
    return screen->geometry().intersects(r);
}

bool LXQtTaskbarX11Backend::setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft)
{
    NETRootInfo mDesktops(m_xcbConnection, NET::NumberOfDesktops | NET::CurrentDesktop | NET::DesktopNames, NET::WM2DesktopLayout);

    if (orientation == Qt::Horizontal)
    {
        mDesktops.setDesktopLayout(NET::OrientationHorizontal,
                                   columns, rows,
                                   rightToLeft ? NET::DesktopLayoutCornerTopRight : NET::DesktopLayoutCornerTopLeft);
    }
    else
    {
        mDesktops.setDesktopLayout(NET::OrientationHorizontal,
                                   rows, columns,
                                   rightToLeft ? NET::DesktopLayoutCornerTopRight : NET::DesktopLayoutCornerTopLeft);
    }
    return true;
}

/************************************************
 *   X11 Specific
 ************************************************/
void LXQtTaskbarX11Backend::moveApplication(WId windowId)
{
    KWindowInfo info(windowId, NET::WMDesktop);
    if (!info.isOnCurrentDesktop())
        KX11Extras::setCurrentDesktop(info.desktop());

    if (getWindowState(windowId) == LXQtTaskBarWindowState::Minimized)
        KX11Extras::unminimizeWindow(windowId);

    KX11Extras::forceActiveWindow(windowId);

    const QRect& g = KWindowInfo(windowId, NET::WMGeometry).geometry();
    int X = g.center().x();
    int Y = g.center().y();
    QCursor::setPos(X, Y);
    NETRootInfo(m_xcbConnection, NET::WMMoveResize).moveResizeRequest(windowId, X, Y, NET::Move);
}

void LXQtTaskbarX11Backend::resizeApplication(WId windowId)
{
    KWindowInfo info(windowId, NET::WMDesktop);
    if (!info.isOnCurrentDesktop())
        KX11Extras::setCurrentDesktop(info.desktop());

    if (getWindowState(windowId) == LXQtTaskBarWindowState::Minimized)
        KX11Extras::unminimizeWindow(windowId);

    KX11Extras::forceActiveWindow(windowId);

    const QRect& g = KWindowInfo(windowId, NET::WMGeometry).geometry();
    int X = g.bottomRight().x();
    int Y = g.bottomRight().y();
    QCursor::setPos(X, Y);
    NETRootInfo(m_xcbConnection, NET::WMMoveResize).moveResizeRequest(windowId, X, Y, NET::BottomRight);
}

void LXQtTaskbarX11Backend::refreshIconGeometry(WId windowId, QRect const & geom)
{
    // NOTE: This function announces where the task icon is,
    // such that X11 WMs can perform their related animations correctly.

    WId appRootWindow = XDefaultRootWindow(m_X11Display);

    NETWinInfo info(m_xcbConnection,
                    windowId,
                    appRootWindow,
                    NET::WMIconGeometry,
                    NET::Properties2());
    NETRect const curr = info.iconGeometry();

    // see kwindowsystem -> NETWinInfo::setIconGeometry for the scale factor
    const qreal scaleFactor = qApp->devicePixelRatio();
    int xPos = geom.x() * scaleFactor;
    int yPos = geom.y() * scaleFactor;
    int w = geom.width() * scaleFactor;
    int h = geom.height() * scaleFactor;
    if (xPos == curr.pos.x && yPos == curr.pos.y && w == curr.size.width && h == curr.size.height)
        return;
    NETRect nrect;
    nrect.pos.x = geom.x();
    nrect.pos.y = geom.y();
    nrect.size.height = geom.height();
    nrect.size.width = geom.width();
    info.setIconGeometry(nrect);
}

bool LXQtTaskbarX11Backend::isAreaOverlapped(const QRect &area) const
{
    //TODO: reuse our m_windows cache?
    QFlags<NET::WindowTypeMask> ignoreList;
    ignoreList |= NET::DesktopMask;
    ignoreList |= NET::DockMask;
    ignoreList |= NET::SplashMask;
    ignoreList |= NET::MenuMask;
    ignoreList |= NET::PopupMenuMask;
    ignoreList |= NET::DropdownMenuMask;
    ignoreList |= NET::TopMenuMask;
    ignoreList |= NET::NotificationMask;

    const auto wIds = KX11Extras::stackingOrder();
    for (auto const wId : wIds)
    {
        KWindowInfo info(wId, NET::WMWindowType | NET::WMState | NET::WMFrameExtents | NET::WMDesktop);
        if (info.valid()
            // skip windows that are on other desktops
            && info.isOnCurrentDesktop()
            // skip shaded, minimized or hidden windows
            && !(info.state() & (NET::Shaded | NET::Hidden))
            // check against the list of ignored types
            && !NET::typeMatchesMask(info.windowType(NET::AllTypesMask), ignoreList))
        {
            if (info.frameGeometry().intersects(area))
                return true;
        }
    }
    return false;
}

bool LXQtTaskbarX11Backend::isShowingDesktop() const
{
    return KWindowSystem::showingDesktop();
}

bool LXQtTaskbarX11Backend::showDesktop(bool value)
{
    KWindowSystem::setShowingDesktop(value);
    return true;
}
