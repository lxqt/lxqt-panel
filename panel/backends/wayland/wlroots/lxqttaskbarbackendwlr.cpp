#include "lxqttaskbarbackendwlr.h"

#include "lxqttaskbarwlrwindowmanagment.h"
#include "lxqtwlrvirtualdesktop.h"

#include <QIcon>
#include <QTime>
#include <QScreen>
#include <algorithm>

LXQtTaskBarWlrootsWindow* findWindow(const std::vector<LXQtTaskBarWlrootsWindow *> windows, LXQtTaskBarWlrootsWindow* window) {
    // Use std::find to locate the window pointer within the vector
    auto it = std::find(windows.begin(), windows.end(), window);

    // Check if the window was found (iterator != end of vector)
    if (it != windows.end()) {
        // Return the pointer to the found window
        return *it;
    }
    // Window not found, return nullptr
    return nullptr;
}

void eraseWindow(std::vector<LXQtTaskBarWlrootsWindow*>& windows, LXQtTaskBarWlrootsWindow* window) {
    // Use std::remove to find the element to erase
    auto it = std::remove(windows.begin(), windows.end(), window);

    // If the element was found, erase it from the vector
    if (it != windows.end()) {
        windows.erase(it, windows.end());
    }
}

LXQtTaskbarWlrootsBackend::LXQtTaskbarWlrootsBackend(QObject *parent) :
    ILXQtTaskbarAbstractBackend(parent)
{
    m_managment.reset(new LXQtTaskBarWlrootsWindowManagment);
    m_workspaceInfo.reset(new LXQtWlrootsWaylandWorkspaceInfo);

    connect(m_managment.get(), &LXQtTaskBarWlrootsWindowManagment::windowCreated, this, [this](LXQtTaskBarWlrootsWindow *window) {
        addWindow(window);
    });
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
    wids.reserve(wids.size());

    for(LXQtTaskBarWlrootsWindow * window : windows)
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
    return false;
}

LXQtTaskBarWindowState LXQtTaskbarWlrootsBackend::getWindowState(WId windowId) const
{
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
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
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
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
        /** Restore if maximized/minimized */
        if ( window->windowState.minimized) {
            window->unset_minimized();
        }

        if ( window->windowState.maximized) {
            window->unset_maximized();
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
    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return activeWindow == window || window->windowState.activated;
}

bool LXQtTaskbarWlrootsBackend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    Q_UNUSED(onCurrentWorkSpace) //TODO

    LXQtTaskBarWlrootsWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->activate();
    return true;
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

void LXQtTaskbarWlrootsBackend::addWindow(LXQtTaskBarWlrootsWindow *window)
{
    if (findWindow(windows, window) != nullptr || transients.contains(window))
    {
        return;
    }

    auto removeWindow = [window, this]
    {
        eraseWindow(windows, window);
        lastActivated.remove(window);

        if (activeWindow == window)
        {
            activeWindow = nullptr;
            emit activeWindowChanged(0);
        }

        emit windowRemoved(window->getWindowId());
    };

    connect(window, &LXQtTaskBarWlrootsWindow::closed, this, removeWindow);

    connect(window, &LXQtTaskBarWlrootsWindow::titleChanged, this, [window, this] {
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Title));
    });

    connect(window, &LXQtTaskBarWlrootsWindow::appIdChanged, this, [window, this] {
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::WindowClass));
    });

    if (window->windowState.activated) {
        LXQtTaskBarWlrootsWindow *effectiveActive = window;
        while (effectiveActive->parentWindow) {
            effectiveActive = effectiveActive->parentWindow;
        }

        lastActivated[effectiveActive] = QTime::currentTime();
        activeWindow = effectiveActive;
    }

    connect(window, &LXQtTaskBarWlrootsWindow::activatedChanged, this, [window, this] {
        LXQtTaskBarWlrootsWindow *effectiveWindow = window;

        while (effectiveWindow->parentWindow)
        {
            effectiveWindow = effectiveWindow->parentWindow;
        }

        if (window->windowState.activated)
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
                Q_ASSERT(findWindow(windows, window) == nullptr);

                windows.emplace_back(window);
            }
        }
        else if (leader)
        {
            eraseWindow(windows, window);
            lastActivated.remove(window);
        }
    });

    auto stateChanged = [window, this] {
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::State));
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
        windows.push_back(window);
    }

    qDebug() << window << window->getWindowId();
    emit windowAdded( window->getWindowId() );
    emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Title));
    emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Icon));
    emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::State));
    emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Geometry));
    emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::WindowClass));
}

bool LXQtTaskbarWlrootsBackend::acceptWindow(LXQtTaskBarWlrootsWindow *window) const
{
    if(transients.contains(window))
        return false;

    return true;
}

LXQtTaskBarWlrootsWindow *LXQtTaskbarWlrootsBackend::getWindow(WId windowId) const
{
    /** Easiest way is to convert the quintptr to the actual pointer */
    LXQtTaskBarWlrootsWindow *win = reinterpret_cast<LXQtTaskBarWlrootsWindow *>( windowId );
    if ( win ) {
        qDebug() << "get-window-windowId";
        return win;
    }

    /** Fallback attempt */
    for(auto &window : std::as_const(windows))
    {
        if(window->getWindowId() == windowId) {
            return window;
        }
    }

    return nullptr;
}
