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

#include "ejectaction.h"
#include "ejectaction_nothing.h"
#include "ejectaction_optical.h"
#include "../lxqtmountplugin.h"

#define ACT_NOTHING       "nothing"
#define ACT_EJECT_OPTICAL "ejectOpticalDrives"

#define ACT_NOTHING_UPPER       QStringLiteral(ACT_NOTHING).toUpper()
#define ACT_EJECT_OPTICAL_UPPER QStringLiteral(ACT_EJECT_OPTICAL).toUpper()

EjectAction::EjectAction(LXQtMountPlugin *plugin, QObject *parent)
    : QObject(parent)
    , mPlugin(plugin)
{
}

EjectAction::~EjectAction()
{
}

EjectAction *EjectAction::create(ActionId id, LXQtMountPlugin *plugin, QObject *parent)
{
    switch (id)
    {
    case ActionNothing:
        return new EjectActionNothing(plugin, parent);

    case ActionOptical:
        return new EjectActionOptical(plugin, parent);
    }

    return nullptr;
}

QString EjectAction::actionIdToString(EjectAction::ActionId id)
{
    switch (id)
    {
    case ActionNothing:    return QStringLiteral(ACT_NOTHING);
    case ActionOptical:    return QStringLiteral(ACT_EJECT_OPTICAL);
    }

    return QStringLiteral(ACT_NOTHING);
}

void EjectAction::onEjectPressed(void)
{
    doEjectPressed();
}

EjectAction::ActionId EjectAction::stringToActionId(const QString &string, ActionId defaultValue)
{
    QString s = string.toUpper();
    if (s == ACT_NOTHING_UPPER)          return ActionNothing;
    if (s == ACT_EJECT_OPTICAL_UPPER)    return ActionOptical;

    return defaultValue;
}
