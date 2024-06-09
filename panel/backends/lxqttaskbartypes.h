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


#ifndef LXQTTASKBARTYPES_H
#define LXQTTASKBARTYPES_H

#include <QtGlobal>

typedef quintptr WId;

enum class LXQtTaskBarBackendAction
{
    Move = 0,
    Resize,
    Maximize,
    MaximizeVertically,
    MaximizeHorizontally,
    Minimize,
    RollUp,
    FullScreen,
    DesktopSwitch
};

enum class LXQtTaskBarWindowProperty
{
    Title = 0,
    Icon,
    State,
    Geometry,
    Urgency,
    WindowClass,
    Workspace
};

enum class LXQtTaskBarWindowState
{
    Hidden = 0,
    FullScreen,
    Minimized,
    Maximized,
    MaximizedVertically,
    MaximizedHorizontally,
    Normal,
    RolledUp //Shaded
};

enum class LXQtTaskBarWindowLayer
{
    KeepBelow = 0,
    Normal,
    KeepAbove
};

enum class LXQtTaskBarWorkspace
{
    ShowOnAll = 0 // Virtual destops have 1-based indexes
};

#endif // LXQTTASKBARTYPES_H
