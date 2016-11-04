/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 * http://lxqt.org
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
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

LXQtTaskbarConfiguration::LXQtTaskbarConfiguration(PluginSettings *settings, QWidget *parent):
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtTaskbarConfiguration)
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
        ui->showDesktopNumCB->addItem(QString("%1 - %2").arg(i).arg(KWindowSystem::desktopName(i)), i);

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
    connect(ui->iconByClassCB, &QCheckBox::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->cycleOnWheelScroll, &QCheckBox::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
}

LXQtTaskbarConfiguration::~LXQtTaskbarConfiguration()
{
    delete ui;
}

void LXQtTaskbarConfiguration::loadSettings()
{
    const bool showOnlyOneDesktopTasks = settings().value("showOnlyOneDesktopTasks", false).toBool();
    ui->limitByDesktopCB->setChecked(showOnlyOneDesktopTasks);
    ui->showDesktopNumCB->setCurrentIndex(ui->showDesktopNumCB->findData(settings().value("showDesktopNum", 0).toInt()));
    ui->showDesktopNumCB->setEnabled(showOnlyOneDesktopTasks);
    ui->limitByScreenCB->setChecked(settings().value("showOnlyCurrentScreenTasks", false).toBool());
    ui->limitByMinimizedCB->setChecked(settings().value("showOnlyMinimizedTasks", false).toBool());

    ui->autoRotateCB->setChecked(settings().value("autoRotate", true).toBool());
    ui->middleClickCB->setChecked(settings().value("closeOnMiddleClick", true).toBool());
    ui->raiseOnCurrentDesktopCB->setChecked(settings().value("raiseOnCurrentDesktop", false).toBool());
    ui->buttonStyleCB->setCurrentIndex(ui->buttonStyleCB->findData(settings().value("buttonStyle", "IconText")));
    ui->buttonWidthSB->setValue(settings().value("buttonWidth", 400).toInt());
    ui->buttonHeightSB->setValue(settings().value("buttonHeight", 100).toInt());
    ui->groupingGB->setChecked(settings().value("groupingEnabled",true).toBool());
    ui->showGroupOnHoverCB->setChecked(settings().value("showGroupOnHover",true).toBool());
    ui->iconByClassCB->setChecked(settings().value("iconByClass", false).toBool());
    ui->cycleOnWheelScroll->setChecked(settings().value("cycleOnWheelScroll", true).toBool());
}

void LXQtTaskbarConfiguration::saveSettings()
{
    settings().setValue("showOnlyOneDesktopTasks", ui->limitByDesktopCB->isChecked());
    settings().setValue("showDesktopNum", ui->showDesktopNumCB->itemData(ui->showDesktopNumCB->currentIndex()));
    settings().setValue("showOnlyCurrentScreenTasks", ui->limitByScreenCB->isChecked());
    settings().setValue("showOnlyMinimizedTasks", ui->limitByMinimizedCB->isChecked());
    settings().setValue("buttonStyle", ui->buttonStyleCB->itemData(ui->buttonStyleCB->currentIndex()));
    settings().setValue("buttonWidth", ui->buttonWidthSB->value());
    settings().setValue("buttonHeight", ui->buttonHeightSB->value());
    settings().setValue("autoRotate", ui->autoRotateCB->isChecked());
    settings().setValue("closeOnMiddleClick", ui->middleClickCB->isChecked());
    settings().setValue("raiseOnCurrentDesktop", ui->raiseOnCurrentDesktopCB->isChecked());
    settings().setValue("groupingEnabled",ui->groupingGB->isChecked());
    settings().setValue("showGroupOnHover",ui->showGroupOnHoverCB->isChecked());
    settings().setValue("iconByClass",ui->iconByClassCB->isChecked());
    settings().setValue("cycleOnWheelScroll",ui->cycleOnWheelScroll->isChecked());
}
