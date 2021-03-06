/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2021 LXQt team
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

#include "lxqtcustomconfiguration.h"
#include "ui_lxqtcustomconfiguration.h"

#include <QFileDialog>
#include <QFontDialog>

LXQtCustomConfiguration::LXQtCustomConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtCustomConfiguration)
{
    ui->setupUi(this);
    loadSettings();

    connect(ui->autoRotateCheckBox, &QCheckBox::toggled, this, &LXQtCustomConfiguration::autoRotateChanged);
    connect(ui->fontButton, &QPushButton::clicked, this, &LXQtCustomConfiguration::fontButtonClicked);
    connect(ui->commandPlainTextEdit, &QPlainTextEdit::textChanged, this, &LXQtCustomConfiguration::commandPlainTextEditChanged);
    connect(ui->runWithBashCheckBox, &QCheckBox::toggled, this, &LXQtCustomConfiguration::runWithBashCheckBoxChanged);
    connect(ui->repeatGroupBox, &QGroupBox::toggled, this, &LXQtCustomConfiguration::repeatGroupBoxChanged);
    connect(ui->repeatTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LXQtCustomConfiguration::repeatTimerSpinBoxChanged);
    connect(ui->iconLineEdit, &QLineEdit::textChanged, this, &LXQtCustomConfiguration::iconLineEditChanged);
    connect(ui->iconBrowseButton, &QPushButton::clicked, this, &LXQtCustomConfiguration::iconBrowseButtonClicked);
    connect(ui->textLineEdit, &QLineEdit::textChanged, this, &LXQtCustomConfiguration::textLineEditChanged);
    connect(ui->maxWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LXQtCustomConfiguration::maxWidthSpinBoxChanged);
    connect(ui->clickLineEdit, &QLineEdit::textChanged, this, &LXQtCustomConfiguration::clickLineEditChanged);
    connect(ui->wheelUpLineEdit, &QLineEdit::textChanged, this, &LXQtCustomConfiguration::wheelUpLineEditChanged);
    connect(ui->wheelDownLineEdit, &QLineEdit::textChanged, this, &LXQtCustomConfiguration::wheelDownLineEditChanged);
}

LXQtCustomConfiguration::~LXQtCustomConfiguration()
{
    delete ui;
}

void LXQtCustomConfiguration::autoRotateChanged(bool autoRotate)
{
    settings().setValue(QStringLiteral("autoRotate"), autoRotate);
}

void LXQtCustomConfiguration::fontButtonClicked()
{
    bool ok;
    QFont currentFont;
    currentFont.fromString(ui->fontButton->text());
    QFont getFont = QFontDialog::getFont(&ok, currentFont, this);
    if (ok)
        fontChanged(getFont.toString());
}

void LXQtCustomConfiguration::fontChanged(QString mFont)
{
    ui->fontButton->setText(mFont);
    settings().setValue(QStringLiteral("font"), mFont);
}

void LXQtCustomConfiguration::commandPlainTextEditChanged()
{
    settings().setValue(QStringLiteral("command"), ui->commandPlainTextEdit->toPlainText());
}

void LXQtCustomConfiguration::runWithBashCheckBoxChanged(bool runWithBash)
{
    settings().setValue(QStringLiteral("runWithBash"), runWithBash);
}

void LXQtCustomConfiguration::repeatGroupBoxChanged(bool repeat)
{
    settings().setValue(QStringLiteral("repeat"), repeat);
}

void LXQtCustomConfiguration::repeatTimerSpinBoxChanged(int repeatTimer)
{
    settings().setValue(QStringLiteral("repeatTimer"), repeatTimer);
}

void LXQtCustomConfiguration::iconLineEditChanged(QString icon)
{
    settings().setValue(QStringLiteral("icon"), icon);
}

void LXQtCustomConfiguration::iconBrowseButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Icon File"), QString(), tr("Images (*.png *.svg *.xpm *.jpg)"));
    ui->iconLineEdit->setText(fileName);
}

void LXQtCustomConfiguration::textLineEditChanged(QString text)
{
    settings().setValue(QStringLiteral("text"), text);
}

void LXQtCustomConfiguration::maxWidthSpinBoxChanged(int maxWidth)
{
    settings().setValue(QStringLiteral("maxWidth"), maxWidth);
}

void LXQtCustomConfiguration::clickLineEditChanged(QString click)
{
    settings().setValue(QStringLiteral("click"), click);
}

void LXQtCustomConfiguration::wheelUpLineEditChanged(QString wheelUp)
{
    settings().setValue(QStringLiteral("wheelUp"), wheelUp);
}

void LXQtCustomConfiguration::wheelDownLineEditChanged(QString wheelDown)
{
    settings().setValue(QStringLiteral("wheelDown"), wheelDown);
}

void LXQtCustomConfiguration::loadSettings()
{
    ui->autoRotateCheckBox->setChecked(settings().value(QStringLiteral("autoRotate"), true).toBool());
    ui->fontButton->setText(settings().value(QStringLiteral("font"), font().toString()).toString());
    ui->commandPlainTextEdit->setPlainText(settings().value(QStringLiteral("command"), QString()).toString());
    ui->runWithBashCheckBox->setChecked(settings().value(QStringLiteral("runWithBash"), true).toBool());
    ui->repeatGroupBox->setChecked(settings().value(QStringLiteral("repeat"), true).toBool());
    ui->repeatTimerSpinBox->setValue(settings().value(QStringLiteral("repeatTimer"), 1000).toInt());
    ui->iconLineEdit->setText(settings().value(QStringLiteral("icon"), QString()).toString());
    ui->textLineEdit->setText(settings().value(QStringLiteral("text"), QStringLiteral("%1")).toString());
    ui->maxWidthSpinBox->setValue(settings().value(QStringLiteral("maxWidth"), 200).toInt());
    ui->clickLineEdit->setText(settings().value(QStringLiteral("click"), QString()).toString());
    ui->wheelUpLineEdit->setText(settings().value(QStringLiteral("wheelUp"), QString()).toString());
    ui->wheelDownLineEdit->setText(settings().value(QStringLiteral("wheelDown"), QString()).toString());
}
