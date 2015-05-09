/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LxQt team
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

#include "spacerconfiguration.h"
#include "ui_spacerconfiguration.h"


const QStringList SpacerConfiguration::msTypes = 
    (QStringList()
     << QStringLiteral("lined")
     << QStringLiteral("dotted")
     << QStringLiteral("invisible")
     );

SpacerConfiguration::SpacerConfiguration(QSettings *settings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SpacerConfiguration)
    , mSettings(settings)
    , mOldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("SpacerConfigurationWindow");
    ui->setupUi(this);

    ui->typeCB->addItems(msTypes);


    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    loadSettings();

    connect(ui->sizeSB, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)));
    connect(ui->typeCB, &QComboBox::currentTextChanged, this, &SpacerConfiguration::typeChanged);
}

SpacerConfiguration::~SpacerConfiguration()
{
    delete ui;
}

void SpacerConfiguration::loadSettings()
{
    ui->sizeSB->setValue(mSettings->value("size", 8).toInt());
    ui->typeCB->setCurrentText(mSettings->value("spaceType", msTypes[0]).toString());

}

void SpacerConfiguration::sizeChanged(int value)
{
    mSettings->setValue("size", value);
}

void SpacerConfiguration::typeChanged(QString const & value)
{
    mSettings->setValue("spaceType", value);
}

void SpacerConfiguration::dialogButtonsAction(QAbstractButton *btn)
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

