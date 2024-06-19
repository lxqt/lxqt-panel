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


#include "lxqttaskbardummybackend.h"

#include <QIcon>

LXQtTaskBarDummyBackend::LXQtTaskBarDummyBackend(QObject *parent)
    : ILXQtTaskbarAbstractBackend(parent)
{

}


/************************************************
 *   Windows function
 ************************************************/
bool LXQtTaskBarDummyBackend::supportsAction(WId, LXQtTaskBarBackendAction) const
{
    return false;
}

bool LXQtTaskBarDummyBackend::reloadWindows()
{
    return false;
}

QVector<WId> LXQtTaskBarDummyBackend::getCurrentWindows() const
{
    return {};
}

QString LXQtTaskBarDummyBackend::getWindowTitle(WId) const
{
    return QString();
}

bool LXQtTaskBarDummyBackend::applicationDemandsAttention(WId) const
{
    return false;
}

QIcon LXQtTaskBarDummyBackend::getApplicationIcon(WId, int) const
{
    return QIcon();
}

QString LXQtTaskBarDummyBackend::getWindowClass(WId) const
{
    return QString();
}

LXQtTaskBarWindowLayer LXQtTaskBarDummyBackend::getWindowLayer(WId) const
{
    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtTaskBarDummyBackend::setWindowLayer(WId, LXQtTaskBarWindowLayer)
{
    return false;
}

LXQtTaskBarWindowState LXQtTaskBarDummyBackend::getWindowState(WId) const
{
    return LXQtTaskBarWindowState::Normal;
}

bool LXQtTaskBarDummyBackend::setWindowState(WId, LXQtTaskBarWindowState, bool)
{
    return false;
}

bool LXQtTaskBarDummyBackend::isWindowActive(WId) const
{
    return false;
}

bool LXQtTaskBarDummyBackend::raiseWindow(WId, bool)
{
    return false;
}

bool LXQtTaskBarDummyBackend::closeWindow(WId)
{
    return false;
}

WId LXQtTaskBarDummyBackend::getActiveWindow() const
{
    return 0;
}


/************************************************
 *   Workspaces
 ************************************************/
int LXQtTaskBarDummyBackend::getWorkspacesCount() const
{
    return 1; // Fake 1 workspace
}

QString LXQtTaskBarDummyBackend::getWorkspaceName(int) const
{
    return QString();
}

int LXQtTaskBarDummyBackend::getCurrentWorkspace() const
{
    return 0;
}

bool LXQtTaskBarDummyBackend::setCurrentWorkspace(int)
{
    return false;
}

int LXQtTaskBarDummyBackend::getWindowWorkspace(WId) const
{
    return 0;
}

bool LXQtTaskBarDummyBackend::setWindowOnWorkspace(WId, int)
{
    return false;
}

void LXQtTaskBarDummyBackend::moveApplicationToPrevNextMonitor(WId, bool, bool)
{
    //No-op
}

bool LXQtTaskBarDummyBackend::isWindowOnScreen(QScreen *, WId) const
{
    return false;
}

bool LXQtTaskBarDummyBackend::setDesktopLayout(Qt::Orientation, int, int, bool)
{
    return false;
}

/************************************************
 *   X11 Specific
 ************************************************/
void LXQtTaskBarDummyBackend::moveApplication(WId)
{
    //No-op
}

void LXQtTaskBarDummyBackend::resizeApplication(WId)
{
    //No-op
}

void LXQtTaskBarDummyBackend::refreshIconGeometry(WId, QRect const &)
{
    //No-op
}

bool LXQtTaskBarDummyBackend::isAreaOverlapped(const QRect &) const
{
    return false;
}

bool LXQtTaskBarDummyBackend::isShowingDesktop() const
{
    return false;
}

bool LXQtTaskBarDummyBackend::showDesktop(bool)
{
    return false;
}

