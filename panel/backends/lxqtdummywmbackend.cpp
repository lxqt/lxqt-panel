#include "lxqtdummywmbackend.h"

#include <QIcon>

LXQtDummyWMBackend::LXQtDummyWMBackend(QObject *parent)
    : ILXQtAbstractWMInterface(parent)
{

}

/************************************************
 *   Windows function
 ************************************************/
bool LXQtDummyWMBackend::supportsAction(WId, LXQtTaskBarBackendAction) const
{
    return false;
}

bool LXQtDummyWMBackend::reloadWindows()
{
    return false;
}

QVector<WId> LXQtDummyWMBackend::getCurrentWindows() const
{
    return {};
}

QString LXQtDummyWMBackend::getWindowTitle(WId) const
{
    return QString();
}

bool LXQtDummyWMBackend::applicationDemandsAttention(WId) const
{
    return false;
}

QIcon LXQtDummyWMBackend::getApplicationIcon(WId, int) const
{
    return QIcon();
}

QString LXQtDummyWMBackend::getWindowClass(WId) const
{
    return QString();
}

LXQtTaskBarWindowLayer LXQtDummyWMBackend::getWindowLayer(WId) const
{
    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtDummyWMBackend::setWindowLayer(WId, LXQtTaskBarWindowLayer)
{
    return false;
}

LXQtTaskBarWindowState LXQtDummyWMBackend::getWindowState(WId) const
{
    return LXQtTaskBarWindowState::Normal;
}

bool LXQtDummyWMBackend::setWindowState(WId, LXQtTaskBarWindowState, bool)
{
    return false;
}

bool LXQtDummyWMBackend::isWindowActive(WId) const
{
    return false;
}

bool LXQtDummyWMBackend::raiseWindow(WId, bool)
{
    return false;
}

bool LXQtDummyWMBackend::closeWindow(WId)
{
    return false;
}

WId LXQtDummyWMBackend::getActiveWindow() const
{
    return 0;
}


/************************************************
 *   Workspaces
 ************************************************/
int LXQtDummyWMBackend::getWorkspacesCount() const
{
    return 1; // Fake 1 workspace
}

QString LXQtDummyWMBackend::getWorkspaceName(int) const
{
    return QString();
}

int LXQtDummyWMBackend::getCurrentWorkspace() const
{
    return 0;
}

bool LXQtDummyWMBackend::setCurrentWorkspace(int)
{
    return false;
}

int LXQtDummyWMBackend::getWindowWorkspace(WId) const
{
    return 0;
}

bool LXQtDummyWMBackend::setWindowOnWorkspace(WId, int)
{
    return false;
}

void LXQtDummyWMBackend::moveApplicationToPrevNextMonitor(WId, bool, bool)
{
    //No-op
}

bool LXQtDummyWMBackend::isWindowOnScreen(QScreen *, WId) const
{
    return false;
}

bool LXQtDummyWMBackend::setDesktopLayout(Qt::Orientation, int, int, bool)
{
    return false;
}

/************************************************
 *   X11 Specific
 ************************************************/
void LXQtDummyWMBackend::moveApplication(WId)
{
    //No-op
}

void LXQtDummyWMBackend::resizeApplication(WId)
{
    //No-op
}

void LXQtDummyWMBackend::refreshIconGeometry(WId, QRect const &)
{
    //No-op
}

bool LXQtDummyWMBackend::isAreaOverlapped(const QRect &) const
{
    return false;
}

bool LXQtDummyWMBackend::isShowingDesktop() const
{
    return false;
}

bool LXQtDummyWMBackend::showDesktop(bool)
{
    return false;
}

