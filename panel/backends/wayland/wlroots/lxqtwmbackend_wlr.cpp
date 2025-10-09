#include "lxqttaskbarwlrwm.h"
#include "lxqtwmbackend_wlr.h"
#include "workspace.hpp"

#include <QIcon>
#include <QDateTime>
#include <QScreen>
#include <algorithm>

#include <private/qwaylandscreen_p.h>

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
    m_wsmgr.reset(new LXQt::Taskbar::WorkspaceManagerV1);

    connect(m_managment.get(), &LXQtTaskbarWlrootsWindowManagment::windowCreated, this, &LXQtTaskbarWlrootsBackend::addWindow);

    connect(m_wsmgr.get(), &LXQt::Taskbar::WorkspaceManagerV1::workspaceAdded, this, &LXQtTaskbarWlrootsBackend::workspacesCountChanged);
    connect(m_wsmgr.get(), &LXQt::Taskbar::WorkspaceManagerV1::workspaceRemoved, this, &LXQtTaskbarWlrootsBackend::workspacesCountChanged);
    connect(m_wsmgr.get(), &LXQt::Taskbar::WorkspaceManagerV1::currentWorkspaceChanged, this, [this] ()
    {
        qDebug() << "Current workspace changed" << m_wsmgr->currentWorkspaceIndex();
        emit currentWorkspaceChanged(m_wsmgr->currentWorkspaceIndex(), QString());
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

    case LXQtTaskBarBackendAction::DesktopSwitch:
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
    return m_wsmgr->workspaceCount();
}

QString LXQtTaskbarWlrootsBackend::getWorkspaceName(int, QString) const
{
    return QString();
}

int LXQtTaskbarWlrootsBackend::getCurrentWorkspace() const
{
    return m_wsmgr->currentWorkspaceIndex();
}

bool LXQtTaskbarWlrootsBackend::setCurrentWorkspace(int idx)
{
    m_wsmgr->setCurrentWorkspaceIndex(idx);
    m_wsmgr->commit();

    /** Currently we always assume that this is true */
    return true;
}

int LXQtTaskbarWlrootsBackend::getWindowWorkspace(WId) const
{
    // Until this works, get the current workspace to not affect the taskbar
    return getCurrentWorkspace();
}

bool LXQtTaskbarWlrootsBackend::setWindowOnWorkspace(WId, int)
{
    return true;
}

void LXQtTaskbarWlrootsBackend::moveApplicationToPrevNextMonitor(WId, bool, bool)
{
}

bool LXQtTaskbarWlrootsBackend::isWindowOnScreen(QScreen *screen, WId windowId) const
{
    LXQtTaskbarWlrootsWindow *window = getWindow(windowId);
    if(window)
    {
        QtWaylandClient::QWaylandScreen *waylandScreen = dynamic_cast<QtWaylandClient::QWaylandScreen*>(screen->handle());
        if (waylandScreen)
        {
            wl_output *output = waylandScreen->output();
            return window->windowState.outputs.contains(output);
        }
    }
    return false;
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

bool LXQtTaskbarWlrootsBackend::showDesktop(bool value)
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
        setLastActivated(pId);
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
    connect(window, &LXQtTaskbarWlrootsWindow::outputsChanged, this, &LXQtTaskbarWlrootsBackend::onOutputsChanged);

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
        disconnect(window, &LXQtTaskbarWlrootsWindow::outputsChanged, this, &LXQtTaskbarWlrootsBackend::onOutputsChanged);

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
                    setLastActivated(window->getWindowId());
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
            disconnect(window, &LXQtTaskbarWlrootsWindow::outputsChanged, this, &LXQtTaskbarWlrootsBackend::onOutputsChanged);
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
                setLastActivated(pId);
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

void LXQtTaskbarWlrootsBackend::onOutputsChanged()
{
    if (auto window = qobject_cast<LXQtTaskbarWlrootsWindow *>(QObject::sender()))
        emit windowPropertyChanged(window->getWindowId(), int(LXQtTaskBarWindowProperty::Geometry));
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

void LXQtTaskbarWlrootsBackend::setLastActivated(WId id)
{
    auto t = QDateTime::currentMSecsSinceEpoch();
    while (lastActivated.key(t) != 0)
        ++t; // make sure the times are not equal
    lastActivated[id] = t;
}


int LXQtWMBackendWlrootsLibrary::getBackendScore(const QString& key) const
{
    if (key.compare(QStringLiteral("wlroots"), Qt::CaseInsensitive) == 0)
    {
        return 50;
    }

    static const QStringList supportedList = {QStringLiteral("labwc"),
                                              QStringLiteral("sway"),
                                              QStringLiteral("wayfire"),
                                              // The following compositors are also supported,
                                              // although they are not based on wlroots.
                                              QStringLiteral("hyprland"),
                                              QStringLiteral("river"),
                                              QStringLiteral("niri")};
    if (supportedList.contains(key, Qt::CaseInsensitive))
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
