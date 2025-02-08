/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Johannes Zellner <webmaster@nebulon.de>
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

#include "audioengine.h"

#include "audiodevice.h"

#include <QMetaType>
#include <QtDebug>

#include <algorithm>

AudioEngine::AudioEngine(QObject *parent) :
    QObject(parent)
{
}

AudioEngine::~AudioEngine()
{
    qDeleteAll(m_sinks);
    m_sinks.clear();
}

int AudioEngine::volumeBounded(int volume, AudioDevice* device) const
{
    int maximum = volumeMax(device);
    double v = ((double) volume / 100.0) * maximum;
    double bounded = std::clamp<double>(v, 0.0, maximum);
    return std::round((bounded / maximum) * 100);
}


void AudioEngine::mute(AudioDevice *device)
{
    setMute(device, true);
}

void AudioEngine::unmute(AudioDevice *device)
{
    setMute(device, false);
}

void AudioEngine::setIgnoreMaxVolume(bool ignore)
{
    Q_UNUSED(ignore)
}
