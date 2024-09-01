#include "lxqttaskbarwlrwm.h"
#include "lxqtwmbackend_wlr.h"

#include <QIcon>
#include <QTime>
#include <QScreen>
#include <algorithm>

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
    ILXQtAbstractWMInterface(parent)
{
    m_managment.reset(new LXQtTaskbarWlrootsWindowManagment);

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
    return 1;
}

QString LXQtTaskbarWlrootsBackend::getWorkspaceName(int) const
{
    return QStringLiteral("Desktop 1");
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
    return true;
}

void LXQtTaskbarWlrootsBackend::moveApplicationToPrevNextMonitor(WId, bool, bool)
{
}

bool LXQtTaskbarWlrootsBackend::isWindowOnScreen(QScreen *, WId) const
{
    // TODO: Manage based on output-enter/output-leave
    return true;
}

bool LXQtTaskbarWlrootsBackend::setDesktopLayout(Qt::Orientation, int, int, bool) {
    // Wlroots has no support for workspace as of 2024-August-20
    return false;
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

WId LXQtTaskbarWlrootsBackend::findWindow(WId tgt) const
{
    for (auto id : std::as_const(windows)) {
        if (equalIds(id, tgt)) {
            return id;
        }
    }
    return 0;
}

WId LXQtTaskbarWlrootsBackend::findTopParent(WId winId) const
{
    while (getWindow(winId)->parentWindow)
    {
        winId = getWindow(winId)->parentWindow;
        // first search in transients because this may be a child window of another one
        WId window = 0;
        for (auto i = transients.cbegin(), end = transients.cend(); i != end; ++i)
        {
            if (equalIds(i.key(), winId))
            {
                window = i.key();
                break;
            }
        }
        if (window)
            winId = window;
        else
        {
            window = findWindow(winId);
            if (window)
                winId = window;
        }
    }
    return winId;
}

void LXQtTaskbarWlrootsBackend::addWindow(WId winId)
{
    for (auto id : std::as_const(windows))
    {
        if (id == winId)
        {
            return;
        }
    }

    if (transients.contains(winId))
    {
        return;
    }

    LXQtTaskbarWlrootsWindow *window = getWindow(winId);
    if (window == nullptr) {
        return;
    }

    if (window->windowState.activated) {
        WId pId = findTopParent(winId);
        lastActivated[pId] = QTime::currentTime();
        activeWindow = pId;
        emit activeWindowChanged(activeWindow);
    }

    connect(window, &LXQtTaskbarWlrootsWindow::activatedChanged, this, &LXQtTaskbarWlrootsBackend::onActivatedChanged);
    connect(window, &LXQtTaskbarWlrootsWindow::parentChanged, this, &LXQtTaskbarWlrootsBackend::onParentChanged);

    // add it either to transients or windows
    if (WId leader = window->parentWindow)
    {
        transients.insert(winId, leader);
        connect(window, &LXQtTaskbarWlrootsWindow::closed, this, &LXQtTaskbarWlrootsBackend::removeTransient);
    }
    else
    {
        addToWindows(winId);
    }
}

void LXQtTaskbarWlrootsBackend::addToWindows(WId winId)
{
    LXQtTaskbarWlrootsWindow *window = getWindow(winId);
    if (window == nullptr) {
        return;
    }

    windows.push_back(winId);

    connect(window, &LXQtTaskbarWlrootsWindow::closed, this, &LXQtTaskbarWlrootsBackend::removeWindow);
    connect(window, &LXQtTaskbarWlrootsWindow::titleChanged, this, &LXQtTaskbarWlrootsBackend::onTitleChanged);
    connect(window, &LXQtTaskbarWlrootsWindow::appIdChanged, this, &LXQtTaskbarWlrootsBackend::onAppIdChanged);
    connect(window, &LXQtTaskbarWlrootsWindow::fullscreenChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);
    connect(window, &LXQtTaskbarWlrootsWindow::maximizedChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);
    connect(window, &LXQtTaskbarWlrootsWindow::minimizedChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);

    emit windowAdded( winId );
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::WindowClass));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::Title));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::Icon));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::State));
}

void LXQtTaskbarWlrootsBackend::removeWindow()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
    {
        disconnect(window, &LXQtTaskbarWlrootsWindow::closed, this, &LXQtTaskbarWlrootsBackend::removeWindow);
        disconnect(window, &LXQtTaskbarWlrootsWindow::parentChanged, this, &LXQtTaskbarWlrootsBackend::onParentChanged);
        disconnect(window, &LXQtTaskbarWlrootsWindow::activatedChanged, this, &LXQtTaskbarWlrootsBackend::onActivatedChanged);
        disconnect(window, &LXQtTaskbarWlrootsWindow::titleChanged, this, &LXQtTaskbarWlrootsBackend::onTitleChanged);
        disconnect(window, &LXQtTaskbarWlrootsWindow::appIdChanged, this, &LXQtTaskbarWlrootsBackend::onAppIdChanged);
        disconnect(window, &LXQtTaskbarWlrootsWindow::fullscreenChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);
        disconnect(window, &LXQtTaskbarWlrootsWindow::maximizedChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);
        disconnect(window, &LXQtTaskbarWlrootsWindow::minimizedChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);

        WId winId = window->getWindowId();
        eraseWindow(windows, winId);
        lastActivated.remove(winId);

        if (activeWindow == winId)
        {
            activeWindow = 0;
            emit activeWindowChanged(0);
        }

        emit windowRemoved(winId);
    }
}

void LXQtTaskbarWlrootsBackend::removeTransient()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
    {
        disconnect(window, &LXQtTaskbarWlrootsWindow::closed, this, &LXQtTaskbarWlrootsBackend::removeTransient);
        disconnect(window, &LXQtTaskbarWlrootsWindow::parentChanged, this, &LXQtTaskbarWlrootsBackend::onParentChanged);
        disconnect(window, &LXQtTaskbarWlrootsWindow::activatedChanged, this, &LXQtTaskbarWlrootsBackend::onActivatedChanged);
        transients.remove(window->getWindowId());
    }
}

void LXQtTaskbarWlrootsBackend::onActivatedChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
    {
        WId effectiveWindow = findTopParent(window->getWindowId());

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
            // First check if it has an active child (transient) window.
            for (auto i = transients.cbegin(), end = transients.cend(); i != end; ++i)
            {
                if (window->getWindowId() != i.key() && equalIds(findTopParent(i.key()), effectiveWindow))
                {
                    if (auto win = getWindow(i.key()))
                    {
                        if (win->windowState.activated)
                            return;
                    }
                }
            }

            if (activeWindow == effectiveWindow)
            {
                activeWindow = 0;
                emit activeWindowChanged(0);
            }
        }
    }
}

void LXQtTaskbarWlrootsBackend::onParentChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
    {
       WId leader = window->parentWindow;

        /** Basically, check if this window is a transient */
        if (transients.remove(window->getWindowId()))
        {
            if (leader)
            {
                // leader change.
                transients.insert(window->getWindowId(), leader);
            }
            else
            {
                // lost a leader, add to regular windows list.
                Q_ASSERT(findWindow(leader) == 0);

                disconnect(window, &LXQtTaskbarWlrootsWindow::closed, this, &LXQtTaskbarWlrootsBackend::removeTransient);
                addToWindows(window->getWindowId());

                // Correct the activation state if an active window that has lost its leader was active before,
                // because "LXQtTaskbarWlrootsWindow::activatedChanged" might not be emitted for it.
                if (window->windowState.activated)
                {
                    lastActivated[window->getWindowId()] = QTime::currentTime();
                    activeWindow = window->getWindowId();
                    emit activeWindowChanged(activeWindow);
                }
            }
        }
        else if (leader)
        {
            // remove it from regular windows list
            disconnect(window, &LXQtTaskbarWlrootsWindow::closed, this, &LXQtTaskbarWlrootsBackend::removeWindow);
            disconnect(window, &LXQtTaskbarWlrootsWindow::titleChanged, this, &LXQtTaskbarWlrootsBackend::onTitleChanged);
            disconnect(window, &LXQtTaskbarWlrootsWindow::appIdChanged, this, &LXQtTaskbarWlrootsBackend::onAppIdChanged);
            disconnect(window, &LXQtTaskbarWlrootsWindow::fullscreenChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);
            disconnect(window, &LXQtTaskbarWlrootsWindow::maximizedChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);
            disconnect(window, &LXQtTaskbarWlrootsWindow::minimizedChanged, this, &LXQtTaskbarWlrootsBackend::onStateChanged);
            eraseWindow(windows, window->getWindowId());
            lastActivated.remove(window->getWindowId());
            // announce that it's removed
            emit windowRemoved(window->getWindowId());

            // add it to transients
            transients.insert(window->getWindowId(), leader);
            connect(window, &LXQtTaskbarWlrootsWindow::closed, this, &LXQtTaskbarWlrootsBackend::removeTransient);

            // Correct the activation state if a window that has got a leader was active before.
            if (activeWindow == window->getWindowId())
            {
                WId pId = findTopParent(window->getWindowId());

                lastActivated[pId] = QTime::currentTime();
                activeWindow = pId;
                emit activeWindowChanged(activeWindow);
            }
        }
    }
}

void LXQtTaskbarWlrootsBackend::onTitleChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Title));
}

void LXQtTaskbarWlrootsBackend::onAppIdChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::WindowClass));
}

void LXQtTaskbarWlrootsBackend::onStateChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::State));
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

bool LXQtTaskbarWlrootsBackend::equalIds(WId windowId1, WId windowId2) const
{
    if (windowId1 == windowId2) {
        return true;
    }
    LXQtTaskbarWlrootsWindow *win1 = reinterpret_cast<LXQtTaskbarWlrootsWindow *>( windowId1 );
    LXQtTaskbarWlrootsWindow *win2 = reinterpret_cast<LXQtTaskbarWlrootsWindow *>( windowId2 );
    if (win1 == nullptr && win2 == nullptr) {
        return true;
    }
    if (win1 != nullptr && win2 != nullptr) {
        return win1->ID == win2->ID;
    }
    return false;
}


int LXQtWMBackendWlrootsLibrary::getBackendScore(const QString& key) const
{
	if (key == QStringLiteral("wlroots"))
	{
		return 50;
	}

	else if (key == QStringLiteral("wayfire"))
	{
		return 30;
	}

	else if (key == QStringLiteral("sway"))
	{
		return 30;
	}

	else if (key == QStringLiteral("hyprland"))
	{
		return 30;
	}

	else if (key == QStringLiteral("labwc"))
	{
		return 30;
	}

	else if (key == QStringLiteral("river"))
	{
		return 30;
	}

	// Unsupported
	return 0;
}


ILXQtAbstractWMInterface *LXQtWMBackendWlrootsLibrary::instance() const
{
    return new LXQtTaskbarWlrootsBackend(nullptr);
}
