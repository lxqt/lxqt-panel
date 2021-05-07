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
    ui(new Ui::LXQtCustomCommandConfiguration)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    loadSettings();
    setUiValues();

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &LXQtCustomCommandConfiguration::buttonBoxClicked);
    connect(ui->autoRotateCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::autoRotateChanged);
    connect(ui->fontButton, &QPushButton::clicked, this, &LXQtCustomCommandConfiguration::fontButtonClicked);
    connect(ui->commandPlainTextEdit, &QPlainTextEdit::textChanged, this, &LXQtCustomCommandConfiguration::commandPlainTextEditChanged);
    connect(ui->runWithBashCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::runWithBashCheckBoxChanged);
    connect(ui->repeatCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::repeatCheckBoxChanged);
    connect(ui->repeatTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LXQtCustomCommandConfiguration::repeatTimerSpinBoxChanged);
    connect(ui->iconLineEdit, &QLineEdit::textChanged, this, &LXQtCustomCommandConfiguration::iconLineEditChanged);
    connect(ui->iconBrowseButton, &QPushButton::clicked, this, &LXQtCustomCommandConfiguration::iconBrowseButtonClicked);
    connect(ui->textLineEdit, &QLineEdit::textChanged, this, &LXQtCustomCommandConfiguration::textLineEditChanged);
    connect(ui->maxWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LXQtCustomCommandConfiguration::maxWidthSpinBoxChanged);
    connect(ui->clickLineEdit, &QLineEdit::textChanged, this, &LXQtCustomCommandConfiguration::clickLineEditChanged);
    connect(ui->wheelUpLineEdit, &QLineEdit::textChanged, this, &LXQtCustomCommandConfiguration::wheelUpLineEditChanged);
    connect(ui->wheelDownLineEdit, &QLineEdit::textChanged, this, &LXQtCustomCommandConfiguration::wheelDownLineEditChanged);
}

LXQtCustomCommandConfiguration::~LXQtCustomCommandConfiguration()
{
    delete ui;
}

void LXQtCustomCommandConfiguration::buttonBoxClicked(QAbstractButton *btn)
{
    QDialogButtonBox *box = qobject_cast<QDialogButtonBox*>(btn->parent());
    if (box && box->buttonRole(btn) == QDialogButtonBox::ResetRole) {
        setUiValues();
        fontChanged(mFont);
    }
    else
        close();
}

void LXQtCustomCommandConfiguration::loadSettings()
{
    mAutoRotate = settings().value(QStringLiteral("autoRotate"), true).toBool();
    mFont = settings().value(QStringLiteral("font"), font().toString()).toString();
    mCommand = settings().value(QStringLiteral("command"), QStringLiteral("echo Configure...")).toString();
    mRunWithBash = settings().value(QStringLiteral("runWithBash"), true).toBool();
    mRepeat = settings().value(QStringLiteral("repeat"), true).toBool();
    mRepeatTimer = settings().value(QStringLiteral("repeatTimer"), 5).toInt();
    mIcon = settings().value(QStringLiteral("icon"), QString()).toString();
    mText = settings().value(QStringLiteral("text"), QStringLiteral("%1")).toString();
    mMaxWidth = settings().value(QStringLiteral("maxWidth"), 200).toInt();
    mClick = settings().value(QStringLiteral("click"), QString()).toString();
    mWheelUp = settings().value(QStringLiteral("wheelUp"), QString()).toString();
    mWheelDown = settings().value(QStringLiteral("wheelDown"), QString()).toString();
}

void LXQtCustomCommandConfiguration::setUiValues()
{
    ui->autoRotateCheckBox->setChecked(mAutoRotate);
    ui->fontButton->setText(mFont);
    ui->commandPlainTextEdit->setPlainText(mCommand);
    ui->runWithBashCheckBox->setChecked(mRunWithBash);
    ui->repeatCheckBox->setChecked(mRepeat);
    ui->repeatTimerSpinBox->setEnabled(mRepeat);
    ui->repeatTimerSpinBox->setValue(mRepeatTimer);
    ui->iconLineEdit->setText(mIcon);
    ui->textLineEdit->setText(mText);
    ui->maxWidthSpinBox->setValue(mMaxWidth);
    ui->clickLineEdit->setText(mClick);
    ui->wheelUpLineEdit->setText(mWheelUp);
    ui->wheelDownLineEdit->setText(mWheelDown);
}

void LXQtCustomCommandConfiguration::autoRotateChanged(bool autoRotate)
{
    settings().setValue(QStringLiteral("autoRotate"), autoRotate);
}

void LXQtCustomCommandConfiguration::fontButtonClicked()
{
    bool ok;
    QFont currentFont;
    currentFont.fromString(ui->fontButton->text());
    QFont getFont = QFontDialog::getFont(&ok, currentFont, this);
    if (ok)
        fontChanged(getFont.toString());
}

void LXQtCustomCommandConfiguration::fontChanged(QString fontString)
{
    ui->fontButton->setText(fontString);
    settings().setValue(QStringLiteral("font"), fontString);
}

void LXQtCustomCommandConfiguration::commandPlainTextEditChanged()
{
    settings().setValue(QStringLiteral("command"), ui->commandPlainTextEdit->toPlainText());
}

void LXQtCustomCommandConfiguration::runWithBashCheckBoxChanged(bool runWithBash)
{
    settings().setValue(QStringLiteral("runWithBash"), runWithBash);
}

void LXQtCustomCommandConfiguration::repeatCheckBoxChanged(bool repeat)
{
    settings().setValue(QStringLiteral("repeat"), repeat);
    ui->repeatTimerSpinBox->setEnabled(repeat);
}

void LXQtCustomCommandConfiguration::repeatTimerSpinBoxChanged(int repeatTimer)
{
    settings().setValue(QStringLiteral("repeatTimer"), repeatTimer);
}

void LXQtCustomCommandConfiguration::iconLineEditChanged(QString icon)
{
    settings().setValue(QStringLiteral("icon"), icon);
}

void LXQtCustomCommandConfiguration::iconBrowseButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Icon File"), QString(), tr("Images (*.png *.svg *.xpm *.jpg)"));
    ui->iconLineEdit->setText(fileName);
}

void LXQtCustomCommandConfiguration::textLineEditChanged(QString text)
{
    settings().setValue(QStringLiteral("text"), text);
}

void LXQtCustomCommandConfiguration::maxWidthSpinBoxChanged(int maxWidth)
{
    settings().setValue(QStringLiteral("maxWidth"), maxWidth);
}

void LXQtCustomCommandConfiguration::clickLineEditChanged(QString click)
{
    settings().setValue(QStringLiteral("click"), click);
}

void LXQtCustomCommandConfiguration::wheelUpLineEditChanged(QString wheelUp)
{
    settings().setValue(QStringLiteral("wheelUp"), wheelUp);
}

void LXQtCustomCommandConfiguration::wheelDownLineEditChanged(QString wheelDown)
{
    settings().setValue(QStringLiteral("wheelDown"), wheelDown);
}
