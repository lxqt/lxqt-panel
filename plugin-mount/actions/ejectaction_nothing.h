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


#ifndef LXQT_PLUGIN_MOUNT_EJECTACTION_NOTHING_H
#define LXQT_PLUGIN_MOUNT_EJECTACTION_NOTHING_H

#include "ejectaction.h"
#include <QWidget>

class EjectActionNothing : public EjectAction
{
    Q_OBJECT

public:
    explicit EjectActionNothing(LXQtMountPlugin *plugin, QObject *parent = nullptr);
    virtual ActionId Type() const throw () { return ActionNothing; };

protected:
    void doEjectPressed(void);
};

#endif // EJECTACTION_NOTHING_H
