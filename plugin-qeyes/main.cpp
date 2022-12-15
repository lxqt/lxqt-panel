/*
 *  qeyes - an xeyes clone
 *
 *  Copyright (C) 2022 Goffredo Baroncelli <kreijack@inwind.it>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <stdio.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include "main.h"

ILXQtPanelPlugin *QEyesPluginLibrary::instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
{
    return new QEyesPlugin(startupInfo);
}

QEyesPlugin::QEyesPlugin(const ILXQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILXQtPanelPlugin(startupInfo)
{
    w = new QEyesVectorWidget();
    w->setTransparent(true);
    realign();
}

void QEyesPlugin::realign() {
    const auto g = panel()->globalGeometry();
    if (panel()->isHorizontal()) {
        w->setMinimumHeight(g.height() / 2);
        w->setMinimumWidth(0.8 * g.height() * w->getNumEyes() );
    } else {
        w->setMinimumWidth(g.width() / 2);
        w->setMinimumHeight(g.width());
    }
}
