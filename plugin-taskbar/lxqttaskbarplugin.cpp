/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
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


#include "lxqttaskbarplugin.h"

#include "lxqttaskbar.h"

#include "lxqttaskbarconfiguration.h"

LXQtTaskBarPlugin::LXQtTaskBarPlugin(const ILXQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILXQtPanelPlugin(startupInfo)

{
    mTaskBar = new LXQtTaskBar(this);
}


LXQtTaskBarPlugin::~LXQtTaskBarPlugin()
{
    delete mTaskBar;
}

QWidget *LXQtTaskBarPlugin::widget() { return mTaskBar; }

QDialog *LXQtTaskBarPlugin::configureDialog()
{
    return new LXQtTaskbarConfiguration(settings());
}

void LXQtTaskBarPlugin::settingsChanged()
{
    mTaskBar->settingsChanged();
}

void LXQtTaskBarPlugin::realign()
{
    mTaskBar->realign();
}
