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

#include "configpluginswidget.h"
#include "ui_configpluginswidget.h"
#include "addplugindialog.h"

#include <QPushButton>

ConfigPluginsWidget::ConfigPluginsWidget(LxQtPanel *panel, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ConfigPluginsWidget),
    mPanel(panel)
{
    ui->setupUi(this);

    connect(ui->pushButton_addPlugin, &QPushButton::clicked,
            this, &ConfigPluginsWidget::showAddPluginDialog);
}

ConfigPluginsWidget::~ConfigPluginsWidget()
{
    delete ui;
}

void ConfigPluginsWidget::reset()
{

}

void ConfigPluginsWidget::showAddPluginDialog()
{
    static AddPluginDialog *addPluginDialog = new AddPluginDialog(this);
    addPluginDialog->exec();
}
