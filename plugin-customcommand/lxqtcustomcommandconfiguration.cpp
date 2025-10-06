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
#include "lxqtcustomcommand.h"
#include "ui_lxqtcustomcommandconfiguration.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QFontDatabase>
#include <QWhatsThis>
#include <qnamespace.h>

#define HELP_TEXT_DESC tr("command outputs plain text to be used as text of the button")
#define HELP_ICON_DESC1 tr("command outputs icon in form of:")
#define HELP_ICON_DESC2 tr("theme name")
#define HELP_ICON_DESC3 tr("name of icon resolved to image based on XDG spec")
#define HELP_ICON_DESC4 tr("image file path")
#define HELP_ICON_DESC5 tr("image data stream")
#define HELP_ICON_DESC6 tr("plain image stream")
#define HELP_ICON_DESC7 tr("base64 encoded image data stream")
#define HELP_ICON_DESC8 tr("as above but base64 encoded")
#define HELP_STRUC_DESC1 tr("command outputs structured variables to be used for button visualization in form")
#define HELP_STRUC_DESC2 tr("name1:base64value1 name2:base64value2  ...")
#define HELP_STRUC_DESC3 tr("Handled names are:")
#define HELP_STRUC_DESC4 tr("string to be used as text of the button")
#define HELP_STRUC_DESC5 tr("icon to be shown in the button, in the same form as in")
#define HELP_STRUC_DESC6 tr("output")
#define HELP_STRUC_DESC7 tr("string to be used as tooltip of the button")
#define HELP_STRUC_DESC8 tr("All values should be encoded in base64")
#define HELP_STRUC_DESC9 tr("Example of script generating structured output:")
#define HELP_STRUC_EXAMPLE1 tr("My Text")
#define HELP_STRUC_EXAMPLE2 tr("my_image")
#define HELP_STRUC_EXAMPLE3 tr("My Tooltip")



//Note: strings can't actually be translated here (in static initialization time)
//      the QT_TR_NOOP here is just for qt translate tools to get the strings for translation
const QStringList LXQtCustomCommandConfiguration::msOutputFormatStrings = {
    QStringLiteral(QT_TR_NOOP("Text only"))
    , QStringLiteral(QT_TR_NOOP("Icon only"))
    , QStringLiteral(QT_TR_NOOP("Structured"))
};

LXQtCustomCommandConfiguration::LXQtCustomCommandConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtCustomCommandConfiguration),
    mLockSettingChanges(false)
{
    assert(msOutputFormatStrings.size() == OUTPUT_END);

    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    const QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->commandPlainTextEdit->setFont(monoFont);

    mHelpText = QStringLiteral("<html><head/><body><p><span style=\" font-weight:700;\">%1</span> - %2</p>"
    "<p><span style=\" font-weight:700;\">%3</span> - %4<br/>"
    "- <span style=\" font-style:italic;\">%5</span> - %6<br/>"
    "- <span style=\" font-style:italic;\">%7</span> - %7<br/>"
    "- <span style=\" font-style:italic;\">%8</span> - %9<br/>"
    "- <span style=\" font-style:italic;\">%10</span> - %11</p>"
    "<p><span style=\" font-weight:700;\">%12</span> - %13 \"<span style=\" text-decoration: underline;\">%14</span>\".<br/>"
    "%15<br/>"
    "- <span style=\" font-style:italic;\">text</span> - %16<br/>"
    "- <span style=\" font-style:italic;\">icon</span> - %17 <span style=\" font-weight:700;\">%3</span> %18<br/>"
    "- <span style=\" font-style:italic;\">tooltip</span> - %19<br/>"
    "<span style=\" text-decoration: underline;\">%20</span><br/><br/>"
    "%21<br/>"
    "<span style=\" font-style:italic;\">echo \"text:$(echo -n \"%22\" | base64 --wrap=0) icon:$(base64 --wrap=0 %23.svg) tooltip:$(echo -n \"%24\" | base64 --wrap=0)\"</span></p></body></html>")
    .arg(msOutputFormatStrings[OUTPUT_TEXT],
        HELP_TEXT_DESC,
        msOutputFormatStrings[OUTPUT_ICON],
        HELP_ICON_DESC1,
        HELP_ICON_DESC2,
        HELP_ICON_DESC3,
        HELP_ICON_DESC4,
        HELP_ICON_DESC5,
        HELP_ICON_DESC6,
        HELP_ICON_DESC7,
        HELP_ICON_DESC8,
        msOutputFormatStrings[OUTPUT_STRUCTURED],
        HELP_STRUC_DESC1,
        HELP_STRUC_DESC2,
        HELP_STRUC_DESC3,
        HELP_STRUC_DESC4,
        HELP_STRUC_DESC5,
        HELP_STRUC_DESC6,
        HELP_STRUC_DESC7,
        HELP_STRUC_DESC8,
        HELP_STRUC_DESC9,
        HELP_STRUC_EXAMPLE1,
        HELP_STRUC_EXAMPLE2,
        HELP_STRUC_EXAMPLE3
    );

    //Note: translation is needed here in runtime (translator is attached already)
    for (int format = OUTPUT_BEGIN; format < OUTPUT_END; ++format)
        ui->outputFormatComboBox->addItem(tr(msOutputFormatStrings[format].toStdString().c_str()), format);

    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &LXQtCustomCommandConfiguration::dialogButtonsAction);

    connect(ui->autoRotateCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::autoRotateChanged);
    connect(ui->fontButton, &QPushButton::clicked, this, &LXQtCustomCommandConfiguration::fontButtonClicked);
    connect(ui->textColorLabel, &ColorLabel::colorChanged, this, &LXQtCustomCommandConfiguration::textColorChanged);
    connect(ui->textColorResetButton, &QPushButton::clicked, this, &LXQtCustomCommandConfiguration::textColorResetButtonClicked);
    connect(ui->commandPlainTextEdit, &QPlainTextEdit::textChanged, this, &LXQtCustomCommandConfiguration::commandPlainTextEditChanged);
    connect(ui->runWithBashCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::runWithBashCheckBoxChanged);
    connect(ui->outputFormatComboBox, &QComboBox::currentIndexChanged, this, &LXQtCustomCommandConfiguration::outputFormatComboBoxChanged);
    connect(ui->continuousOutputCheckBox, &QCheckBox::toggled, this, &LXQtCustomCommandConfiguration::continuousOutputCheckBoxChanged);
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
    connect(ui->helpToolButton, &QToolButton::clicked, this, [this] () {
        QWhatsThis::showText(QCursor::pos(), mHelpText, this);
    });
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
    ui->textColorLabel->setColor(QColor::fromString(settings().value(QStringLiteral("textColor")).toString()));
    ui->commandPlainTextEdit->setPlainText(settings().value(QStringLiteral("command"), QStringLiteral("echo Configure...")).toString());
    ui->runWithBashCheckBox->setChecked(settings().value(QStringLiteral("runWithBash"), true).toBool());
    // backward compatibility check
    if (settings().contains(QStringLiteral("outputFormat")))
        ui->outputFormatComboBox->setCurrentIndex(ui->outputFormatComboBox->findData(settings().value(QStringLiteral("outputFormat")).toInt()));
    else {
        const bool image = settings().value(QStringLiteral("outputImage"), false).toBool();
        ui->outputFormatComboBox->setCurrentIndex(ui->outputFormatComboBox->findData(image ? OUTPUT_ICON : OUTPUT_TEXT));
    }
    ui->continuousOutputCheckBox->setChecked(settings().value(QStringLiteral("continuousOutput"), false).toBool());
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

void LXQtCustomCommandConfiguration::textColorChanged()
{
    QColor color = ui->textColorLabel->getColor();
    QColor oldColor = QColor::fromString(settings().value(QStringLiteral("textColor")).toString());
    if (color != oldColor)
        settings().setValue(QStringLiteral("textColor"), color.name());
}

void LXQtCustomCommandConfiguration::textColorResetButtonClicked()
{
    ui->textColorLabel->reset();
    settings().remove(QStringLiteral("textColor"));
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

void LXQtCustomCommandConfiguration::outputFormatComboBoxChanged(int index)
{
    if (!mLockSettingChanges) {
        settings().setValue(QStringLiteral("outputFormat"), ui->outputFormatComboBox->itemData(index, Qt::UserRole));
        settings().remove(QStringLiteral("outputImage"));
    }
}

void LXQtCustomCommandConfiguration::continuousOutputCheckBoxChanged(bool continuousOutput)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("continuousOutput"), continuousOutput);
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
