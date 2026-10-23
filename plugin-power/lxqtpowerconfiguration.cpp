/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2026 LXQt team
 * Authors:
 *   envy-vain <340632583+envy-vain@users.noreply.github.com>
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

#include "lxqtpowerconfiguration.h"
#include "ui_lxqtpowerconfiguration.h"

#include <QPushButton>

LXQtPowerConfiguration::LXQtPowerConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtPowerConfiguration),
    mLockSettingChanges(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QStringLiteral("PowerConfigurationWindow"));
    ui->setupUi(this);

    connect(ui->buttons, &QDialogButtonBox::clicked, this, &LXQtPowerConfiguration::dialogButtonsAction);
    if (auto btn = ui->buttons->button(QDialogButtonBox::Close))
        btn->setDefault(true);

    loadSettings();

    connect(ui->showBatteryCB,         &QCheckBox::toggled,           this, &LXQtPowerConfiguration::showBatteryChanged);
    connect(ui->showCpuCB,             &QCheckBox::toggled,           this, &LXQtPowerConfiguration::showCpuChanged);
    connect(ui->updateIntervalSpinBox, &QDoubleSpinBox::valueChanged, this, &LXQtPowerConfiguration::updateIntervalChanged);
}

LXQtPowerConfiguration::~LXQtPowerConfiguration()
{
    delete ui;
}

void LXQtPowerConfiguration::loadSettings()
{
    mLockSettingChanges = true;

    ui->showBatteryCB->setChecked(settings().value(QStringLiteral("showBattery"), true).toBool());
    ui->showCpuCB->setChecked(settings().value(QStringLiteral("showCpu"), true).toBool());
    ui->updateIntervalSpinBox->setValue(settings().value(QStringLiteral("updateInterval"), 2000).toInt() / 1000.0);

    mLockSettingChanges = false;
}

void LXQtPowerConfiguration::showBatteryChanged(bool value)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("showBattery"), value);
}

void LXQtPowerConfiguration::showCpuChanged(bool value)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("showCpu"), value);
}

void LXQtPowerConfiguration::updateIntervalChanged(double value)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("updateInterval"), static_cast<int>(value * 1000));
}
