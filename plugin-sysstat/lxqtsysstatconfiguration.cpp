/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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
    QStringLiteral(QT_TR_NOOP("CPU"))
    , QStringLiteral(QT_TR_NOOP("Memory"))
    , QStringLiteral(QT_TR_NOOP("Network"))
};

namespace
{
    //Note: workaround for making source strings translatable
    //  (no need to ever call this function)
    void localizationWorkaround();
    auto t = localizationWorkaround;//avoid unused function warning
    void localizationWorkaround()
    {
        const char * loc;
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu1");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu2");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu3");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu4");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu5");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu6");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu7");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu8");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu9");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu11");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu12");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu13");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu14");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu15");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu16");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu17");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu18");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu19");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu20");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu21");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu22");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu23");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "cpu24");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "memory");
        loc = QT_TRANSLATE_NOOP("LXQtSysStatConfiguration", "swap");
        static_cast<void>(t);//avoid unused variable warning
    }
}

LXQtSysStatConfiguration::LXQtSysStatConfiguration(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LXQtSysStatConfiguration),
    mSettings(settings),
    oldSettings(settings),
    mStat(NULL),
    mColoursDialog(NULL)
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
    connect(ui->logarithmicCB, &QCheckBox::toggled, this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->sourceCOB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &LXQtSysStatConfiguration::saveSettings);
    connect(ui->useThemeColoursRB, &QRadioButton::toggled, this, &LXQtSysStatConfiguration::saveSettings);
}

LXQtSysStatConfiguration::~LXQtSysStatConfiguration()
{
    delete ui;
}

void LXQtSysStatConfiguration::loadSettings()
{
    ui->intervalSB->setValue(mSettings->value("graph/updateInterval", 1.0).toDouble());
    ui->sizeSB->setValue(mSettings->value("graph/minimalSize", 30).toInt());

    ui->linesSB->setValue(mSettings->value("grid/lines", 1).toInt());

    ui->titleLE->setText(mSettings->value("title/label", QString()).toString());

    int typeIndex = ui->typeCOB->findData(mSettings->value("data/type", msStatTypes[0]));
    ui->typeCOB->setCurrentIndex((typeIndex >= 0) ? typeIndex : 0);
    on_typeCOB_currentIndexChanged(ui->typeCOB->currentIndex());

    int sourceIndex = ui->sourceCOB->findData(mSettings->value("data/source", QString()));
    ui->sourceCOB->setCurrentIndex((sourceIndex >= 0) ? sourceIndex : 0);

    ui->useFrequencyCB->setChecked(mSettings->value("cpu/useFrequency", true).toBool());
    ui->maximumHS->setValue(PluginSysStat::netSpeedFromString(mSettings->value("net/maximumSpeed", "1 MB/s").toString()));
    on_maximumHS_valueChanged(ui->maximumHS->value());
    ui->logarithmicCB->setChecked(mSettings->value("net/logarithmicScale", true).toBool());
    ui->logScaleSB->setValue(mSettings->value("net/logarithmicScaleSteps", 4).toInt());

    bool useThemeColours = mSettings->value("graph/useThemeColours", true).toBool();
    ui->useThemeColoursRB->setChecked(useThemeColours);
    ui->useCustomColoursRB->setChecked(!useThemeColours);
    ui->customColoursB->setEnabled(!useThemeColours);
}

void LXQtSysStatConfiguration::saveSettings()
{
    mSettings->setValue("graph/useThemeColours", ui->useThemeColoursRB->isChecked());
    mSettings->setValue("graph/updateInterval", ui->intervalSB->value());
    mSettings->setValue("graph/minimalSize", ui->sizeSB->value());

    mSettings->setValue("grid/lines", ui->linesSB->value());

    mSettings->setValue("title/label", ui->titleLE->text());

    //Note:
    // need to make a realy deep copy of the msStatTypes[x] because of SEGFAULTs
    // occuring in static finalization time (don't know the real reason...maybe ordering of static finalizers/destructors)
    QString type = ui->typeCOB->itemData(ui->typeCOB->currentIndex(), Qt::UserRole).toString().toStdString().c_str();
    mSettings->setValue("data/type", type);
    mSettings->setValue("data/source", ui->sourceCOB->itemData(ui->sourceCOB->currentIndex(), Qt::UserRole));

    mSettings->setValue("cpu/useFrequency", ui->useFrequencyCB->isChecked());

    mSettings->setValue("net/maximumSpeed", PluginSysStat::netSpeedToString(ui->maximumHS->value()));
    mSettings->setValue("net/logarithmicScale", ui->logarithmicCB->isChecked());
    mSettings->setValue("net/logarithmicScaleSteps", ui->logScaleSB->value());
}

void LXQtSysStatConfiguration::on_buttons_clicked(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        oldSettings.loadToSettings();
        loadSettings();
    }
    else
        close();
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
    for (auto const & s : mStat->sources())
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
    const LXQtSysStatColours::Colours &colours = mColoursDialog->colours();

    mSettings->setValue("grid/colour",  colours["grid"].name());
    mSettings->setValue("title/colour", colours["title"].name());

    mSettings->setValue("cpu/systemColour",    colours["cpuSystem"].name());
    mSettings->setValue("cpu/userColour",      colours["cpuUser"].name());
    mSettings->setValue("cpu/niceColour",      colours["cpuNice"].name());
    mSettings->setValue("cpu/otherColour",     colours["cpuOther"].name());
    mSettings->setValue("cpu/frequencyColour", colours["cpuFrequency"].name());

    mSettings->setValue("mem/appsColour",    colours["memApps"].name());
    mSettings->setValue("mem/buffersColour", colours["memBuffers"].name());
    mSettings->setValue("mem/cachedColour",  colours["memCached"].name());
    mSettings->setValue("mem/swapColour",    colours["memSwap"].name());

    mSettings->setValue("net/receivedColour",    colours["netReceived"].name());
    mSettings->setValue("net/transmittedColour", colours["netTransmitted"].name());
}

void LXQtSysStatConfiguration::on_customColoursB_clicked()
{
    if (!mColoursDialog)
    {
        mColoursDialog = new LXQtSysStatColours(this);
        connect(mColoursDialog, SIGNAL(coloursChanged()), SLOT(coloursChanged()));
    }

    LXQtSysStatColours::Colours colours;

    const LXQtSysStatColours::Colours &defaultColours = mColoursDialog->defaultColours();

    colours["grid"]  = QColor(mSettings->value("grid/colour",  defaultColours["grid"] .name()).toString());
    colours["title"] = QColor(mSettings->value("title/colour", defaultColours["title"].name()).toString());

    colours["cpuSystem"]    = QColor(mSettings->value("cpu/systemColour",    defaultColours["cpuSystem"]   .name()).toString());
    colours["cpuUser"]      = QColor(mSettings->value("cpu/userColour",      defaultColours["cpuUser"]     .name()).toString());
    colours["cpuNice"]      = QColor(mSettings->value("cpu/niceColour",      defaultColours["cpuNice"]     .name()).toString());
    colours["cpuOther"]     = QColor(mSettings->value("cpu/otherColour",     defaultColours["cpuOther"]    .name()).toString());
    colours["cpuFrequency"] = QColor(mSettings->value("cpu/frequencyColour", defaultColours["cpuFrequency"].name()).toString());

    colours["memApps"]    = QColor(mSettings->value("mem/appsColour",    defaultColours["memApps"]   .name()).toString());
    colours["memBuffers"] = QColor(mSettings->value("mem/buffersColour", defaultColours["memBuffers"].name()).toString());
    colours["memCached"]  = QColor(mSettings->value("mem/cachedColour",  defaultColours["memCached"] .name()).toString());
    colours["memSwap"]    = QColor(mSettings->value("mem/swapColour",    defaultColours["memSwap"]   .name()).toString());

    colours["netReceived"]    = QColor(mSettings->value("net/receivedColour",    defaultColours["netReceived"]   .name()).toString());
    colours["netTransmitted"] = QColor(mSettings->value("net/transmittedColour", defaultColours["netTransmitted"].name()).toString());

    mColoursDialog->setColours(colours);

    mColoursDialog->exec();
}
