#include "lxqttaskbarniriwm.h"
#include "lxqtwmbackend_niri.h"

#include <QIcon>
#include <QDateTime>
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

LXQtTaskbarNiriBackend::LXQtTaskbarNiriBackend(QObject *parent) :
    ILXQtAbstractWMInterface(parent)
{
    m_managment.reset(new LXQtTaskbarNiriWindowManagment);

    connect(m_managment.get(), &LXQtTaskbarNiriWindowManagment::windowCreated, this, &LXQtTaskbarNiriBackend::addWindow);
}

bool LXQtTaskbarNiriBackend::supportsAction(WId, LXQtTaskBarBackendAction action) const
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

bool LXQtTaskbarNiriBackend::reloadWindows()
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

QVector<WId> LXQtTaskbarNiriBackend::getCurrentWindows() const
{
    QVector<WId> wids;

    for( WId wid: windows ){
        wids << wid;
    }

    return wids;
}

QString LXQtTaskbarNiriBackend::getWindowTitle(WId windowId) const
{
    LXQtTaskbarNiriWindow *window = getWindow(windowId);
    if(!window)
        return QString();

    return window->windowState.title;
}

bool LXQtTaskbarNiriBackend::applicationDemandsAttention(WId) const
{
    return false;
}

QIcon LXQtTaskbarNiriBackend::getApplicationIcon(WId windowId, int devicePixels) const
{
    Q_UNUSED(devicePixels)

    LXQtTaskbarNiriWindow *window = getWindow(windowId);
    if(!window)
        return QIcon();

    return window->icon;
}

QString LXQtTaskbarNiriBackend::getWindowClass(WId windowId) const
{
    LXQtTaskbarNiriWindow *window = getWindow(windowId);
    if(!window)
        return QString();
    return window->windowState.appId;
}

LXQtTaskBarWindowLayer LXQtTaskbarNiriBackend::getWindowLayer(WId) const
{
    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtTaskbarNiriBackend::setWindowLayer(WId, LXQtTaskBarWindowLayer)
{
    return false;
}

LXQtTaskBarWindowState LXQtTaskbarNiriBackend::getWindowState(WId windowId) const
{
    LXQtTaskbarNiriWindow *window = getWindow(windowId);
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

bool LXQtTaskbarNiriBackend::setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set)
{
    LXQtTaskbarNiriWindow *window = getWindow(windowId);
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

bool LXQtTaskbarNiriBackend::isWindowActive(WId windowId) const
{
    LXQtTaskbarNiriWindow *window = getWindow(windowId);
    if(!window)
        return false;

    return activeWindow == windowId || window->windowState.activated;
}

bool LXQtTaskbarNiriBackend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    Q_UNUSED(onCurrentWorkSpace) // Cannot be done on a generic wlroots-based compositor!

    LXQtTaskbarNiriWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->activate();
    return true;
}

bool LXQtTaskbarNiriBackend::closeWindow(WId windowId)
{
    LXQtTaskbarNiriWindow *window = getWindow(windowId);
    if(!window)
        return false;

    window->close();
    return true;
}

WId LXQtTaskbarNiriBackend::getActiveWindow() const
{
    return activeWindow;
}

int LXQtTaskbarNiriBackend::getWorkspacesCount() const
{
    return 1;
}

QString LXQtTaskbarNiriBackend::getWorkspaceName(int) const
{
    return QStringLiteral("Desktop 1");
}

int LXQtTaskbarNiriBackend::getCurrentWorkspace() const
{
    return 1;
}

bool LXQtTaskbarNiriBackend::setCurrentWorkspace(int)
{
    return false;
}

int LXQtTaskbarNiriBackend::getWindowWorkspace(WId) const
{
    return 1;
}

bool LXQtTaskbarNiriBackend::setWindowOnWorkspace(WId, int)
{
    return true;
}

void LXQtTaskbarNiriBackend::moveApplicationToPrevNextMonitor(WId, bool, bool)
{
}

bool LXQtTaskbarNiriBackend::isWindowOnScreen(QScreen *, WId) const
{
    // TODO: Manage based on output-enter/output-leave
    return true;
}

bool LXQtTaskbarNiriBackend::setDesktopLayout(Qt::Orientation, int, int, bool) {
    // Niri has no support for workspace as of 2024-August-20
    return false;
}

void LXQtTaskbarNiriBackend::moveApplication(WId)
{
}

void LXQtTaskbarNiriBackend::resizeApplication(WId)
{
}

void LXQtTaskbarNiriBackend::refreshIconGeometry(WId, const QRect &)
{

}

bool LXQtTaskbarNiriBackend::isAreaOverlapped(const QRect &) const
{
    return false;
}

bool LXQtTaskbarNiriBackend::isShowingDesktop() const
{
    return m_managment->isShowingDesktop();
}

bool LXQtTaskbarNiriBackend::showDesktop(bool value)
{
    if (isShowingDesktop() == value)
        return false;

    // If the windows are going to be restored but all of them are already restored,
    // removed or closed (e.g., by the user), show the desktop instead.
    if (!value)
    {
        bool hasMinimized = false;
        for (auto windowId : std::as_const(showDesktopWins))
        {
            if (getWindowState(windowId) == LXQtTaskBarWindowState::Minimized
                && std::find(windows.begin(), windows.end(), windowId) != windows.end())
            {
                hasMinimized = true;
                break;
            }
        }
        if (!hasMinimized)
            value = true;
    }

    if (value)
    {
        showDesktopWins.clear();
        QVector<WId> wids = getCurrentWindows();
        std::sort(wids.begin(), wids.end(), [this](WId id1, WId id2) {
            // sort the list by activation time to keep the z-order on restoring
            return (lastActivated.value(id1) < lastActivated.value(id2));
        });
        for (auto windowId : std::as_const(wids))
        {
            if (getWindowState(windowId) == LXQtTaskBarWindowState::Minimized)
            { // was minimized before showing the desktop and so, should not be restored later
                continue;
            }
            setWindowState(windowId, LXQtTaskBarWindowState::Minimized, true);
            showDesktopWins.push_back(windowId);
        }
    }
    else
    {
        for (auto windowId : std::as_const(showDesktopWins))
            setWindowState(windowId, LXQtTaskBarWindowState::Minimized, false);
        showDesktopWins.clear();
    }
    m_managment->setShowingDesktop(!showDesktopWins.empty());
    return true;
}

WId LXQtTaskbarNiriBackend::findWindow(WId tgt) const
{
    for (auto id : std::as_const(windows)) {
        if (equalIds(id, tgt)) {
            return id;
        }
    }
    return 0;
}

WId LXQtTaskbarNiriBackend::findTopParent(WId winId) const
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

void LXQtTaskbarNiriBackend::addWindow(WId winId)
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

    LXQtTaskbarNiriWindow *window = getWindow(winId);
    if (window == nullptr) {
        return;
    }

    if (window->windowState.activated) {
        WId pId = findTopParent(winId);
        setLastActivated(pId);
        activeWindow = pId;
        emit activeWindowChanged(activeWindow);
    }

    connect(window, &LXQtTaskbarNiriWindow::activatedChanged, this, &LXQtTaskbarNiriBackend::onActivatedChanged);
    connect(window, &LXQtTaskbarNiriWindow::parentChanged, this, &LXQtTaskbarNiriBackend::onParentChanged);

    // add it either to transients or windows
    if (WId leader = window->parentWindow)
    {
        transients.insert(winId, leader);
        connect(window, &LXQtTaskbarNiriWindow::closed, this, &LXQtTaskbarNiriBackend::removeTransient);
    }
    else
    {
        addToWindows(winId);
    }
}

void LXQtTaskbarNiriBackend::addToWindows(WId winId)
{
    LXQtTaskbarNiriWindow *window = getWindow(winId);
    if (window == nullptr) {
        return;
    }

    windows.push_back(winId);

    connect(window, &LXQtTaskbarNiriWindow::closed, this, &LXQtTaskbarNiriBackend::removeWindow);
    connect(window, &LXQtTaskbarNiriWindow::titleChanged, this, &LXQtTaskbarNiriBackend::onTitleChanged);
    connect(window, &LXQtTaskbarNiriWindow::appIdChanged, this, &LXQtTaskbarNiriBackend::onAppIdChanged);
    connect(window, &LXQtTaskbarNiriWindow::fullscreenChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);
    connect(window, &LXQtTaskbarNiriWindow::maximizedChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);
    connect(window, &LXQtTaskbarNiriWindow::minimizedChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);

    emit windowAdded( winId );
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::WindowClass));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::Title));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::Icon));
    emit windowPropertyChanged(winId, int(LXQtTaskBarWindowProperty::State));
}

void LXQtTaskbarNiriBackend::removeWindow()
{
    if (auto window = qobject_cast<LXQtTaskbarNiriWindow *>(QObject::sender()))
    {
        disconnect(window, &LXQtTaskbarNiriWindow::closed, this, &LXQtTaskbarNiriBackend::removeWindow);
        disconnect(window, &LXQtTaskbarNiriWindow::parentChanged, this, &LXQtTaskbarNiriBackend::onParentChanged);
        disconnect(window, &LXQtTaskbarNiriWindow::activatedChanged, this, &LXQtTaskbarNiriBackend::onActivatedChanged);
        disconnect(window, &LXQtTaskbarNiriWindow::titleChanged, this, &LXQtTaskbarNiriBackend::onTitleChanged);
        disconnect(window, &LXQtTaskbarNiriWindow::appIdChanged, this, &LXQtTaskbarNiriBackend::onAppIdChanged);
        disconnect(window, &LXQtTaskbarNiriWindow::fullscreenChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);
        disconnect(window, &LXQtTaskbarNiriWindow::maximizedChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);
        disconnect(window, &LXQtTaskbarNiriWindow::minimizedChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);

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

void LXQtTaskbarNiriBackend::removeTransient()
{
    if (auto window = qobject_cast<LXQtTaskbarNiriWindow *>(QObject::sender()))
    {
        disconnect(window, &LXQtTaskbarNiriWindow::closed, this, &LXQtTaskbarNiriBackend::removeTransient);
        disconnect(window, &LXQtTaskbarNiriWindow::parentChanged, this, &LXQtTaskbarNiriBackend::onParentChanged);
        disconnect(window, &LXQtTaskbarNiriWindow::activatedChanged, this, &LXQtTaskbarNiriBackend::onActivatedChanged);
        transients.remove(window->getWindowId());
    }
}

void LXQtTaskbarNiriBackend::onActivatedChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarNiriWindow *>(QObject::sender()))
    {
        WId effectiveWindow = findTopParent(window->getWindowId());

        if (window->windowState.activated)
        {
            setLastActivated(effectiveWindow);

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

void LXQtTaskbarNiriBackend::onParentChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarNiriWindow *>(QObject::sender()))
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

                disconnect(window, &LXQtTaskbarNiriWindow::closed, this, &LXQtTaskbarNiriBackend::removeTransient);
                addToWindows(window->getWindowId());

                // Correct the activation state if an active window that has lost its leader was active before,
                // because "LXQtTaskbarNiriWindow::activatedChanged" might not be emitted for it.
                if (window->windowState.activated)
                {
                    setLastActivated(window->getWindowId());
                    activeWindow = window->getWindowId();
                    emit activeWindowChanged(activeWindow);
                }
            }
        }
        else if (leader)
        {
            // remove it from regular windows list
            disconnect(window, &LXQtTaskbarNiriWindow::closed, this, &LXQtTaskbarNiriBackend::removeWindow);
            disconnect(window, &LXQtTaskbarNiriWindow::titleChanged, this, &LXQtTaskbarNiriBackend::onTitleChanged);
            disconnect(window, &LXQtTaskbarNiriWindow::appIdChanged, this, &LXQtTaskbarNiriBackend::onAppIdChanged);
            disconnect(window, &LXQtTaskbarNiriWindow::fullscreenChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);
            disconnect(window, &LXQtTaskbarNiriWindow::maximizedChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);
            disconnect(window, &LXQtTaskbarNiriWindow::minimizedChanged, this, &LXQtTaskbarNiriBackend::onStateChanged);
            eraseWindow(windows, window->getWindowId());
            lastActivated.remove(window->getWindowId());
            // announce that it's removed
            emit windowRemoved(window->getWindowId());

            // add it to transients
            transients.insert(window->getWindowId(), leader);
            connect(window, &LXQtTaskbarNiriWindow::closed, this, &LXQtTaskbarNiriBackend::removeTransient);

            // Correct the activation state if a window that has got a leader was active before.
            if (activeWindow == window->getWindowId())
            {
                WId pId = findTopParent(window->getWindowId());
                setLastActivated(pId);
                activeWindow = pId;
                emit activeWindowChanged(activeWindow);
            }
        }
    }
}

void LXQtTaskbarNiriBackend::onTitleChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarNiriWindow *>(QObject::sender()))
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Title));
}

void LXQtTaskbarNiriBackend::onAppIdChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarNiriWindow *>(QObject::sender()))
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::WindowClass));
}

void LXQtTaskbarNiriBackend::onStateChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarNiriWindow *>(QObject::sender()))
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::State));
}

bool LXQtTaskbarNiriBackend::acceptWindow(WId window) const
{
    if(transients.contains(window))
        return false;

    return true;
}

LXQtTaskbarNiriWindow *LXQtTaskbarNiriBackend::getWindow(WId windowId) const
{
    /** Easiest way is to convert the quintptr to the actual pointer */
    LXQtTaskbarNiriWindow *win = reinterpret_cast<LXQtTaskbarNiriWindow *>( windowId );
    if ( win ) {
        return win;
    }

    return nullptr;
}

bool LXQtTaskbarNiriBackend::equalIds(WId windowId1, WId windowId2) const
{
    if (windowId1 == windowId2) {
        return true;
    }
    LXQtTaskbarNiriWindow *win1 = reinterpret_cast<LXQtTaskbarNiriWindow *>( windowId1 );
    LXQtTaskbarNiriWindow *win2 = reinterpret_cast<LXQtTaskbarNiriWindow *>( windowId2 );
    if (win1 == nullptr && win2 == nullptr) {
        return true;
    }
    if (win1 != nullptr && win2 != nullptr) {
        return win1->ID == win2->ID;
    }
    return false;
}

void LXQtTaskbarNiriBackend::setLastActivated(WId id)
{
    auto t = QDateTime::currentMSecsSinceEpoch();
    while (lastActivated.key(t) != 0)
        ++t; // make sure the times are not equal
    lastActivated[id] = t;
}


int LXQtWMBackendNiriLibrary::getBackendScore(const QString& key) const
{
    if (key == QStringLiteral("smithay"))
    {
        return 50;
    }

    if (key == QStringLiteral("niri"))
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


ILXQtAbstractWMInterface *LXQtWMBackendNiriLibrary::instance() const
{
    return new LXQtTaskbarNiriBackend(nullptr);
}
