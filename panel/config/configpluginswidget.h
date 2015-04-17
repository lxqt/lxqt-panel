/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Paulo Lieuthier <paulolieuthier@gmail.com>
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

#ifndef CONFIGPLUGINSWIDGET_H
#define CONFIGPLUGINSWIDGET_H

#include "../lxqtpanel.h"

#include <QWidget>

namespace Ui {
    class ConfigPluginsWidget;
}

class ConfigPluginsWidget : public QWidget
{
    Q_OBJECT

public:
    ConfigPluginsWidget(LxQtPanel *panel, QWidget* parent = 0);
    ~ConfigPluginsWidget();

signals:
    void changed();

public slots:
    void reset();

private slots:
    void showAddPluginDialog();

private:
    Ui::ConfigPluginsWidget *ui;
    LxQtPanel *mPanel;
};

#endif
