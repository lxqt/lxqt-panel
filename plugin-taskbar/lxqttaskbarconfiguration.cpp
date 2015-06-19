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
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#include "lxqttaskbarconfiguration.h"
#include "ui_lxqttaskbarconfiguration.h"
#include <KWindowSystem/KWindowSystem>

LxQtTaskbarConfiguration::LxQtTaskbarConfiguration(QSettings &settings, QWidget *parent):
    QDialog(parent),
    ui(new Ui::LxQtTaskbarConfiguration),
    mSettings(settings),
    oldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("TaskbarConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    ui->buttonStyleCB->addItem(tr("Icon and text"), "IconText");
    ui->buttonStyleCB->addItem(tr("Only icon"), "Icon");
    ui->buttonStyleCB->addItem(tr("Only text"), "Text");

    ui->showDesktopNumCB->addItem(tr("Current"), 0);
    //Note: in KWindowSystem desktops are numbered from 1..N
    const int desk_cnt = KWindowSystem::numberOfDesktops();
    for (int i = 1; desk_cnt >= i; ++i)
        ui->showDesktopNumCB->addItem(QStringLiteral("%1 - %2").arg(i).arg(KWindowSystem::desktopName(i)), i);

    loadSettings();

    /* We use clicked() and activated(int) because these signals aren't emitting after programmaticaly
        change of state */
    connect(ui->limitByDesktopCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->limitByDesktopCB, &QCheckBox::stateChanged, ui->showDesktopNumCB, &QWidget::setEnabled);
    connect(ui->showDesktopNumCB, SIGNAL(activated(int)), this, SLOT(saveSettings()));
    connect(ui->limitByScreenCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->limitByMinimizedCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->raiseOnCurrentDesktopCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->buttonStyleCB, SIGNAL(activated(int)), this, SLOT(saveSettings()));
    connect(ui->buttonWidthSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
    connect(ui->buttonHeightSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
    connect(ui->autoRotateCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->middleClickCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->groupingGB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->showGroupOnHoverCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
}

LxQtTaskbarConfiguration::~LxQtTaskbarConfiguration()
{
    delete ui;
}

void LxQtTaskbarConfiguration::loadSettings()
{
    const bool showOnlyOneDesktopTasks = mSettings.value("showOnlyOneDesktopTasks", false).toBool();
    ui->limitByDesktopCB->setChecked(showOnlyOneDesktopTasks);
    ui->showDesktopNumCB->setCurrentIndex(ui->showDesktopNumCB->findData(mSettings.value("showDesktopNum", 0).toInt()));
    ui->showDesktopNumCB->setEnabled(showOnlyOneDesktopTasks);
    ui->limitByScreenCB->setChecked(mSettings.value("showOnlyCurrentScreenTasks", false).toBool());
    ui->limitByMinimizedCB->setChecked(mSettings.value("showOnlyMinimizedTasks", false).toBool());

    ui->autoRotateCB->setChecked(mSettings.value("autoRotate", true).toBool());
    ui->middleClickCB->setChecked(mSettings.value("closeOnMiddleClick", true).toBool());
    ui->raiseOnCurrentDesktopCB->setChecked(mSettings.value("raiseOnCurrentDesktop", false).toBool());
    ui->buttonStyleCB->setCurrentIndex(ui->buttonStyleCB->findData(mSettings.value("buttonStyle", "IconText")));
    ui->buttonWidthSB->setValue(mSettings.value("buttonWidth", 400).toInt());
    ui->buttonHeightSB->setValue(mSettings.value("buttonHeight", 100).toInt());
    ui->groupingGB->setChecked(mSettings.value("groupingEnabled",true).toBool());
    ui->showGroupOnHoverCB->setChecked(mSettings.value("showGroupOnHover",true).toBool());
}

void LxQtTaskbarConfiguration::saveSettings()
{
    mSettings.setValue("showOnlyOneDesktopTasks", ui->limitByDesktopCB->isChecked());
    mSettings.setValue("showDesktopNum", ui->showDesktopNumCB->itemData(ui->showDesktopNumCB->currentIndex()));
    mSettings.setValue("showOnlyCurrentScreenTasks", ui->limitByScreenCB->isChecked());
    mSettings.setValue("showOnlyMinimizedTasks", ui->limitByMinimizedCB->isChecked());
    mSettings.setValue("buttonStyle", ui->buttonStyleCB->itemData(ui->buttonStyleCB->currentIndex()));
    mSettings.setValue("buttonWidth", ui->buttonWidthSB->value());
    mSettings.setValue("buttonHeight", ui->buttonHeightSB->value());
    mSettings.setValue("autoRotate", ui->autoRotateCB->isChecked());
    mSettings.setValue("closeOnMiddleClick", ui->middleClickCB->isChecked());
    mSettings.setValue("raiseOnCurrentDesktop", ui->raiseOnCurrentDesktopCB->isChecked());
    mSettings.setValue("groupingEnabled",ui->groupingGB->isChecked());
    mSettings.setValue("showGroupOnHover",ui->showGroupOnHoverCB->isChecked());
}

void LxQtTaskbarConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        /* We have to disable signals for buttonWidthSB to prevent errors. Otherwise not all data
          could be restored */
        ui->buttonWidthSB->blockSignals(true);
        ui->buttonHeightSB->blockSignals(true);
        oldSettings.loadToSettings();
        loadSettings();
        ui->buttonWidthSB->blockSignals(false);
        ui->buttonHeightSB->blockSignals(false);
    }
    else
        close();
}
