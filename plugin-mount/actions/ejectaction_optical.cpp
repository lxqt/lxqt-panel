/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2020 LXQt team
 * Authors:
 *   Oleksandr Ostrenko <oleksandr.ostrenko@gmail.com>
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

#include "../lxqtmountplugin.h"
#include "ejectaction_optical.h"

#include <Solid/StorageAccess>
#include <Solid/StorageDrive>
#include <Solid/DeviceNotifier>
#include <Solid/OpticalDrive>

#include <LXQt/Notification>
//#include <QDebug>

EjectActionOptical::EjectActionOptical(LXQtMountPlugin *plugin, QObject *parent):
    EjectAction(plugin, parent)
{
}

void EjectActionOptical::doEjectPressed(void)
{
    for (const Solid::Device& device : Solid::Device::listFromType(Solid::DeviceInterface::OpticalDrive))
    {
        Solid::Device it;
        if (device.isValid())
        {
            it = device;
            //qDebug() << "device : " << it.udi() << "\n";
            // search for parent drive
            for (; !it.udi().isEmpty(); it = it.parent())
                if (it.is<Solid::OpticalDrive>())
                    it.as<Solid::OpticalDrive>()->eject();
        }
    }
    LXQt::Notification::notify(tr("Removable media/devices manager"), tr("Ejected all optical drives"), mPlugin->icon().name());
}
