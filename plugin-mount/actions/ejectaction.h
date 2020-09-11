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

#ifndef LXQT_PLUGIN_MOUNT_EJECTACTION_H
#define LXQT_PLUGIN_MOUNT_EJECTACTION_H

#include <QObject>
#include <QSettings>
#include <Solid/Device>

class LXQtMountPlugin;

class EjectAction: public QObject
{
    Q_OBJECT

public:
    enum ActionId
    {
        ActionNothing,
        ActionOptical
    };

    virtual ~EjectAction();
    virtual ActionId Type() const throw () = 0;

    static EjectAction *create(ActionId id, LXQtMountPlugin *plugin, QObject *parent = nullptr);
    static ActionId stringToActionId(const QString &string, ActionId defaultValue);
    static QString actionIdToString(ActionId id);

public slots:
    void onEjectPressed(void);

protected:
    explicit EjectAction(LXQtMountPlugin *plugin, QObject *parent = nullptr);
    virtual void doEjectPressed() = 0;

    LXQtMountPlugin *mPlugin;
};

#endif // EJECTACTION_H
