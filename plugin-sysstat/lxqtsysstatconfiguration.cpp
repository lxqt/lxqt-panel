/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
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


#include "lxqtsysstatconfiguration.h"
#include "ui_lxqtsysstatconfiguration.h"
#include "lxqtsysstatutils.h"
#include "lxqtsysstatcolours.h"

#include <SysStat/CpuStat>
#include <SysStat/MemStat>
#include <SysStat/NetStat>

//Note: strings can't actually be translated here (in static initialization time)
//      the QT_TR_NOOP here is just for qt translate tools to get the strings for translation
const QStringList LXQtSysStatConfiguration::msStatTypes = {
    QLatin1String(QT_TR_NOOP("CPU"))
    , QLatin1String(QT_TR_NOOP("Memory"))
    , QLatin1String(QT_TR_NOOP("Network"))
};

namespace
{
    //Note: workaround for making source strings translatable
    //  (no need to ever call this function)
    void localizationWorkaround();
    auto t = localizationWorkaround;//avoid unused function warning
    void localizationWorkaround()
    {
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu0"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu1"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu2"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu3"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu4"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu5"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu6"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu7"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu8"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu9"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu10"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu11"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu12"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu13"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu14"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu15"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu16"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu17"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu18"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu19"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu20"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu21"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu22"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu23"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "memory"));
        static_cast<void>(QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "swap"));
        static_cast<void>(t);//avoid unused variable warning
    }
}

LXQtSysStatConfiguration::LXQtSysStatConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtSysStatConfiguration),
    mStat(NULL),
    mColorsDialog(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("SysStatConfigurationWindow");
    ui->setupUi(this);

    //Note: translation is needed here in runtime (translator is attached already)
    for (auto const & type : msStatTypes)
        ui->typeCOB->addItem(tr(type.toStdString().c_str()), type);

    loadSettings();

    connect(ui->typeCOB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->intervalSB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->sizeSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->linesSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->titleLE, &QLineEdit::editingFinished, this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->useFrequencyCB, &QCheckBox::toggled, this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->maximumHS, &QSlider::valueChanged, this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->logarithmicCB, &QCheckBox::toggled, this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->sourceCOB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->useThemeColorsRB, &QRadioButton::toggled, this, &LXQtSysStatConfiguration::saveSettings);
}

LXQtSysStatConfiguration::~LXQtSysStatConfiguration()
{
    delete ui;
}

void LXQtSysStatConfiguration::loadSettings()
{
    ui->intervalSB->setValue(settings().value("graph/updateInterval", 1.0).toDouble());
    ui->sizeSB->setValue(settings().value("graph/minimalSize", 30).toInt());

    ui->linesSB->setValue(settings().value("grid/lines", 1).toInt());

    ui->titleLE->setText(settings().value("title/label", QString()).toString());

    int typeIndex = ui->typeCOB->findData(settings().value("data/type", msStatTypes[0]));
    ui->typeCOB->setCurrentIndex((typeIndex >= 0) ? typeIndex : 0);
    on_typeCOB_currentIndexChanged(ui->typeCOB->currentIndex());

    int sourceIndex = ui->sourceCOB->findData(settings().value("data/source", QString()));
    ui->sourceCOB->setCurrentIndex((sourceIndex >= 0) ? sourceIndex : 0);

    ui->useFrequencyCB->setChecked(settings().value("cpu/useFrequency", true).toBool());
    ui->maximumHS->setValue(PluginSysStat::netSpeedFromString(settings().value("net/maximumSpeed", "1 MB/s").toString()));
    on_maximumHS_valueChanged(ui->maximumHS->value());
    ui->logarithmicCB->setChecked(settings().value("net/logarithmicScale", true).toBool());
    ui->logScaleSB->setValue(settings().value("net/logarithmicScaleSteps", 4).toInt());

    bool useThemeColors = settings().value("graph/useThemeColors", true).toBool();
    ui->useThemeColorsRB->setChecked(useThemeColors);
    ui->useCustomColorsRB->setChecked(!useThemeColors);
    ui->customColorsB->setEnabled(!useThemeColors);
}

void LXQtSysStatConfiguration::saveSettings()
{
    settings().setValue("graph/useThemeColors", ui->useThemeColorsRB->isChecked());
    settings().setValue("graph/updateInterval", ui->intervalSB->value());
    settings().setValue("graph/minimalSize", ui->sizeSB->value());

    settings().setValue("grid/lines", ui->linesSB->value());

    settings().setValue("title/label", ui->titleLE->text());

    //Note:
    // need to make a realy deep copy of the msStatTypes[x] because of SEGFAULTs
    // occuring in static finalization time (don't know the real reason...maybe ordering of static finalizers/destructors)
    QString type = ui->typeCOB->itemData(ui->typeCOB->currentIndex(), Qt::UserRole).toString().toStdString().c_str();
    settings().setValue("data/type", type);
    settings().setValue("data/source", ui->sourceCOB->itemData(ui->sourceCOB->currentIndex(), Qt::UserRole));

    settings().setValue("cpu/useFrequency", ui->useFrequencyCB->isChecked());

    settings().setValue("net/maximumSpeed", PluginSysStat::netSpeedToString(ui->maximumHS->value()));
    settings().setValue("net/logarithmicScale", ui->logarithmicCB->isChecked());
    settings().setValue("net/logarithmicScaleSteps", ui->logScaleSB->value());
}

void LXQtSysStatConfiguration::on_typeCOB_currentIndexChanged(int index)
{
    if (mStat)
        mStat->deleteLater();
    switch (index)
    {
    case 0:
        mStat = new SysStat::CpuStat(this);
        break;

    case 1:
        mStat = new SysStat::MemStat(this);
        break;

    case 2:
        mStat = new SysStat::NetStat(this);
        break;
    }

    ui->sourceCOB->blockSignals(true);
    ui->sourceCOB->clear();
    const auto sources = mStat->sources();
    for (auto const & s : sources)
        ui->sourceCOB->addItem(tr(s.toStdString().c_str()), s);
    ui->sourceCOB->blockSignals(false);
    ui->sourceCOB->setCurrentIndex(0);
}

void LXQtSysStatConfiguration::on_maximumHS_valueChanged(int value)
{
    ui->maximumValueL->setText(PluginSysStat::netSpeedToString(value));
}

void LXQtSysStatConfiguration::coloursChanged()
{
    const LXQtSysStatColors::Colors &colours = mColorsDialog->colours();

    settings().setValue("grid/colour",  colours["grid"].name());
    settings().setValue("title/colour", colours["title"].name());

    settings().setValue("cpu/systemColor",    colours["cpuSystem"].name());
    settings().setValue("cpu/userColor",      colours["cpuUser"].name());
    settings().setValue("cpu/niceColor",      colours["cpuNice"].name());
    settings().setValue("cpu/otherColor",     colours["cpuOther"].name());
    settings().setValue("cpu/frequencyColor", colours["cpuFrequency"].name());

    settings().setValue("mem/appsColor",    colours["memApps"].name());
    settings().setValue("mem/buffersColor", colours["memBuffers"].name());
    settings().setValue("mem/cachedColor",  colours["memCached"].name());
    settings().setValue("mem/swapColor",    colours["memSwap"].name());

    settings().setValue("net/receivedColor",    colours["netReceived"].name());
    settings().setValue("net/transmittedColor", colours["netTransmitted"].name());
}

void LXQtSysStatConfiguration::on_customColorsB_clicked()
{
    if (!mColorsDialog)
    {
        mColorsDialog = new LXQtSysStatColors(this);
        connect(mColorsDialog, SIGNAL(coloursChanged()), SLOT(coloursChanged()));
    }

    LXQtSysStatColors::Colors colours;

    const LXQtSysStatColors::Colors &defaultColors = mColorsDialog->defaultColors();

    colours["grid"]  = QColor(settings().value("grid/colour",  defaultColors["grid"] .name()).toString());
    colours["title"] = QColor(settings().value("title/colour", defaultColors["title"].name()).toString());

    colours["cpuSystem"]    = QColor(settings().value("cpu/systemColor",    defaultColors["cpuSystem"]   .name()).toString());
    colours["cpuUser"]      = QColor(settings().value("cpu/userColor",      defaultColors["cpuUser"]     .name()).toString());
    colours["cpuNice"]      = QColor(settings().value("cpu/niceColor",      defaultColors["cpuNice"]     .name()).toString());
    colours["cpuOther"]     = QColor(settings().value("cpu/otherColor",     defaultColors["cpuOther"]    .name()).toString());
    colours["cpuFrequency"] = QColor(settings().value("cpu/frequencyColor", defaultColors["cpuFrequency"].name()).toString());

    colours["memApps"]    = QColor(settings().value("mem/appsColor",    defaultColors["memApps"]   .name()).toString());
    colours["memBuffers"] = QColor(settings().value("mem/buffersColor", defaultColors["memBuffers"].name()).toString());
    colours["memCached"]  = QColor(settings().value("mem/cachedColor",  defaultColors["memCached"] .name()).toString());
    colours["memSwap"]    = QColor(settings().value("mem/swapColor",    defaultColors["memSwap"]   .name()).toString());

    colours["netReceived"]    = QColor(settings().value("net/receivedColor",    defaultColors["netReceived"]   .name()).toString());
    colours["netTransmitted"] = QColor(settings().value("net/transmittedColor", defaultColors["netTransmitted"].name()).toString());

    mColorsDialog->setColors(colours);

    mColorsDialog->exec();
}
