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

#include "lxqtwmbackend_kwinwayland.h"

#include "lxqttaskbarplasmawindowmanagment.h"
#include "lxqtplasmavirtualdesktop.h"

#include <QIcon>
#include <QTime>
#include <QScreen>

auto findWindow(const std::vector<std::unique_ptr<LXQtTaskBarPlasmaWindow>>& windows, LXQtTaskBarPlasmaWindow *window)
{
    //TODO: use algorithms
    auto end = windows.end();
    for(auto it = windows.begin(); it != end; it++)
    {
        if((*it).get() == window)
        {
            return it;
        }
    }

    return windows.end();
}

LXQtWMBackend_KWinWayland::LXQtWMBackend_KWinWayland(QObject *parent) :
    ILXQtAbstractWMInterface(parent)
{
    m_managment.reset(new LXQtTaskBarPlasmaWindowManagment);
    m_workspaceInfo.reset(new LXQtPlasmaWaylandWorkspaceInfo);

    connect(m_managment.get(), &LXQtTaskBarPlasmaWindowManagment::windowCreated, this, [this](LXQtTaskBarPlasmaWindow *window) {
        connect(window, &LXQtTaskBarPlasmaWindow::initialStateDone, this, [this, window] {
            addWindow(window);
        });
    });

    // connect(m_managment.get(), &LXQtTaskBarPlasmaWindowManagment::stackingOrderChanged,
    //         this, [this](const QString &order) {
    //     // stackingOrder = order.split(QLatin1Char(';'));
    //     // for (const auto &window : std::as_const(windows)) {
    //     //     this->dataChanged(window.get(), StackingOrder);
    //     // }
    // });

    connect(m_workspaceInfo.get(), &LXQtPlasmaWaylandWorkspaceInfo::currentDesktopChanged, this,
            [this](){
                int idx = m_workspaceInfo->position(m_workspaceInfo->currentDesktop());
                idx += 1; // Make 1-based
                emit currentWorkspaceChanged(idx);
            });

    connect(m_workspaceInfo.get(), &LXQtPlasmaWaylandWorkspaceInfo::numberOfDesktopsChanged,
            this, &ILXQtAbstractWMInterface::workspacesCountChanged);

    connect(m_workspaceInfo.get(), &LXQtPlasmaWaylandWorkspaceInfo::desktopNameChanged,
            this, [this](int idx) {
                emit workspaceNameChanged(idx + 1); // Make 1-based
            });
}

bool LXQtWMBackend_KWinWayland::supportsAction(WId windowId, LXQtTaskBarBackendAction action) const
{
    if(action == LXQtTaskBarBackendAction::DesktopSwitch)
        return true;

    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    LXQtTaskBarPlasmaWindow::state state;

    switch (action)
    {
    case LXQtTaskBarBackendAction::Move:
        state = LXQtTaskBarPlasmaWindow::state::state_movable;
        break;

    case LXQtTaskBarBackendAction::Resize:
        state = LXQtTaskBarPlasmaWindow::state::state_resizable;
        break;

    case LXQtTaskBarBackendAction::Maximize:
        state = LXQtTaskBarPlasmaWindow::state::state_maximizable;
        break;

    case LXQtTaskBarBackendAction::Minimize:
        state = LXQtTaskBarPlasmaWindow::state::state_minimizable;
        break;

    case LXQtTaskBarBackendAction::RollUp:
        state = LXQtTaskBarPlasmaWindow::state::state_shadeable;
        break;

    case LXQtTaskBarBackendAction::FullScreen:
        state = LXQtTaskBarPlasmaWindow::state::state_fullscreenable;
        break;

    default:
        return false;
    }

    return window->windowState.testFlag(state);
}

bool LXQtWMBackend_KWinWayland::reloadWindows()
{
    const QVector<WId> wids = getCurrentWindows();

    // Force removal and re-adding
    for(WId windowId : wids)
    {
        emit windowRemoved(windowId);
    }
    for(WId windowId : wids)
    {
        emit windowAdded(windowId);
    }

    return true;
}

QVector<WId> LXQtWMBackend_KWinWayland::getCurrentWindows() const
{
    QVector<WId> wids;
    wids.reserve(wids.size());

    for(const std::unique_ptr<LXQtTaskBarPlasmaWindow>& window : std::as_const(windows))
    {
        if(window->acceptedInTaskBar)
            wids << window->getWindowId();
    }
    return wids;
}

QString LXQtWMBackend_KWinWayland::getWindowTitle(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return QString();

    return window->title;
}

bool LXQtWMBackend_KWinWayland::applicationDemandsAttention(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_demands_attention) || transientsDemandingAttention.contains(window);
}

QIcon LXQtWMBackend_KWinWayland::getApplicationIcon(WId windowId, int devicePixels) const
{
    Q_UNUSED(devicePixels)

    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return QIcon();

    return window->icon;
}

QString LXQtWMBackend_KWinWayland::getWindowClass(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return QString();
    return window->appId;
}

LXQtTaskBarWindowLayer LXQtWMBackend_KWinWayland::getWindowLayer(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return LXQtTaskBarWindowLayer::Normal;

    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_keep_above))
        return LXQtTaskBarWindowLayer::KeepAbove;

    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_keep_below))
        return LXQtTaskBarWindowLayer::KeepBelow;

    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtWMBackend_KWinWayland::setWindowLayer(WId windowId, LXQtTaskBarWindowLayer layer)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    if(getWindowLayer(windowId) == layer)
        return true; //TODO: make more efficient

    LXQtTaskBarPlasmaWindow::state plasmaState = LXQtTaskBarPlasmaWindow::state::state_keep_above;
    switch (layer)
    {
    case LXQtTaskBarWindowLayer::Normal:
    case LXQtTaskBarWindowLayer::KeepAbove:
        break;
    case LXQtTaskBarWindowLayer::KeepBelow:
        plasmaState = LXQtTaskBarPlasmaWindow::state::state_keep_below;
        break;
    default:
        return false;
    }

    window->set_state(plasmaState, layer == LXQtTaskBarWindowLayer::Normal ? 0 : plasmaState);
    return false;
}

LXQtTaskBarWindowState LXQtWMBackend_KWinWayland::getWindowState(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return LXQtTaskBarWindowState::Normal;

    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_minimized))
        return LXQtTaskBarWindowState::Hidden;
    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_maximizable))
        return LXQtTaskBarWindowState::Maximized;
    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_shaded))
        return LXQtTaskBarWindowState::RolledUp;
    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_fullscreen))
        return LXQtTaskBarWindowState::FullScreen;

    return LXQtTaskBarWindowState::Normal;
}

bool LXQtWMBackend_KWinWayland::setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    LXQtTaskBarPlasmaWindow::state plasmaState;
    switch (state)
    {
    case LXQtTaskBarWindowState::Minimized:
    {
        plasmaState = LXQtTaskBarPlasmaWindow::state::state_minimized;
        break;
    }
    case LXQtTaskBarWindowState::Maximized:
    case LXQtTaskBarWindowState::MaximizedVertically:
    case LXQtTaskBarWindowState::MaximizedHorizontally:
    {
        plasmaState = LXQtTaskBarPlasmaWindow::state::state_maximized;
        break;
    }
    case LXQtTaskBarWindowState::Normal:
    {
        plasmaState = LXQtTaskBarPlasmaWindow::state::state_maximized;
        set = !set; //TODO: correct
        break;
    }
    case LXQtTaskBarWindowState::RolledUp:
    {
        plasmaState = LXQtTaskBarPlasmaWindow::state::state_shaded;
        break;
    }
    default:
        return false;
    }

    window->set_state(plasmaState, set ? plasmaState : 0);
    return true;
}

bool LXQtWMBackend_KWinWayland::isWindowActive(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return activeWindow == window || window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_active);
}

bool LXQtWMBackend_KWinWayland::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    Q_UNUSED(onCurrentWorkSpace) //TODO

    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    // Pull forward any transient demanding attention.
    if (auto *transientDemandingAttention = transientsDemandingAttention.value(window))
    {
        window = transientDemandingAttention;
    }
    else
    {
        // TODO Shouldn't KWin take care of that?
        // Bringing a transient to the front usually brings its parent with it
        // but focus is not handled properly.
        // TODO take into account d->lastActivation instead
        // of just taking the first one.
        while (transients.key(window))
        {
            window = transients.key(window);
        }
    }

    window->set_state(LXQtTaskBarPlasmaWindow::state::state_active, LXQtTaskBarPlasmaWindow::state::state_active);
    return true;
}

bool LXQtWMBackend_KWinWayland::closeWindow(WId windowId)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->close();
    return true;
}

WId LXQtWMBackend_KWinWayland::getActiveWindow() const
{
    if(activeWindow)
        return activeWindow->getWindowId();
    return 0;
}

int LXQtWMBackend_KWinWayland::getWorkspacesCount() const
{
    return m_workspaceInfo->numberOfDesktops();
}

QString LXQtWMBackend_KWinWayland::getWorkspaceName(int idx) const
{
    return m_workspaceInfo->getDesktopName(idx - 1); //Return to 0-based
}

int LXQtWMBackend_KWinWayland::getCurrentWorkspace() const
{
    if(!m_workspaceInfo->currentDesktop().isValid())
        return 0;
    return m_workspaceInfo->position(m_workspaceInfo->currentDesktop()) + 1; // 1-based
}

bool LXQtWMBackend_KWinWayland::setCurrentWorkspace(int idx)
{
    QString id = m_workspaceInfo->getDesktopId(idx - 1); //Return to 0-based
    if(id.isEmpty())
        return false;
    m_workspaceInfo->requestActivate(id);
    return true;
}

int LXQtWMBackend_KWinWayland::getWindowWorkspace(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return 0;

    // TODO: this protocol seems to allow multiple desktop for each window
    // We do not support that yet
    // Also from KDE Plasma task switch it's not clear how to actually put
    // a window on multiple desktops (which is different from "All desktops")
    QString id = window->virtualDesktops.value(0, QString());
    if(id.isEmpty())
        return 0;

    return m_workspaceInfo->position(id) + 1; //Make 1-based
}

bool LXQtWMBackend_KWinWayland::setWindowOnWorkspace(WId windowId, int idx)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    // Prepare for future multiple virtual desktops per window
    QList<QString> newDesktops;

    // Fill the list
    newDesktops.append(m_workspaceInfo->getDesktopId(idx - 1)); //Return to 0-based

    // Keep only valid IDs
    newDesktops.erase(std::remove_if(newDesktops.begin(), newDesktops.end(),
                                     [](const QString& id) { return id.isEmpty(); }),
                      newDesktops.end());

    // Add to new requested desktops
    for(const QString& id : std::as_const(newDesktops))
    {
        if(!window->virtualDesktops.contains(id))
            window->request_enter_virtual_desktop(id);
    }

    // Remove from non-requested destops
    const QList<QString> currentDesktops = window->virtualDesktops;
    for(const QString& id : std::as_const(currentDesktops))
    {
        if(!newDesktops.contains(id))
            window->request_leave_virtual_desktop(id);
    }

    return true;
}

void LXQtWMBackend_KWinWayland::moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop)
{

}

bool LXQtWMBackend_KWinWayland::isWindowOnScreen(QScreen *screen, WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return screen->geometry().intersects(window->geometry);
}

bool LXQtWMBackend_KWinWayland::setDesktopLayout(Qt::Orientation, int, int, bool)
{
    //TODO: implement
    return false;
}

void LXQtWMBackend_KWinWayland::moveApplication(WId windowId)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return;

    window->set_state(LXQtTaskBarPlasmaWindow::state::state_active, LXQtTaskBarPlasmaWindow::state::state_active);
    window->request_move();
}

void LXQtWMBackend_KWinWayland::resizeApplication(WId windowId)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return;

    window->set_state(LXQtTaskBarPlasmaWindow::state::state_active, LXQtTaskBarPlasmaWindow::state::state_active);
    window->request_resize();
}

void LXQtWMBackend_KWinWayland::refreshIconGeometry(WId windowId, const QRect &geom)
{

}

bool LXQtWMBackend_KWinWayland::isAreaOverlapped(const QRect &area) const
{
    for(auto &window : std::as_const(windows))
    {
        if(window->geometry.intersects(area))
            return true;
    }
    return false;
}

bool LXQtWMBackend_KWinWayland::isShowingDesktop() const
{
    return m_managment->isActive() ? m_managment->isShowingDesktop() : false;
}

bool LXQtWMBackend_KWinWayland::showDesktop(bool value)
{
    if(!m_managment->isActive())
        return false;

    enum LXQtTaskBarPlasmaWindowManagment::show_desktop flag_;
    if(value)
        flag_ = LXQtTaskBarPlasmaWindowManagment::show_desktop::show_desktop_enabled;
    else
        flag_ = LXQtTaskBarPlasmaWindowManagment::show_desktop::show_desktop_disabled;

    m_managment->show_desktop(flag_);
    return true;
}

void LXQtWMBackend_KWinWayland::addWindow(LXQtTaskBarPlasmaWindow *window)
{
    if (findWindow(windows, window) != windows.end() || transients.contains(window))
    {
        return;
    }

    auto removeWindow = [window, this]
    {
        auto it = findWindow(windows, window);
        if (it != windows.end())
        {
            if(window->acceptedInTaskBar)
                emit windowRemoved(window->getWindowId());
            windows.erase(it);
            transientsDemandingAttention.remove(window);
            lastActivated.remove(window);
        }
        else
        {
            // Could be a transient.
            // Removing a transient might change the demands attention state of the leader.
            if (transients.remove(window))
            {
                if (LXQtTaskBarPlasmaWindow *leader = transientsDemandingAttention.key(window)) {
                    transientsDemandingAttention.remove(leader, window);
                    emit windowPropertyChanged(leader->getWindowId(), int(LXQtTaskBarWindowProperty::Urgency));
                }
            }
        }

        if (activeWindow == window)
        {
            activeWindow = nullptr;
            emit activeWindowChanged(0);
        }
    };

    connect(window, &LXQtTaskBarPlasmaWindow::unmapped, this, removeWindow);

    connect(window, &LXQtTaskBarPlasmaWindow::titleChanged, this, [window, this] {
        updateWindowAcceptance(window);
        if(window->acceptedInTaskBar)
            emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Title));
    });

    connect(window, &LXQtTaskBarPlasmaWindow::iconChanged, this, [window, this] {
        updateWindowAcceptance(window);
        if(window->acceptedInTaskBar)
            emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Icon));
    });

    connect(window, &LXQtTaskBarPlasmaWindow::geometryChanged, this, [window, this] {
        updateWindowAcceptance(window);
        if(window->acceptedInTaskBar)
            emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Geometry));
    });

    connect(window, &LXQtTaskBarPlasmaWindow::appIdChanged, this, [window, this] {
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::WindowClass));
    });

    if (window->windowState & LXQtTaskBarPlasmaWindow::state::state_active) {
        LXQtTaskBarPlasmaWindow *effectiveActive = window;
        while (effectiveActive->parentWindow) {
            effectiveActive = effectiveActive->parentWindow;
        }

        lastActivated[effectiveActive] = QTime::currentTime();
        activeWindow = effectiveActive;
    }

    connect(window, &LXQtTaskBarPlasmaWindow::activeChanged, this, [window, this] {
        const bool active = window->windowState & LXQtTaskBarPlasmaWindow::state::state_active;

        LXQtTaskBarPlasmaWindow *effectiveWindow = window;

        while (effectiveWindow->parentWindow)
        {
            effectiveWindow = effectiveWindow->parentWindow;
        }

        if (active)
        {
            lastActivated[effectiveWindow] = QTime::currentTime();

            if (activeWindow != effectiveWindow)
            {
                activeWindow = effectiveWindow;
                emit activeWindowChanged(activeWindow->getWindowId());
            }
        }
        else
        {
            if (activeWindow == effectiveWindow)
            {
                activeWindow = nullptr;
                emit activeWindowChanged(0);
            }
        }
    });

    connect(window, &LXQtTaskBarPlasmaWindow::parentWindowChanged, this, [window, this] {
        LXQtTaskBarPlasmaWindow *leader = window->parentWindow.data();

        // Migrate demanding attention to new leader.
        if (window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_demands_attention))
        {
            if (auto *oldLeader = transientsDemandingAttention.key(window))
            {
                if (window->parentWindow != oldLeader)
                {
                    transientsDemandingAttention.remove(oldLeader, window);
                    transientsDemandingAttention.insert(leader, window);
                    emit windowPropertyChanged(oldLeader->getWindowId(), int(LXQtTaskBarWindowProperty::Urgency));
                    emit windowPropertyChanged(leader->getWindowId(), int(LXQtTaskBarWindowProperty::Urgency));
                }
            }
        }

        if (transients.remove(window))
        {
            if (leader)
            {
                // leader change.
                transients.insert(window, leader);
            }
            else
            {
                // lost a leader, add to regular windows list.
                Q_ASSERT(findWindow(windows, window) == windows.end());

                windows.emplace_back(window);
            }
        }
        else if (leader)
        {
            // gained a leader, remove from regular windows list.
            auto it = findWindow(windows, window);
            Q_ASSERT(it != windows.end());

            windows.erase(it);
            lastActivated.remove(window);
        }
    });

    auto stateChanged = [window, this] {
        updateWindowAcceptance(window);
        if(window->acceptedInTaskBar)
            emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::State));
    };

    connect(window, &LXQtTaskBarPlasmaWindow::fullscreenChanged, this, stateChanged);

    connect(window, &LXQtTaskBarPlasmaWindow::maximizedChanged, this, stateChanged);

    connect(window, &LXQtTaskBarPlasmaWindow::minimizedChanged, this, stateChanged);

    connect(window, &LXQtTaskBarPlasmaWindow::shadedChanged, this, stateChanged);

    auto workspaceChanged = [window, this] {
        updateWindowAcceptance(window);
        if(window->acceptedInTaskBar)
            emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Workspace));
    };

    connect(window, &LXQtTaskBarPlasmaWindow::virtualDesktopEntered, this, workspaceChanged);
    connect(window, &LXQtTaskBarPlasmaWindow::virtualDesktopLeft, this, workspaceChanged);

    connect(window, &LXQtTaskBarPlasmaWindow::demandsAttentionChanged, this, [window, this] {
        // Changes to a transient's state might change demands attention state for leader.
        if (auto *leader = transients.value(window))
        {
            if (window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_demands_attention))
            {
                if (!transientsDemandingAttention.values(leader).contains(window))
                {
                    transientsDemandingAttention.insert(leader, window);
                    emit windowPropertyChanged(leader->getWindowId(), int(LXQtTaskBarWindowProperty::Urgency));
                }
            }
            else if (transientsDemandingAttention.remove(window))
            {
                emit windowPropertyChanged(leader->getWindowId(), int(LXQtTaskBarWindowProperty::Urgency));
            }
        }
        else
        {
            emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Urgency));
        }
    });

    connect(window, &LXQtTaskBarPlasmaWindow::skipTaskbarChanged, this, [window, this] {
        updateWindowAcceptance(window);
    });

    // QObject::connect(window, &PlasmaWindow::applicationMenuChanged, q, [window, this] {
    //     this->dataChanged(window, QList<int>{ApplicationMenuServiceName, ApplicationMenuObjectPath});
    // });

    // Handle transient.
    if (LXQtTaskBarPlasmaWindow *leader = window->parentWindow.data())
    {
        transients.insert(window, leader);

        // Update demands attention state for leader.
        if (window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_demands_attention))
        {
            transientsDemandingAttention.insert(leader, window);
            if(leader->acceptedInTaskBar)
                emit windowPropertyChanged(leader->getWindowId(), int(LXQtTaskBarWindowProperty::Urgency));
        }
    }
    else
    {
        windows.emplace_back(window);
        updateWindowAcceptance(window);
    }
}

bool LXQtWMBackend_KWinWayland::acceptWindow(LXQtTaskBarPlasmaWindow *window) const
{
    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_skiptaskbar))
        return false;

    if(transients.contains(window))
        return false;

    return true;
}

void LXQtWMBackend_KWinWayland::updateWindowAcceptance(LXQtTaskBarPlasmaWindow *window)
{
    if(!window->acceptedInTaskBar && acceptWindow(window))
    {
        window->acceptedInTaskBar = true;
        emit windowAdded(window->getWindowId());
    }
    else if(window->acceptedInTaskBar && !acceptWindow(window))
    {
        window->acceptedInTaskBar = false;
        emit windowRemoved(window->getWindowId());
    }
}

LXQtTaskBarPlasmaWindow *LXQtWMBackend_KWinWayland::getWindow(WId windowId) const
{
    for(auto &window : std::as_const(windows))
    {
        if(window->getWindowId() == windowId)
            return window.get();
    }

    return nullptr;
}

int LXQtWMBackendKWinWaylandLibrary::getBackendScore(const QString &key) const
{
    auto *waylandApplication = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if(!waylandApplication)
        return 0;

    // Detect KWin Plasma
    if(key == QLatin1String("KDE") || key == QLatin1String("KWIN"))
        return 100;

    // It's not useful for other wayland compositors
    return 0;
}

ILXQtAbstractWMInterface *LXQtWMBackendKWinWaylandLibrary::instance() const
{
    return new LXQtWMBackend_KWinWayland;
}
