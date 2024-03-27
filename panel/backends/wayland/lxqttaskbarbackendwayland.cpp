#include "lxqttaskbarbackendwayland.h"
#include "plasma/lxqttaskbarbackendplasma.h"
#include "wlroots/lxqttaskbarbackendwlr.h"

#include <QIcon>

LXQtTaskbarWaylandBackend::LXQtTaskbarWaylandBackend(QObject *parent)
    : ILXQtTaskbarAbstractBackend(parent)
{
    /**
     * HACK: For now, we will simply concentrate on plasma and wlroots.
     * Eventually, we should be dynamically loading different plugins.
     */

    /**
     * Let's get our DE:
     * If plasma/kde is in desktopsList, we'll use the plasma backend.
     * If wayfire/sway/labwc/hyprland/wlroots is in desktopsList, we'll use the wlroots backend.
     */
    QList<QByteArray> desktopsList = qgetenv("XDG_CURRENT_DESKTOP").toLower().split(':');
    qDebug() << "--------------> Current desktop" << desktopsList;
    for( QByteArray desktop: desktopsList ) {
        if ( desktop == "plasma" || desktop == "kde" || desktop == "kwin_wayland" )
        {
            qDebug() << "--------------> Using plasma backend";
            m_backend = new LXQtTaskbarPlasmaBackend();
            break;
        }

        else if ( desktop == "wayfire" )
        {
            qDebug() << "--------------> Using wayfire backend";
            m_backend = new LXQtTaskbarWlrootsBackend();
            break;
        }

        else if ( desktop == "sway" )
        {
            qDebug() << "--------------> Using sway backend";
            m_backend = new LXQtTaskbarWlrootsBackend();
            break;
        }

        else if ( desktop == "labwc" )
        {
            qDebug() << "--------------> Using labwc backend";
            m_backend = new LXQtTaskbarWlrootsBackend();
            break;
        }

        else if ( desktop == "hyprland" )
        {
            qDebug() << "--------------> Using hyprland backend";
            m_backend = new LXQtTaskbarWlrootsBackend();
            break;
        }

        else
        {
            // m_backend = nullptr;
        }
    }

    if ( m_backend == nullptr ) {
        qDebug() << "-------------->  Using dummy backend. No window management will be done";
    }
}


/************************************************
 *   Windows function
 ************************************************/
bool LXQtTaskbarWaylandBackend::supportsAction(WId id, LXQtTaskBarBackendAction act) const
{
    return ( m_backend == nullptr ? false : m_backend->supportsAction(id, act) );
}

bool LXQtTaskbarWaylandBackend::reloadWindows()
{
    return ( m_backend == nullptr ? false : m_backend->reloadWindows() );
}

QVector<WId> LXQtTaskbarWaylandBackend::getCurrentWindows() const
{
    return ( m_backend == nullptr ? QVector<WId>() : m_backend->getCurrentWindows() );
}

QString LXQtTaskbarWaylandBackend::getWindowTitle(WId id) const
{
    return ( m_backend == nullptr ? QString() : m_backend->getWindowTitle(id) );
}

bool LXQtTaskbarWaylandBackend::applicationDemandsAttention(WId id) const
{
    return ( m_backend == nullptr ? false : m_backend->applicationDemandsAttention(id) );
}

QIcon LXQtTaskbarWaylandBackend::getApplicationIcon(WId id, int fallbackDevicePixels) const
{
    return ( m_backend == nullptr ? QIcon() : m_backend->getApplicationIcon(id, fallbackDevicePixels) );
}

QString LXQtTaskbarWaylandBackend::getWindowClass(WId id) const
{
    return ( m_backend == nullptr ? QString() : m_backend->getWindowClass(id) );
}

LXQtTaskBarWindowLayer LXQtTaskbarWaylandBackend::getWindowLayer(WId id) const
{
    return ( m_backend == nullptr ? LXQtTaskBarWindowLayer::Normal : m_backend->getWindowLayer(id) );
}

bool LXQtTaskbarWaylandBackend::setWindowLayer(WId id, LXQtTaskBarWindowLayer lyr)
{
    return ( m_backend == nullptr ? false : m_backend->setWindowLayer(id, lyr) );
}

LXQtTaskBarWindowState LXQtTaskbarWaylandBackend::getWindowState(WId id) const
{
    return ( m_backend == nullptr ? LXQtTaskBarWindowState::Normal : m_backend->getWindowState(id) );
}

bool LXQtTaskbarWaylandBackend::setWindowState(WId id, LXQtTaskBarWindowState state, bool set)
{
    return ( m_backend == nullptr ? false : m_backend->setWindowState(id, state, set) );
}

bool LXQtTaskbarWaylandBackend::isWindowActive(WId id) const
{
    return ( m_backend == nullptr ? false : m_backend->isWindowActive(id) );
}

bool LXQtTaskbarWaylandBackend::raiseWindow(WId id, bool yes)
{
    return ( m_backend == nullptr ? false : m_backend->raiseWindow(id, yes) );
}

bool LXQtTaskbarWaylandBackend::closeWindow(WId id)
{
    return ( m_backend == nullptr ? false : m_backend->closeWindow(id) );
}

WId LXQtTaskbarWaylandBackend::getActiveWindow() const
{
    return ( m_backend == nullptr ? 0 : m_backend->getActiveWindow() );
}


/************************************************
 *   Workspaces
 ************************************************/
int LXQtTaskbarWaylandBackend::getWorkspacesCount() const
{
    return ( m_backend == nullptr ? 1 : m_backend->getWorkspacesCount() ); // Fake 1 workspace
}

QString LXQtTaskbarWaylandBackend::getWorkspaceName(int ws) const
{
    return ( m_backend == nullptr ? QString() : m_backend->getWorkspaceName( ws ) );
}

int LXQtTaskbarWaylandBackend::getCurrentWorkspace() const
{
    return ( m_backend == nullptr ? 0 : m_backend->getCurrentWorkspace() );
}

bool LXQtTaskbarWaylandBackend::setCurrentWorkspace(int ws)
{
    return ( m_backend == nullptr ? false : m_backend->setCurrentWorkspace(ws) );
}

int LXQtTaskbarWaylandBackend::getWindowWorkspace(WId id) const
{
    return ( m_backend == nullptr ? 0 : m_backend->getWindowWorkspace( id ) );
}

bool LXQtTaskbarWaylandBackend::setWindowOnWorkspace(WId id, int ws)
{
    return ( m_backend == nullptr ? false : m_backend->setWindowOnWorkspace(id, ws) );
}

void LXQtTaskbarWaylandBackend::moveApplicationToPrevNextMonitor(WId id, bool next, bool raise)
{
    ( m_backend == nullptr ? void() : m_backend->moveApplicationToPrevNextMonitor( id, next, raise) );
}

bool LXQtTaskbarWaylandBackend::isWindowOnScreen(QScreen *scrn, WId id) const
{
    return ( m_backend == nullptr ? false : m_backend->isWindowOnScreen(scrn, id) );
}

/************************************************
 *   X11 Specific
 ************************************************/
void LXQtTaskbarWaylandBackend::moveApplication(WId id)
{
    ( m_backend == nullptr ? void() : m_backend->moveApplication(id) );
}

void LXQtTaskbarWaylandBackend::resizeApplication(WId id)
{
    ( m_backend == nullptr ? void() : m_backend->resizeApplication(id) );
}

void LXQtTaskbarWaylandBackend::refreshIconGeometry(WId id, QRect const &rect)
{
    ( m_backend == nullptr ? void() : m_backend->refreshIconGeometry(id, rect) );
}

bool LXQtTaskbarWaylandBackend::isAreaOverlapped(const QRect &rect) const
{
    return ( m_backend == nullptr ? false : m_backend->isAreaOverlapped(rect) );
}

bool LXQtTaskbarWaylandBackend::isShowingDesktop() const
{
    return ( m_backend == nullptr ? false : m_backend->isShowingDesktop() );
}

bool LXQtTaskbarWaylandBackend::showDesktop(bool yes)
{
    return ( m_backend == nullptr ? false : m_backend->showDesktop(yes) );
}
