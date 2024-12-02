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

int LXQtDummyWMBackend::onAllWorkspacesEnum() const
{
    return 0;
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

