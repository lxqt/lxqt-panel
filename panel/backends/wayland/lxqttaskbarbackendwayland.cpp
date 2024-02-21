#include "lxqttaskbarbackendwayland.h"

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

LXQtTaskbarWaylandBackend::LXQtTaskbarWaylandBackend(QObject *parent) :
    ILXQtTaskbarAbstractBackend(parent)
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
                emit currentWorkspaceChanged(idx);
            });

    connect(m_workspaceInfo.get(), &LXQtPlasmaWaylandWorkspaceInfo::numberOfDesktopsChanged,
            this, &ILXQtTaskbarAbstractBackend::workspacesCountChanged);

    //TODO: connect name changed
}

bool LXQtTaskbarWaylandBackend::supportsAction(WId windowId, LXQtTaskBarBackendAction action) const
{
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

bool LXQtTaskbarWaylandBackend::reloadWindows()
{
    return false; //TODO
}

QVector<WId> LXQtTaskbarWaylandBackend::getCurrentWindows() const
{
    QVector<WId> wids(windows.size());
    for(const auto& window : std::as_const(windows))
    {
        wids << window->getWindowId();
    }
    return wids;
}

QString LXQtTaskbarWaylandBackend::getWindowTitle(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return QString();

    return window->title;
}

bool LXQtTaskbarWaylandBackend::applicationDemandsAttention(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_demands_attention) || transientsDemandingAttention.contains(window);
}

QIcon LXQtTaskbarWaylandBackend::getApplicationIcon(WId windowId, int devicePixels) const
{
    Q_UNUSED(devicePixels)

    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return QIcon();

    return window->icon;
}

QString LXQtTaskbarWaylandBackend::getWindowClass(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return QString();
    return window->appId;
}

LXQtTaskBarWindowLayer LXQtTaskbarWaylandBackend::getWindowLayer(WId windowId) const
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

bool LXQtTaskbarWaylandBackend::setWindowLayer(WId windowId, LXQtTaskBarWindowLayer layer)
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

LXQtTaskBarWindowState LXQtTaskbarWaylandBackend::getWindowState(WId windowId) const
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

bool LXQtTaskbarWaylandBackend::setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set)
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

bool LXQtTaskbarWaylandBackend::isWindowActive(WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return activeWindow == window || window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_active);
}

bool LXQtTaskbarWaylandBackend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
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

bool LXQtTaskbarWaylandBackend::closeWindow(WId windowId)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->close();
    return true;
}

WId LXQtTaskbarWaylandBackend::getActiveWindow() const
{
    if(activeWindow)
        return activeWindow->getWindowId();
    return 0;
}

int LXQtTaskbarWaylandBackend::getWorkspacesCount() const
{
    return m_workspaceInfo->numberOfDesktops();
}

QString LXQtTaskbarWaylandBackend::getWorkspaceName(int idx) const
{
    //TODO: optimize
    return m_workspaceInfo->desktopNames().value(idx, QStringLiteral("ERROR"));
}

int LXQtTaskbarWaylandBackend::getCurrentWorkspace() const
{
    return 0; //TODO
}

bool LXQtTaskbarWaylandBackend::setCurrentWorkspace(int idx)
{
    Q_UNUSED(idx)
    return false; //TODO
}

int LXQtTaskbarWaylandBackend::getWindowWorkspace(WId windowId) const
{
    Q_UNUSED(windowId)
    return 0; //TODO
}

bool LXQtTaskbarWaylandBackend::setWindowOnWorkspace(WId windowId, int idx)
{
    Q_UNUSED(windowId)
    Q_UNUSED(idx)
    return false; //TODO
}

void LXQtTaskbarWaylandBackend::moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop)
{

}

bool LXQtTaskbarWaylandBackend::isWindowOnScreen(QScreen *screen, WId windowId) const
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return screen->geometry().intersects(window->geometry);
}

void LXQtTaskbarWaylandBackend::moveApplication(WId windowId)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return;

    window->set_state(LXQtTaskBarPlasmaWindow::state::state_active, LXQtTaskBarPlasmaWindow::state::state_active);
    window->request_move();
}

void LXQtTaskbarWaylandBackend::resizeApplication(WId windowId)
{
    LXQtTaskBarPlasmaWindow *window = getWindow(windowId);
    if(!window)
        return;

    window->set_state(LXQtTaskBarPlasmaWindow::state::state_active, LXQtTaskBarPlasmaWindow::state::state_active);
    window->request_resize();
}

void LXQtTaskbarWaylandBackend::refreshIconGeometry(WId windowId, const QRect &geom)
{

}

bool LXQtTaskbarWaylandBackend::isAreaOverlapped(const QRect &area) const
{
    for(auto &window : std::as_const(windows))
    {
        if(window->geometry.intersects(area))
            return true;
    }
    return false;
}

bool LXQtTaskbarWaylandBackend::isShowingDesktop() const
{
    return m_managment->isShowingDesktop();
}

bool LXQtTaskbarWaylandBackend::showDesktop(bool value)
{
    enum LXQtTaskBarPlasmaWindowManagment::show_desktop flag_;
    if(value)
        flag_ = LXQtTaskBarPlasmaWindowManagment::show_desktop::show_desktop_enabled;
    else
        flag_ = LXQtTaskBarPlasmaWindowManagment::show_desktop::show_desktop_disabled;

    m_managment->show_desktop(flag_);
    return true;
}

void LXQtTaskbarWaylandBackend::addWindow(LXQtTaskBarPlasmaWindow *window)
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

bool LXQtTaskbarWaylandBackend::acceptWindow(LXQtTaskBarPlasmaWindow *window) const
{
    if(window->windowState.testFlag(LXQtTaskBarPlasmaWindow::state::state_skiptaskbar))
        return false;

    if(transients.contains(window))
        return false;

    return true;
}

void LXQtTaskbarWaylandBackend::updateWindowAcceptance(LXQtTaskBarPlasmaWindow *window)
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

LXQtTaskBarPlasmaWindow *LXQtTaskbarWaylandBackend::getWindow(WId windowId) const
{
    for(auto &window : std::as_const(windows))
    {
        if(window->getWindowId() == windowId)
            return window.get();
    }

    return nullptr;
}
