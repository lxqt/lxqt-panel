/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
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

#include "../panel/lxqtpanelapplication.h"
#include "../panel/backends/ilxqtabstractwmiface.h"

LXQtTaskbarConfiguration::LXQtTaskbarConfiguration(PluginSettings *settings, QWidget *parent):
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtTaskbarConfiguration)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QStringLiteral("TaskbarConfigurationWindow"));
    ui->setupUi(this);

    connect(ui->buttons, &QDialogButtonBox::clicked, this, &LXQtTaskbarConfiguration::dialogButtonsAction);

    ui->buttonStyleCB->addItem(tr("Icon and text"), QLatin1String("IconText"));
    ui->buttonStyleCB->addItem(tr("Only icon"), QLatin1String("Icon"));
    ui->buttonStyleCB->addItem(tr("Only text"), QLatin1String("Text"));

    ui->wheelEventsActionCB->addItem(tr("Disabled"), 0);
    ui->wheelEventsActionCB->addItem(tr("Cycle windows on wheel scrolling"), 1);
    ui->wheelEventsActionCB->addItem(tr("Scroll up to raise, down to minimize"), 2);
    ui->wheelEventsActionCB->addItem(tr("Scroll up to minimize, down to raise"), 3);
    ui->wheelEventsActionCB->addItem(tr("Scroll up to move to next desktop, down to previous"), 4);
    ui->wheelEventsActionCB->addItem(tr("Scroll up to move to previous desktop, down to next"), 5);

    LXQtPanelApplication *a = reinterpret_cast<LXQtPanelApplication*>(qApp);
    auto wmBackend = a->getWMBackend();

    ui->showDesktopNumCB->addItem(tr("Current"), 0);
    //Note: in KWindowSystem desktops are numbered from 1..N
    const int desk_cnt = wmBackend->getWorkspacesCount();
    for (int i = 1; desk_cnt >= i; ++i)
    {
        auto deskName = wmBackend->getWorkspaceName(i);
        if (deskName.isEmpty())
            deskName = tr("Desktop %1").arg(i);
        ui->showDesktopNumCB->addItem(QString(QStringLiteral("%1 - %2")).arg(i).arg(deskName), i);
    }

    loadSettings();
    ui->ungroupedNextToExistingCB->setEnabled(!(ui->groupingGB->isChecked()));
    /* We use clicked() and activated(int) because these signals aren't emitting after programmatically
        change of state */
    connect(ui->limitByDesktopCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
#if (QT_VERSION >= QT_VERSION_CHECK(6,7,0))
    connect(ui->limitByDesktopCB, &QCheckBox::checkStateChanged, ui->showDesktopNumCB, &QWidget::setEnabled);
#else
    connect(ui->limitByDesktopCB, &QCheckBox::stateChanged, ui->showDesktopNumCB, &QWidget::setEnabled);
#endif
    connect(ui->showDesktopNumCB, QOverload<int>::of(&QComboBox::activated), this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->limitByScreenCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->limitByMinimizedCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->raiseOnCurrentDesktopCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->buttonStyleCB, QOverload<int>::of(&QComboBox::activated), this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->buttonWidthSB, &QAbstractSpinBox::editingFinished, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->buttonHeightSB, &QAbstractSpinBox::editingFinished, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->autoRotateCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->middleClickCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->groupingGB, &QGroupBox::clicked, this, [this] {
        saveSettings();
        ui->ungroupedNextToExistingCB->setEnabled(!(ui->groupingGB->isChecked()));
    });
    connect(ui->showGroupOnHoverCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->ungroupedNextToExistingCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->iconByClassCB, &QAbstractButton::clicked, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->wheelEventsActionCB, QOverload<int>::of(&QComboBox::activated), this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->wheelDeltaThresholdSB, &QAbstractSpinBox::editingFinished, this, &LXQtTaskbarConfiguration::saveSettings);
    connect(ui->excludeLE, &QLineEdit::editingFinished, this, &LXQtTaskbarConfiguration::saveSettings);
}

LXQtTaskbarConfiguration::~LXQtTaskbarConfiguration()
{
    delete ui;
}

void LXQtTaskbarConfiguration::loadSettings()
{
    const bool showOnlyOneDesktopTasks = settings().value(QStringLiteral("showOnlyOneDesktopTasks"), false).toBool();
    ui->limitByDesktopCB->setChecked(showOnlyOneDesktopTasks);
    ui->showDesktopNumCB->setCurrentIndex(ui->showDesktopNumCB->findData(settings().value(QStringLiteral("showDesktopNum"), 0).toInt()));
    ui->showDesktopNumCB->setEnabled(showOnlyOneDesktopTasks);
    ui->limitByScreenCB->setChecked(settings().value(QStringLiteral("showOnlyCurrentScreenTasks"), false).toBool());
    ui->limitByMinimizedCB->setChecked(settings().value(QStringLiteral("showOnlyMinimizedTasks"), false).toBool());

    ui->autoRotateCB->setChecked(settings().value(QStringLiteral("autoRotate"), true).toBool());
    ui->middleClickCB->setChecked(settings().value(QStringLiteral("closeOnMiddleClick"), true).toBool());
    ui->raiseOnCurrentDesktopCB->setChecked(settings().value(QStringLiteral("raiseOnCurrentDesktop"), false).toBool());
    ui->buttonStyleCB->setCurrentIndex(ui->buttonStyleCB->findData(settings().value(QStringLiteral("buttonStyle"), QLatin1String("IconText"))));
    ui->buttonWidthSB->setValue(settings().value(QStringLiteral("buttonWidth"), 220).toInt());
    ui->buttonHeightSB->setValue(settings().value(QStringLiteral("buttonHeight"), 100).toInt());
    ui->groupingGB->setChecked(settings().value(QStringLiteral("groupingEnabled"), true).toBool());
    ui->showGroupOnHoverCB->setChecked(settings().value(QStringLiteral("showGroupOnHover"), true).toBool());
    ui->ungroupedNextToExistingCB->setChecked(settings().value(QStringLiteral("ungroupedNextToExisting"), false).toBool());
    ui->iconByClassCB->setChecked(settings().value(QStringLiteral("iconByClass"), false).toBool());
    ui->wheelEventsActionCB->setCurrentIndex(ui->wheelEventsActionCB->findData(settings().value(QStringLiteral("wheelEventsAction"), 1).toInt()));
    ui->wheelDeltaThresholdSB->setValue(settings().value(QStringLiteral("wheelDeltaThreshold"), 300).toInt());
    ui->excludeLE->setText(settings().value(QStringLiteral("excludedList")).toString());
}

void LXQtTaskbarConfiguration::saveSettings()
{
    settings().setValue(QStringLiteral("showOnlyOneDesktopTasks"), ui->limitByDesktopCB->isChecked());
    settings().setValue(QStringLiteral("showDesktopNum"), ui->showDesktopNumCB->itemData(ui->showDesktopNumCB->currentIndex()));
    settings().setValue(QStringLiteral("showOnlyCurrentScreenTasks"), ui->limitByScreenCB->isChecked());
    settings().setValue(QStringLiteral("showOnlyMinimizedTasks"), ui->limitByMinimizedCB->isChecked());
    settings().setValue(QStringLiteral("buttonStyle"), ui->buttonStyleCB->itemData(ui->buttonStyleCB->currentIndex()));
    settings().setValue(QStringLiteral("buttonWidth"), ui->buttonWidthSB->value());
    settings().setValue(QStringLiteral("buttonHeight"), ui->buttonHeightSB->value());
    settings().setValue(QStringLiteral("autoRotate"), ui->autoRotateCB->isChecked());
    settings().setValue(QStringLiteral("closeOnMiddleClick"), ui->middleClickCB->isChecked());
    settings().setValue(QStringLiteral("raiseOnCurrentDesktop"), ui->raiseOnCurrentDesktopCB->isChecked());
    settings().setValue(QStringLiteral("groupingEnabled"), ui->groupingGB->isChecked());
    settings().setValue(QStringLiteral("showGroupOnHover"), ui->showGroupOnHoverCB->isChecked());
    settings().setValue(QStringLiteral("ungroupedNextToExisting"), ui->ungroupedNextToExistingCB->isChecked());
    settings().setValue(QStringLiteral("iconByClass"), ui->iconByClassCB->isChecked());
    settings().setValue(QStringLiteral("wheelEventsAction"), ui->wheelEventsActionCB->itemData(ui->wheelEventsActionCB->currentIndex()));
    settings().setValue(QStringLiteral("wheelDeltaThreshold"), ui->wheelDeltaThresholdSB->value());
    settings().setValue(QStringLiteral("excludedList"), ui->excludeLE->text());
}
