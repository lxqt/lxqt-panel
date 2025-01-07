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

#include "lxqtcustomcommandconfiguration.h"
#include "ui_lxqtcustomcommandconfiguration.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFontDialog>

LXQtCustomCommandConfiguration::LXQtCustomCommandConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtCustomCommandConfiguration),
    mLockSettingChanges(false)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &LXQtCustomCommandConfiguration::dialogButtonsAction);

    connect(ui->autoRotateCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::autoRotateChanged);
    connect(ui->fontButton, &QPushButton::clicked, this, &LXQtCustomCommandConfiguration::fontButtonClicked);
    connect(ui->commandPlainTextEdit, &QPlainTextEdit::textChanged, this, &LXQtCustomCommandConfiguration::commandPlainTextEditChanged);
    connect(ui->runWithBashCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::runWithBashCheckBoxChanged);
    connect(ui->outputImageCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::outputImageCheckBoxChanged);
    connect(ui->repeatCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::repeatCheckBoxChanged);
    connect(ui->repeatTimerSpinBox, &QSpinBox::editingFinished, this, &LXQtCustomCommandConfiguration::repeatTimerSpinBoxChanged);
    connect(ui->iconLineEdit, &QLineEdit::editingFinished, this, &LXQtCustomCommandConfiguration::iconLineEditChanged);
    connect(ui->iconBrowseButton, &QPushButton::clicked, this, &LXQtCustomCommandConfiguration::iconBrowseButtonClicked);
    connect(ui->textLineEdit, &QLineEdit::editingFinished, this, &LXQtCustomCommandConfiguration::textLineEditChanged);
    connect(ui->tooltipLineEdit, &QLineEdit::editingFinished, this, &LXQtCustomCommandConfiguration::tooltipLineEditChanged);
    connect(ui->maxWidthSpinBox, &QSpinBox::editingFinished, this, &LXQtCustomCommandConfiguration::maxWidthSpinBoxChanged);
    connect(ui->clickLineEdit, &QLineEdit::editingFinished, this, &LXQtCustomCommandConfiguration::clickLineEditChanged);
    connect(ui->wheelUpLineEdit, &QLineEdit::editingFinished, this, &LXQtCustomCommandConfiguration::wheelUpLineEditChanged);
    connect(ui->wheelDownLineEdit, &QLineEdit::editingFinished, this, &LXQtCustomCommandConfiguration::wheelDownLineEditChanged);
}

LXQtCustomCommandConfiguration::~LXQtCustomCommandConfiguration()
{
    delete ui;
}

void LXQtCustomCommandConfiguration::loadSettings()
{
    mLockSettingChanges = true;

    ui->autoRotateCheckBox->setChecked(settings().value(QStringLiteral("autoRotate"), true).toBool());
    ui->fontButton->setText(settings().value(QStringLiteral("font"), font().toString()).toString());
    ui->commandPlainTextEdit->setPlainText(settings().value(QStringLiteral("command"), QStringLiteral("echo Configure...")).toString());
    ui->runWithBashCheckBox->setChecked(settings().value(QStringLiteral("runWithBash"), true).toBool());
    ui->outputImageCheckBox->setChecked(settings().value(QStringLiteral("outputImage"), false).toBool());
    ui->repeatCheckBox->setChecked(settings().value(QStringLiteral("repeat"), true).toBool());
    ui->repeatTimerSpinBox->setEnabled(ui->repeatCheckBox->isChecked());
    ui->repeatTimerSpinBox->setValue(settings().value(QStringLiteral("repeatTimer"), 5).toInt());
    ui->iconLineEdit->setText(settings().value(QStringLiteral("icon"), QString()).toString());
    ui->textLineEdit->setText(settings().value(QStringLiteral("text"), QStringLiteral("%1")).toString());
    ui->tooltipLineEdit->setText(settings().value(QStringLiteral("tooltip"), QString()).toString());
    ui->maxWidthSpinBox->setValue(settings().value(QStringLiteral("maxWidth"), 200).toInt());
    ui->clickLineEdit->setText(settings().value(QStringLiteral("click"), QString()).toString());
    ui->wheelUpLineEdit->setText(settings().value(QStringLiteral("wheelUp"), QString()).toString());
    ui->wheelDownLineEdit->setText(settings().value(QStringLiteral("wheelDown"), QString()).toString());

    mLockSettingChanges = false;
}

void LXQtCustomCommandConfiguration::autoRotateChanged(bool autoRotate)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("autoRotate"), autoRotate);
}

void LXQtCustomCommandConfiguration::fontButtonClicked()
{
    bool ok;
    QFont currentFont;
    currentFont.fromString(ui->fontButton->text());
    QFont getFont = QFontDialog::getFont(&ok, currentFont, this);
    if (ok)
    {
        auto fontString = getFont.toString();
        ui->fontButton->setText(fontString);
        settings().setValue(QStringLiteral("font"), fontString);
    }
}

void LXQtCustomCommandConfiguration::commandPlainTextEditChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("command"), ui->commandPlainTextEdit->toPlainText().trimmed());
}

void LXQtCustomCommandConfiguration::runWithBashCheckBoxChanged(bool runWithBash)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("runWithBash"), runWithBash);
}

void LXQtCustomCommandConfiguration::outputImageCheckBoxChanged(bool outputImage)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("outputImage"), outputImage);
}

void LXQtCustomCommandConfiguration::repeatCheckBoxChanged(bool repeat)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("repeat"), repeat);
    ui->repeatTimerSpinBox->setEnabled(repeat);
}

void LXQtCustomCommandConfiguration::repeatTimerSpinBoxChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("repeatTimer"), ui->repeatTimerSpinBox->value());
}

void LXQtCustomCommandConfiguration::iconLineEditChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("icon"), ui->iconLineEdit->text());
}

void LXQtCustomCommandConfiguration::iconBrowseButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Icon File"), QString(), tr("Images (*.png *.svg *.xpm *.jpg)"));
    ui->iconLineEdit->setText(fileName);
}

void LXQtCustomCommandConfiguration::textLineEditChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("text"), ui->textLineEdit->text());
}

void LXQtCustomCommandConfiguration::tooltipLineEditChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("tooltip"), ui->tooltipLineEdit->text());
}

void LXQtCustomCommandConfiguration::maxWidthSpinBoxChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("maxWidth"), ui->maxWidthSpinBox->value());
}

void LXQtCustomCommandConfiguration::clickLineEditChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("click"), ui->clickLineEdit->text().trimmed());
}

void LXQtCustomCommandConfiguration::wheelUpLineEditChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("wheelUp"), ui->wheelUpLineEdit->text().trimmed());
}

void LXQtCustomCommandConfiguration::wheelDownLineEditChanged()
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("wheelDown"), ui->wheelDownLineEdit->text().trimmed());
}
