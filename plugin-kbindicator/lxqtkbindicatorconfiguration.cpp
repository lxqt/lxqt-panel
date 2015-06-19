/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2012      Razor team
 * Authors:
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


#include "lxqtkbindicatorconfiguration.h"
#include "ui_lxqtkbindicatorconfiguration.h"


LxQtKbIndicatorConfiguration::LxQtKbIndicatorConfiguration(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LxQtKbIndicatorConfiguration),
    mSettings(settings),
    oldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("KbIndicatorConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    connect(ui->capsLockCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->numLockCB, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->scrollLockCB, SIGNAL(clicked()), this, SLOT(saveSettings()));

    loadSettings();
}

LxQtKbIndicatorConfiguration::~LxQtKbIndicatorConfiguration()
{
    delete ui;
}

void LxQtKbIndicatorConfiguration::loadSettings()
{
    ui->capsLockCB->setChecked(mSettings->value("show_caps_lock", true).toBool());
    ui->numLockCB->setChecked(mSettings->value("show_num_lock", true).toBool());
    ui->scrollLockCB->setChecked(mSettings->value("show_scroll_lock", true).toBool());
}

void LxQtKbIndicatorConfiguration::saveSettings()
{
    mSettings->setValue("show_caps_lock", ui->capsLockCB->isChecked());
    mSettings->setValue("show_num_lock", ui->numLockCB->isChecked());
    mSettings->setValue("show_scroll_lock", ui->scrollLockCB->isChecked());
}

void LxQtKbIndicatorConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        oldSettings.loadToSettings();
        loadSettings();
    }
    else
        close();
}
