/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#include "lxqtvolumeconfiguration.h"
#include "ui_lxqtvolumeconfiguration.h"

#include "audiodevice.h"

#include <QComboBox>
#include <QDebug>

LxQtVolumeConfiguration::LxQtVolumeConfiguration(QSettings &settings, QWidget *parent) :
    LxQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LxQtVolumeConfiguration)
{
    ui->setupUi(this);

    loadSettings();
    connect(ui->ossRadioButton, SIGNAL(toggled(bool)), this, SLOT(audioEngineChanged(bool)));
    connect(ui->devAddedCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(sinkSelectionChanged(int)));
    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(ui->showOnClickCheckBox, SIGNAL(toggled(bool)), this, SLOT(showOnClickedChanged(bool)));
    connect(ui->muteOnMiddleClickCheckBox, SIGNAL(toggled(bool)), this, SLOT(muteOnMiddleClickChanged(bool)));
    connect(ui->mixerLineEdit, SIGNAL(textChanged(QString)), this, SLOT(mixerLineEditChanged(QString)));
    connect(ui->stepSpinBox, SIGNAL(valueChanged(int)), this, SLOT(stepSpinBoxChanged(int)));
    connect(ui->ignoreMaxVolumeCheckBox, SIGNAL(toggled(bool)), this, SLOT(ignoreMaxVolumeCheckBoxChanged(bool)));

#ifdef USE_PULSEAUDIO
    connect(ui->pulseAudioRadioButton, SIGNAL(toggled(bool)), this, SLOT(audioEngineChanged(bool)));
#else
    ui->pulseAudioRadioButton->setVisible(false);
#endif

#ifdef USE_ALSA
    connect(ui->alsaRadioButton, SIGNAL(toggled(bool)), this, SLOT(audioEngineChanged(bool)));
#else
    ui->alsaRadioButton->setVisible(false);
#endif
}

LxQtVolumeConfiguration::~LxQtVolumeConfiguration()
{
    delete ui;
}

void LxQtVolumeConfiguration::setSinkList(const QList<AudioDevice *> sinks)
{
    // preserve the current index, as we change the list
    int tmp_index = settings().value(SETTINGS_DEVICE, SETTINGS_DEFAULT_DEVICE).toInt();

    ui->devAddedCombo->clear();

    foreach (const AudioDevice *dev, sinks) {
        ui->devAddedCombo->addItem(dev->description(), dev->index());
    }

    ui->devAddedCombo->setCurrentIndex(tmp_index);
}

void LxQtVolumeConfiguration::audioEngineChanged(bool checked)
{
    if (!checked)
        return;

    if (ui->pulseAudioRadioButton->isChecked())
        settings().setValue(SETTINGS_AUDIO_ENGINE, "PulseAudio");
    else if(ui->alsaRadioButton->isChecked())
        settings().setValue(SETTINGS_AUDIO_ENGINE, "Alsa");
    else
        settings().setValue(SETTINGS_AUDIO_ENGINE, "Oss");
}

void LxQtVolumeConfiguration::sinkSelectionChanged(int index)
{
    settings().setValue(SETTINGS_DEVICE, index >= 0 ? index : 0);
}

void LxQtVolumeConfiguration::showOnClickedChanged(bool state)
{
    settings().setValue(SETTINGS_SHOW_ON_LEFTCLICK, state);
}

void LxQtVolumeConfiguration::muteOnMiddleClickChanged(bool state)
{
    settings().setValue(SETTINGS_MUTE_ON_MIDDLECLICK, state);
}

void LxQtVolumeConfiguration::mixerLineEditChanged(const QString &command)
{
    settings().setValue(SETTINGS_MIXER_COMMAND, command);
}

void LxQtVolumeConfiguration::stepSpinBoxChanged(int step)
{
    settings().setValue(SETTINGS_STEP, step);
}

void LxQtVolumeConfiguration::ignoreMaxVolumeCheckBoxChanged(bool state)
{
    settings().setValue(SETTINGS_IGNORE_MAX_VOLUME, state);
}

void LxQtVolumeConfiguration::loadSettings()
{
    QString engine = settings().value(SETTINGS_AUDIO_ENGINE, SETTINGS_DEFAULT_AUDIO_ENGINE).toString().toLower();
    if (engine == "pulseaudio")
        ui->pulseAudioRadioButton->setChecked(true);
    else if (engine == "alsa")
        ui->alsaRadioButton->setChecked(true);
    else
        ui->ossRadioButton->setChecked(true);

    setComboboxIndexByData(ui->devAddedCombo, settings().value(SETTINGS_DEVICE, SETTINGS_DEFAULT_DEVICE), 1);
    ui->showOnClickCheckBox->setChecked(settings().value(SETTINGS_SHOW_ON_LEFTCLICK, SETTINGS_DEFAULT_SHOW_ON_LEFTCLICK).toBool());
    ui->muteOnMiddleClickCheckBox->setChecked(settings().value(SETTINGS_MUTE_ON_MIDDLECLICK, SETTINGS_DEFAULT_MUTE_ON_MIDDLECLICK).toBool());
    ui->mixerLineEdit->setText(settings().value(SETTINGS_MIXER_COMMAND, SETTINGS_DEFAULT_MIXER_COMMAND).toString());
    ui->stepSpinBox->setValue(settings().value(SETTINGS_STEP, SETTINGS_DEFAULT_STEP).toInt());
    ui->ignoreMaxVolumeCheckBox->setChecked(settings().value(SETTINGS_IGNORE_MAX_VOLUME, SETTINGS_DEFAULT_IGNORE_MAX_VOLUME).toBool());
}

