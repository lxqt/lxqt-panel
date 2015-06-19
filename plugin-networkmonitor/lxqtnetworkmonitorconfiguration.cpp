/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2011      Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#include "lxqtnetworkmonitorconfiguration.h"
#include "ui_lxqtnetworkmonitorconfiguration.h"

extern "C" {
#include <statgrab.h>
}

#ifdef __sg_public
// since libstatgrab 0.90 this macro is defined, so we use it for version check
#define STATGRAB_NEWER_THAN_0_90 	1
#endif

LxQtNetworkMonitorConfiguration::LxQtNetworkMonitorConfiguration(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LxQtNetworkMonitorConfiguration),
    mSettings(settings),
    mOldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("NetworkMonitorConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(ui->iconCB, SIGNAL(currentIndexChanged(int)), SLOT(saveSettings()));
    connect(ui->interfaceCB, SIGNAL(currentIndexChanged(int)), SLOT(saveSettings()));

    loadSettings();
}

LxQtNetworkMonitorConfiguration::~LxQtNetworkMonitorConfiguration()
{
    delete ui;
}

void LxQtNetworkMonitorConfiguration::saveSettings()
{
    mSettings->setValue("icon", ui->iconCB->currentIndex());
    mSettings->setValue("interface", ui->interfaceCB->currentText());
}

void LxQtNetworkMonitorConfiguration::loadSettings()
{
    ui->iconCB->setCurrentIndex(mSettings->value("icon", 1).toInt());

    int count;
#ifdef STATGRAB_NEWER_THAN_0_90
    size_t ret_count;
    sg_network_iface_stats* stats = sg_get_network_iface_stats(&ret_count);
    count = (int)ret_count;
#else
    sg_network_iface_stats* stats = sg_get_network_iface_stats(&count);
#endif
    for (int ix = 0; ix < count; ix++)
        ui->interfaceCB->addItem(stats[ix].interface_name);

    QString interface = mSettings->value("interface").toString();
    ui->interfaceCB->setCurrentIndex(qMax(qMin(0, count - 1), ui->interfaceCB->findText(interface)));
}

void LxQtNetworkMonitorConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        mOldSettings.loadToSettings();
        loadSettings();
    }
    else
    {
        close();
    }
}

