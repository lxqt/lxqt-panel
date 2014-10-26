/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
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


#include "lxqtnetworkmonitorplugin.h"
#include "lxqtnetworkmonitor.h"
#include "lxqtnetworkmonitorconfiguration.h"

LxQtNetworkMonitorPlugin::LxQtNetworkMonitorPlugin(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mWidget(new LxQtNetworkMonitor(this))
{
}

LxQtNetworkMonitorPlugin::~LxQtNetworkMonitorPlugin()
{
    delete mWidget;
}

QWidget *LxQtNetworkMonitorPlugin::widget()
{
    return mWidget;
}

QDialog *LxQtNetworkMonitorPlugin::configureDialog()
{
    return new LxQtNetworkMonitorConfiguration(settings(), mWidget);
}

void LxQtNetworkMonitorPlugin::settingsChanged()
{
    mWidget->settingsChanged();
}
