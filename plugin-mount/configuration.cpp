/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
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


#include "configuration.h"
#include "ui_configuration.h"

#include <QComboBox>
#include <QDialogButtonBox>

Configuration::Configuration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::Configuration),
    mLockSettingChanges(false)
{
    ui->setupUi(this);

    // Set size policies
    QSizePolicy sp = ui->devAddedLabel->sizePolicy();
    sp.setHorizontalStretch(1);
    ui->devAddedLabel->setSizePolicy(sp);

    sp = ui->devAddedCombo->sizePolicy();
    sp.setHorizontalStretch(1);
    ui->devAddedCombo->setSizePolicy(sp);

    sp = ui->ejectPressedLabel->sizePolicy();
    sp.setHorizontalStretch(1);
    ui->ejectPressedLabel->setSizePolicy(sp);

    sp = ui->ejectPressedCombo->sizePolicy();
    sp.setHorizontalStretch(1);
    ui->ejectPressedCombo->setSizePolicy(sp);

    // Fill combo boxes
    if (QGuiApplication::platformName() != QStringLiteral("wayland"))
    {
        // WARNING: The popup menu does not work consistently under Wayland.
        // See LXQtMountPlugin::settingsChanged() for an explanation.
        ui->devAddedCombo->addItem(tr("Popup menu"), QLatin1String(ACT_SHOW_MENU));
    }
    ui->devAddedCombo->addItem(tr("Show info"),  QLatin1String(ACT_SHOW_INFO));
    ui->devAddedCombo->addItem(tr("Do nothing"), QLatin1String(ACT_NOTHING));

    ui->ejectPressedCombo->addItem(tr("Do nothing"), QLatin1String(ACT_NOTHING));
    ui->ejectPressedCombo->addItem(tr("Eject All Optical Drives"), QLatin1String(ACT_EJECT_OPTICAL));

    adjustSize();

    loadSettings();
    connect(ui->devAddedCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Configuration::devAddedChanged);
    connect(ui->ejectPressedCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Configuration::ejectPressedChanged);
    connect(ui->buttons, &QDialogButtonBox::clicked, this, &Configuration::dialogButtonsAction);
}

Configuration::~Configuration()
{
    delete ui;
}

void Configuration::loadSettings()
{
    mLockSettingChanges = true;

    int defaultIndex = QGuiApplication::platformName() == QStringLiteral("wayland") ? 0 : 1;

    QVariant value = settings().value(QLatin1String(CFG_KEY_ACTION), QLatin1String(ACT_SHOW_INFO));
    setComboboxIndexByData(ui->devAddedCombo, value, defaultIndex);

    value = settings().value(QLatin1String(CFG_EJECT_ACTION), QLatin1String(ACT_NOTHING));
    setComboboxIndexByData(ui->ejectPressedCombo, value, defaultIndex);

    mLockSettingChanges = false;
}

void Configuration::devAddedChanged(int index)
{
    if (!mLockSettingChanges)
    {
        QString s = ui->devAddedCombo->itemData(index).toString();
        settings().setValue(QLatin1String(CFG_KEY_ACTION), s);
    }
}

void Configuration::ejectPressedChanged(int index)
{
    if (!mLockSettingChanges)
    {
        QString s = ui->ejectPressedCombo->itemData(index).toString();
        settings().setValue(QLatin1String(CFG_EJECT_ACTION), s);
    }
}
