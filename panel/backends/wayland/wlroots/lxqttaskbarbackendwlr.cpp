#include "lxqttaskbarbackendwlr.h"

#include "lxqttaskbarwlrwindowmanagment.h"
#include "lxqtwlrvirtualdesktop.h"

#include <QIcon>
#include <QTime>
#include <QScreen>

auto findWindow(const std::vector<std::unique_ptr<LXQtTaskBarWlrootsWindow>>& windows, LXQtTaskBarWlrootsWindow *window)
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

LXQtTaskbarWlrootsBackend::LXQtTaskbarWlrootsBackend(QObject *parent) :
    ILXQtTaskbarAbstractBackend(parent)
{
    m_managment.reset(new LXQtTaskBarWlrootsWindowManagment);
    m_workspaceInfo.reset(new LXQtWlrootsWaylandWorkspaceInfo);

    connect(m_managment.get(), &LXQtTaskBarWlrootsWindowManagment::windowCreated, this, [this](LXQtTaskBarWlrootsWindow *window) {
        addWindow(window);
    });

    // connect(m_managment.get(), &LXQtTaskBarWlrootsWindowManagment::stackingOrderChanged,
    //         this, [this](const QString &order) {
    //     // stackingOrder = order.split(QLatin1Char(';'));
    //     // for (const auto &window : std::as_const(windows)) {
    //     //     this->dataChanged(window.get(), StackingOrder);
    //     // }
    // });

    connect(m_workspaceInfo.get(), &LXQtWlrootsWaylandWorkspaceInfo::currentDesktopChanged, this,
            [this](){
                int idx = m_workspaceInfo->position(m_workspaceInfo->currentDesktop());
                idx += 1; // Make 1-based
                emit currentWorkspaceChanged(idx);
            });

    connect(m_workspaceInfo.get(), &LXQtWlrootsWaylandWorkspaceInfo::numberOfDesktopsChanged,
            this, &ILXQtTaskbarAbstractBackend::workspacesCountChanged);

    connect(m_workspaceInfo.get(), &LXQtWlrootsWaylandWorkspaceInfo::desktopNameChanged,
            this, [this](int idx) {
        emit workspaceNameChanged(idx + 1); // Make 1-based
    });
}

bool LXQtTaskbarWlrootsBackend::supportsAction(WId, LXQtTaskBarBackendAction action) const
{
    switch (action)
    {
    case LXQtTaskBarBackendAction::Move:
        return false;

    case LXQtTaskBarBackendAction::Resize:
        return false;

    case LXQtTaskBarBackendAction::Maximize:
        return true;

    case LXQtTaskBarBackendAction::Minimize:
        return true;

    case LXQtTaskBarBackendAction::RollUp:
        return false;

    case LXQtTaskBarBackendAction::FullScreen:
        return true;

    default:
        return false;
    }

    return false;
}

bool LXQtTaskbarWlrootsBackend::reloadWindows()
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

QVector<WId> LXQtTaskbarWlrootsBackend::getCurrentWindows() const
{
    QVector<WId> wids;
    wids.reserve(wids.size());

    for(const std::unique_ptr<LXQtTaskBarWlrootsWindow>& window : std::as_const(windows))
    {
        wids << window->getWindowId();
    }
    return wids;
}

QString LXQtTaskbarWlrootsBackend::getWindowTitle(WId windowId) const
{
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return QString();

    return window->title;
}

bool LXQtTaskbarWlrootsBackend::applicationDemandsAttention(WId) const
{
    return false;
}

QIcon LXQtTaskbarWlrootsBackend::getApplicationIcon(WId windowId, int devicePixels) const
{
    Q_UNUSED(devicePixels)

    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return QIcon();

    return window->icon;
}

QString LXQtTaskbarWlrootsBackend::getWindowClass(WId windowId) const
{
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return QString();
    return window->appId;
}

LXQtTaskBarWindowLayer LXQtTaskbarWlrootsBackend::getWindowLayer(WId) const
{
    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtTaskbarWlrootsBackend::setWindowLayer(WId, LXQtTaskBarWindowLayer)
{
}

LXQtTaskBarWindowState LXQtTaskbarWlrootsBackend::getWindowState(WId windowId) const
{
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return LXQtTaskBarWindowState::Normal;

    if(window->windowState.testFlag(LXQtTaskBarWlrootsWindow::state::state_minimized))
        return LXQtTaskBarWindowState::Hidden;

    if(window->windowState.testFlag(LXQtTaskBarWlrootsWindow::state::state_maximized))
        return LXQtTaskBarWindowState::Maximized;

    if(window->windowState.testFlag(LXQtTaskBarWlrootsWindow::state::state_fullscreen))
        return LXQtTaskBarWindowState::FullScreen;

    return LXQtTaskBarWindowState::Normal;
}

bool LXQtTaskbarWlrootsBackend::setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set)
{
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    LXQtTaskBarWlrootsWindow::state WlrootsState;
    switch (state)
    {
    case LXQtTaskBarWindowState::Minimized:
    {
        WlrootsState = LXQtTaskBarWlrootsWindow::state::state_minimized;
        break;
    }
    case LXQtTaskBarWindowState::Maximized:
    case LXQtTaskBarWindowState::MaximizedVertically:
    case LXQtTaskBarWindowState::MaximizedHorizontally:
    {
        WlrootsState = LXQtTaskBarWlrootsWindow::state::state_maximized;
        break;
    }
    case LXQtTaskBarWindowState::Normal:
    {
        WlrootsState = LXQtTaskBarWlrootsWindow::state::state_maximized;
        set = !set; //TODO: correct
        break;
    }
    case LXQtTaskBarWindowState::RolledUp:
    {
        break;
    }
    default:
        return false;
    }

    window->set_state(WlrootsState, set ? WlrootsState : 0);
    return true;
}

bool LXQtTaskbarWlrootsBackend::isWindowActive(WId windowId) const
{
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return activeWindow == window || window->windowState.testFlag(LXQtTaskBarWlrootsWindow::state::state_activated);
}

bool LXQtTaskbarWlrootsBackend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    Q_UNUSED(onCurrentWorkSpace) //TODO

    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->activate();
}

bool LXQtTaskbarWlrootsBackend::closeWindow(WId windowId)
{
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->close();
    return true;
}

WId LXQtTaskbarWlrootsBackend::getActiveWindow() const
{
    if(activeWindow)
        return activeWindow->getWindowId();
    return 0;
}

int LXQtTaskbarWlrootsBackend::getWorkspacesCount() const
{
    return m_workspaceInfo->numberOfDesktops();
}

QString LXQtTaskbarWlrootsBackend::getWorkspaceName(int idx) const
{
    return m_workspaceInfo->getDesktopName(idx - 1); //Return to 0-based
}

int LXQtTaskbarWlrootsBackend::getCurrentWorkspace() const
{
    return 1;
}

bool LXQtTaskbarWlrootsBackend::setCurrentWorkspace(int)
{
    return false;
}

int LXQtTaskbarWlrootsBackend::getWindowWorkspace(WId windowId) const
{
    return 1;
}

bool LXQtTaskbarWlrootsBackend::setWindowOnWorkspace(WId, int)
{
}

void LXQtTaskbarWlrootsBackend::moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop)
{
}

bool LXQtTaskbarWlrootsBackend::isWindowOnScreen(QScreen *screen, WId windowId) const
{
    // Manage based on output-enter/output-leave
    return true;
}

void LXQtTaskbarWlrootsBackend::moveApplication(WId windowId)
{
}

void LXQtTaskbarWlrootsBackend::resizeApplication(WId windowId)
{
}

void LXQtTaskbarWlrootsBackend::refreshIconGeometry(WId windowId, const QRect &geom)
{

}

bool LXQtTaskbarWlrootsBackend::isAreaOverlapped(const QRect &area) const
{
}

bool LXQtTaskbarWlrootsBackend::isShowingDesktop() const
{
    return m_managment->isShowingDesktop();
}

bool LXQtTaskbarWlrootsBackend::showDesktop(bool value)
{
}

void LXQtTaskbarWlrootsBackend::addWindow(LXQtTaskBarWlrootsWindow *window)
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
                if (LXQtTaskBarWlrootsWindow *leader = transientsDemandingAttention.key(window)) {
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

    connect(window, &LXQtTaskBarWlrootsWindow::closed, this, removeWindow);

    connect(window, &LXQtTaskBarWlrootsWindow::titleChanged, this, [window, this] {
        updateWindowAcceptance(window);
    });

    connect(window, &LXQtTaskBarWlrootsWindow::appIdChanged, this, [window, this] {
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::WindowClass));
    });

    if (window->windowState & LXQtTaskBarWlrootsWindow::state::state_activated) {
        LXQtTaskBarWlrootsWindow *effectiveActive = window;
        while (effectiveActive->parentWindow) {
            effectiveActive = effectiveActive->parentWindow;
        }

        lastActivated[effectiveActive] = QTime::currentTime();
        activeWindow = effectiveActive;
    }

    connect(window, &LXQtTaskBarWlrootsWindow::activeChanged, this, [window, this] {
        const bool active = window->windowState & LXQtTaskBarWlrootsWindow::state::state_activated;

        LXQtTaskBarWlrootsWindow *effectiveWindow = window;

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

    connect(window, &LXQtTaskBarWlrootsWindow::parentChanged, this, [window, this] {
        LXQtTaskBarWlrootsWindow *leader = window->parentWindow.data();

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
    };

    connect(window, &LXQtTaskBarWlrootsWindow::fullscreenChanged, this, stateChanged);

    connect(window, &LXQtTaskBarWlrootsWindow::maximizedChanged, this, stateChanged);

    connect(window, &LXQtTaskBarWlrootsWindow::minimizedChanged, this, stateChanged);

    // Handle transient.
    if (LXQtTaskBarWlrootsWindow *leader = window->parentWindow.data())
    {
        transients.insert(window, leader);
    }
    else
    {
        windows.emplace_back(window);
        updateWindowAcceptance(window);
    }

    emit windowAdded( window->getWindowId() );
}

bool LXQtTaskbarWlrootsBackend::acceptWindow(LXQtTaskBarWlrootsWindow *window) const
{
    if(transients.contains(window))
        return false;

    return true;
}

void LXQtTaskbarWlrootsBackend::updateWindowAcceptance(LXQtTaskBarWlrootsWindow *)
{
}

LXQtTaskBarWlrootsWindow *LXQtTaskbarWlrootsBackend::getWindow(WId windowId) const
{
    for(auto &window : std::as_const(windows))
    {
        if(window->getWindowId() == windowId)
            return window.get();
    }

    return nullptr;
}
