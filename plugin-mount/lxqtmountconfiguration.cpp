/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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


#include "lxqtmountconfiguration.h"
#include "ui_lxqtmountconfiguration.h"
#include <QComboBox>
#include <QDebug>

#define ACT_SHOW_MENU "showMenu"
#define ACT_SHOW_INFO "showInfo"
#define ACT_NOTHING   "nothing"


LxQtMountConfiguration::LxQtMountConfiguration(QSettings &settings, QWidget *parent) :
    LxQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LxQtMountConfiguration)
{
    ui->setupUi(this);
    ui->devAddedCombo->addItem(tr("Popup menu"), ACT_SHOW_MENU);
    ui->devAddedCombo->addItem(tr("Show info"),  ACT_SHOW_INFO);
    ui->devAddedCombo->addItem(tr("Do nothing"), ACT_NOTHING);

    loadSettings();
    connect(ui->devAddedCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(devAddedChanged(int)));
    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
}

LxQtMountConfiguration::~LxQtMountConfiguration()
{
    delete ui;
}


void LxQtMountConfiguration::loadSettings()
{
    setComboboxIndexByData(ui->devAddedCombo, settings().value("newDeviceAction", ACT_SHOW_INFO), 1);
}


void LxQtMountConfiguration::devAddedChanged(int index)
{
    QString s = ui->devAddedCombo->itemData(index).toString();
    settings().setValue("newDeviceAction", s);
}
