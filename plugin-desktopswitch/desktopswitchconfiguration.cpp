/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
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

#include "desktopswitchconfiguration.h"
#include "ui_desktopswitchconfiguration.h"

#include "../panel/lxqtpanelapplication.h"
#include "../panel/backends/ilxqtabstractwmiface.h"

#include <KX11Extras>
#include <QScreen>
#include <QTimer>

DesktopSwitchConfiguration::DesktopSwitchConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::DesktopSwitchConfiguration)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QStringLiteral("DesktopSwitchConfigurationWindow"));
    ui->setupUi(this);

    connect(ui->buttons, &QDialogButtonBox::clicked, this, &DesktopSwitchConfiguration::dialogButtonsAction);

    loadSettings();

    connect(ui->rowsSB,           &QSpinBox::valueChanged,         this, &DesktopSwitchConfiguration::rowsChanged);
    connect(ui->labelTypeCB,      &QComboBox::currentIndexChanged, this, &DesktopSwitchConfiguration::labelTypeChanged);
    connect(ui->showOnlyActiveCB, &QAbstractButton::toggled,       this, [this] (bool checked) {
        this->settings().setValue(QStringLiteral("showOnlyActive"), checked);
    });
}

DesktopSwitchConfiguration::~DesktopSwitchConfiguration()
{
    delete ui;
}

void DesktopSwitchConfiguration::loadSettings()
{
    ui->rowsSB->setValue(settings().value(QStringLiteral("rows"), 1).toInt());
    ui->labelTypeCB->setCurrentIndex(settings().value(QStringLiteral("labelType"), 0).toInt());
    ui->showOnlyActiveCB->setChecked(settings().value(QStringLiteral("showOnlyActive"), false).toBool());
}

void DesktopSwitchConfiguration::rowsChanged(int value)
{
    settings().setValue(QStringLiteral("rows"), value);
}

void DesktopSwitchConfiguration::labelTypeChanged(int type)
{
    settings().setValue(QStringLiteral("labelType"), type);
}
