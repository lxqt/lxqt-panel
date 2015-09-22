/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *  Balázs Béla <balazsbela[at]gmail.com>
 *  Paulo Lieuthier <paulolieuthier@gmail.com>
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

#ifndef STATUSNOTIFIERWIDGET_H
#define STATUSNOTIFIERWIDGET_H

#include <QDir>

#include <LXQt/GridLayout>

#include "statusnotifierbutton.h"
#include "statusnotifierwatcher.h"

class StatusNotifierWidget : public QWidget
{
    Q_OBJECT

public:
    StatusNotifierWidget(ILXQtPanelPlugin *plugin, QWidget *parent = 0);
    ~StatusNotifierWidget();

signals:

public slots:
    void itemAdded(QString serviceAndPath);
    void itemRemoved(const QString &serviceAndPath);

    void realign();

private:
    ILXQtPanelPlugin *mPlugin;
    StatusNotifierWatcher *mWatcher;

    QHash<QString, StatusNotifierButton*> mServices;
};

#endif // STATUSNOTIFIERWIDGET_H
