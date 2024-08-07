#include "lxqttaskbarbackendwlr.h"

#include "lxqttaskbarwlrwindowmanagment.h"
#include "lxqtwlrvirtualdesktop.h"

#include <QIcon>
#include <QTime>
#include <QScreen>
#include <algorithm>

// Function to search for a window in the vector
WId findWindow(const std::vector<WId>& windows, WId tgt) {
    // Use std::find to locate the target window
    auto it = std::find(windows.begin(), windows.end(), tgt);

    // Check if the window was found (iterator points to windows.end() if not found)
    if (it != windows.end()) {
        // If found, return the window ID by dereferencing the iterator
        return *it;
    }

    return 0;
}

// Function to erase a window from the vector
void eraseWindow(std::vector<WId>& windows, WId tgt) {
    // Use std::vector::iterator to find the window
    auto it = std::find(windows.begin(), windows.end(), tgt);

    // Check if the window was found
    if (it != windows.end()) {
        // If found, erase the element pointed to by the iterator
        windows.erase(it);
    }
}

LXQtTaskbarWlrootsBackend::LXQtTaskbarWlrootsBackend(QObject *parent) :
    ILXQtTaskbarAbstractBackend(parent)
{
    m_managment.reset(new LXQtTaskbarWlrootsWindowManagment);
    m_workspaceInfo.reset(new LXQtWlrootsWaylandWorkspaceInfo);

    connect(m_managment.get(), &LXQtTaskbarWlrootsWindowManagment::windowCreated, this, &LXQtTaskbarWlrootsBackend::addWindow);
}

bool LXQtTaskbarWlrootsBackend::supportsAction(WId, LXQtTaskBarBackendAction action) const
{
    switch (action)
    {
    case LXQtTaskBarBackendAction::Maximize:
        return true;

    case LXQtTaskBarBackendAction::Minimize:
        return true;

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

    for( WId wid: windows ){
        wids << wid;
    }

    return wids;
}

QString LXQtTaskbarWlrootsBackend::getWindowTitle(WId windowId) const
{
    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return QString();

    return window->windowState.title;
}

bool LXQtTaskbarWlrootsBackend::applicationDemandsAttention(WId) const
{
    return false;
}

QIcon LXQtTaskbarWlrootsBackend::getApplicationIcon(WId windowId, int devicePixels) const
{
    Q_UNUSED(devicePixels)

    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return QIcon();

    return window->icon;
}

QString LXQtTaskbarWlrootsBackend::getWindowClass(WId windowId) const
{
    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return QString();
    return window->windowState.appId;
}

LXQtTaskBarWindowLayer LXQtTaskbarWlrootsBackend::getWindowLayer(WId) const
{
    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtTaskbarWlrootsBackend::setWindowLayer(WId, LXQtTaskBarWindowLayer)
{
    return false;
}

LXQtTaskBarWindowState LXQtTaskbarWlrootsBackend::getWindowState(WId windowId) const
{
    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return LXQtTaskBarWindowState::Normal;

    if(window->windowState.minimized)
        return LXQtTaskBarWindowState::Minimized;

    if(window->windowState.maximized)
        return LXQtTaskBarWindowState::Maximized;

    if(window->windowState.fullscreen)
        return LXQtTaskBarWindowState::FullScreen;

    return LXQtTaskBarWindowState::Normal;
}

bool LXQtTaskbarWlrootsBackend::setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set)
{
    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    switch (state)
    {
    case LXQtTaskBarWindowState::Minimized:
    {
        if ( set ) {
            window->set_minimized();
        }

        else {
            window->unset_minimized();
        }

        break;
    }
    case LXQtTaskBarWindowState::Maximized:
    case LXQtTaskBarWindowState::MaximizedVertically:
    case LXQtTaskBarWindowState::MaximizedHorizontally:
    {
        if ( set ) {
            window->set_maximized();
        }

        else {
            window->unset_maximized();
        }

        break;
    }
    case LXQtTaskBarWindowState::Normal:
    {
        if (set)
        {
            if ( window->windowState.maximized) {
                window->unset_maximized();
            }
        }

        break;
    }

    case LXQtTaskBarWindowState::FullScreen:
    {
        if ( set ) {
            window->set_fullscreen(nullptr);
        }

        else {
            window->unset_fullscreen();
        }
        break;
    }

    default:
        return false;
    }

    return true;
}

bool LXQtTaskbarWlrootsBackend::isWindowActive(WId windowId) const
{
    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return activeWindow == windowId || window->windowState.activated;
}

bool LXQtTaskbarWlrootsBackend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    Q_UNUSED(onCurrentWorkSpace) // Cannot be done on a generic wlroots-based compositor!

    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->activate();
    return true;
}

bool LXQtTaskbarWlrootsBackend::closeWindow(WId windowId)
{
    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->close();
    return true;
}

WId LXQtTaskbarWlrootsBackend::getActiveWindow() const
{
    return activeWindow;
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

int LXQtTaskbarWlrootsBackend::getWindowWorkspace(WId) const
{
    return 1;
}

bool LXQtTaskbarWlrootsBackend::setWindowOnWorkspace(WId, int)
{
    return false;
}

void LXQtTaskbarWlrootsBackend::moveApplicationToPrevNextMonitor(WId, bool, bool)
{
}

bool LXQtTaskbarWlrootsBackend::isWindowOnScreen(QScreen *, WId) const
{
    // TODO: Manage based on output-enter/output-leave
    return true;
}

void LXQtTaskbarWlrootsBackend::moveApplication(WId)
{
}

void LXQtTaskbarWlrootsBackend::resizeApplication(WId)
{
}

void LXQtTaskbarWlrootsBackend::refreshIconGeometry(WId, const QRect &)
{

}

bool LXQtTaskbarWlrootsBackend::isAreaOverlapped(const QRect &) const
{
    return false;
}

bool LXQtTaskbarWlrootsBackend::isShowingDesktop() const
{
    return m_managment->isShowingDesktop();
}

bool LXQtTaskbarWlrootsBackend::showDesktop(bool)
{
    return false;
}

void LXQtTaskbarWlrootsBackend::addWindow(WId winId)
{
    if (findWindow(windows, winId) != 0 || transients.contains(winId))
    {
        return;
    }

    auto removeWindow = [winId, this]
    {
        eraseWindow(windows, winId);
        lastActivated.remove(winId);

        if (activeWindow == winId)
        {
            activeWindow = 0;
            emit activeWindowChanged(0);
        }

        emit windowRemoved(winId);
    };

    LXQtTaskbarWlrootsWindow *window = getWindow( winId );
    if ( window == nullptr ) {
        return;
    }

    /** The window was closed. Remove from our lists */
    connect(window, &LXQtTaskbarWlrootsWindow::closed, this, removeWindow);

    /**  */
    connect(window, &LXQtTaskbarWlrootsWindow::titleChanged, this, [winId, this] {
        emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::Title));
    });

    connect(window, &LXQtTaskbarWlrootsWindow::appIdChanged, this, [winId, this] {
        emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::WindowClass));
    });

    if (window->windowState.activated) {
        LXQtTaskbarWlrootsWindow *effectiveActive = window;
        while (effectiveActive->parentWindow) {
            effectiveActive = getWindow(effectiveActive->parentWindow);
        }

        lastActivated[effectiveActive->getWindowId()] = QTime::currentTime();
        activeWindow = effectiveActive->getWindowId();
    }

    connect(window, &LXQtTaskbarWlrootsWindow::activatedChanged, this, [window, this] {
        WId effectiveWindow = window->getWindowId();

        while (getWindow(effectiveWindow)->parentWindow)
        {
            effectiveWindow = getWindow(effectiveWindow)->parentWindow;
        }

        if (window->windowState.activated)
        {
            lastActivated[effectiveWindow] = QTime::currentTime();

            if (activeWindow != effectiveWindow)
            {
                activeWindow = effectiveWindow;
                emit activeWindowChanged(activeWindow);
            }
        }
        else
        {
            if (activeWindow == effectiveWindow)
            {
                activeWindow = 0;
                emit activeWindowChanged(0);
            }
        }
    });

    connect(window, &LXQtTaskbarWlrootsWindow::parentChanged, this, [window, this] {
        WId leader = window->parentWindow;

        /** Basically, check if this window is a transient */
        if (transients.remove(leader))
        {
            if (leader)
            {
                // leader change.
                transients.insert(window->getWindowId(), leader);
            }
            else
            {
                // lost a leader, add to regular windows list.
                Q_ASSERT(findWindow(windows, leader) == 0);

                windows.push_back(leader);
            }
        }

        else if (leader)
        {
            eraseWindow(windows, window->getWindowId());
            lastActivated.remove(window->getWindowId());
        }
    });

    auto stateChanged = [window, this] {
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::State));
    };

    connect(window, &LXQtTaskbarWlrootsWindow::fullscreenChanged, this, stateChanged);

    connect(window, &LXQtTaskbarWlrootsWindow::maximizedChanged, this, stateChanged);

    connect(window, &LXQtTaskbarWlrootsWindow::minimizedChanged, this, stateChanged);

    // Handle transient.
    if (WId leader = window->parentWindow)
    {
        transients.insert(winId, leader);
    }
    else
    {
        windows.push_back(winId);
    }

    emit windowAdded( winId );
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::WindowClass));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::Title));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::Icon));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::State));
}

bool LXQtTaskbarWlrootsBackend::acceptWindow(WId window) const
{
    if(transients.contains(window))
        return false;

    return true;
}

LXQtTaskbarWlrootsWindow *LXQtTaskbarWlrootsBackend::getWindow(WId windowId) const
{
    /** Easiest way is to convert the quintptr to the actual pointer */
    LXQtTaskbarWlrootsWindow *win = reinterpret_cast<LXQtTaskbarWlrootsWindow *>( windowId );
    if ( win ) {
        return win;
    }

    return nullptr;
}
