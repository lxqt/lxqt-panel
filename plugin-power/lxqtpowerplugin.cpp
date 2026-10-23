/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2026 LXQt team
 * Authors:
 *   envy-vain <340632583+envy-vain@users.noreply.github.com>
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

#include "lxqtpowerplugin.h"
#include "lxqtpower.h"
#include "lxqtpowerconfiguration.h"

LXQtPowerPlugin::LXQtPowerPlugin(const ILXQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    mWidget(new LXQtPower(this))
{
}

LXQtPowerPlugin::~LXQtPowerPlugin()
{
    delete mWidget;
}

QWidget *LXQtPowerPlugin::widget()
{
    return mWidget;
}

QDialog *LXQtPowerPlugin::configureDialog()
{
    return new LXQtPowerConfiguration(settings());
}

void LXQtPowerPlugin::settingsChanged()
{
    mWidget->settingsChanged();
}
