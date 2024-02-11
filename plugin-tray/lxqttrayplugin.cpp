/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2.1+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2013 Razor team
 *            2022 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "lxqttrayplugin.h"
#include "fdoselectionmanager.h"

#include <QGuiApplication> // For nativeInterface()

LXQtTrayPlugin::LXQtTrayPlugin(const ILXQtPanelPluginStartupInfo &startupInfo)
    : QObject()
    , ILXQtPanelPlugin(startupInfo)
    , mManager{new FdoSelectionManager}
{
}

LXQtTrayPlugin::~LXQtTrayPlugin()
{
}

QWidget *LXQtTrayPlugin::widget()
{
    return nullptr;
}

ILXQtPanelPlugin *LXQtTrayPluginLibrary::instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
{
    auto *x11Application = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if(!x11Application || !x11Application->connection())
    {
        // Currently only X11 supported
        qWarning() << "Currently tray plugin supports X11 only. Skipping.";
        return nullptr;
    }

    return new LXQtTrayPlugin(startupInfo);
}
