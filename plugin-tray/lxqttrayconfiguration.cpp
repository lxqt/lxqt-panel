/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Corentin Ferry <cferr@openmailbox.org>
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


#include <QInputDialog>
#include <QStandardItemModel>
#include <QStandardItem>

#include "lxqttrayconfiguration.h"
#include "ui_lxqttrayconfiguration.h"

LXQtTrayConfiguration::LXQtTrayConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtTrayConfiguration)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("TrayConfigurationWindow");
    ui->setupUi(this);

    //connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonsAction(QAbstractButton*)));

    loadSettings();
    
    connect(ui->buttons, SIGNAL(accepted()), SLOT(saveSettings()));
    connect(ui->buttons, SIGNAL(rejected()), SLOT(reject()));
}

LXQtTrayConfiguration::~LXQtTrayConfiguration()
{
    delete ui;
}

void LXQtTrayConfiguration::loadSettings()
{
    /* Let's specify a custom icon size of 16px by default. There's nothing wrong
       in setting it hard there, as the option isn't checked by default */
    ui->useCustomSizeCB->setChecked(settings().value("useCustomTrayIconSize", false).toBool());
    ui->iconSizeSB->setValue(settings().value("customTrayIconSize", TRAY_ICON_SIZE_DEFAULT).toInt());
}

void LXQtTrayConfiguration::saveSettings()
{
    settings().setValue("useCustomTrayIconSize", ui->useCustomSizeCB->isChecked());
    settings().setValue("customTrayIconSize", ui->iconSizeSB->value());
    
    this->accept();
}
