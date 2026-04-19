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

#ifndef PULSEAUDIOENGINE_H
#define PULSEAUDIOENGINE_H

#include "audioengine.h"

#include <QObject>
#include <QList>
#include <QTimer>
#include <QMap>

#include <pulse/pulseaudio.h>

class AudioDevice;

class PulseAudioEngine : public AudioEngine
{
    Q_OBJECT

public:
    PulseAudioEngine(QObject *parent = nullptr);
    ~PulseAudioEngine();

    const QString backendName() const override { return QLatin1String("PulseAudio"); }

    int volumeMax(AudioDevice */*device*/) const override { return m_maximumVolume; }

    bool setDefaultSink(AudioDevice *device) override;

    struct PulseAudioDevice
    {
        PulseAudioDevice(const pa_sink_info&);

        QString name;
        QString description;
        uint32_t index;
        int mute;
        pa_cvolume volume;
    };

public slots:
    void commitDeviceVolume(AudioDevice *device) override;
    void retrieveSinkInfo(uint32_t idx);
    void setMute(AudioDevice *device, bool state) override;
    void setContextState(pa_context_state_t state);
    void setIgnoreMaxVolume(bool ignore) override;

signals:
    void sinkInfoChanged(uint32_t idx);
    void contextStateChanged(pa_context_state_t state);
    void readyChanged(bool ready);
    void sinkRemoved(uint32_t);
    void sinkInfoReceived(const PulseAudioDevice&);

private slots:
    void handleContextStateChanged();
    void connectContext();
    void removeSink(uint32_t idx);
    void addOrUpdateSink(const PulseAudioDevice&);

private:
    static void sinkInfoCallback(pa_context *, const pa_sink_info *, int, void *);
    static void contextStateCallback(pa_context *, void *);
    static void contextSuccessCallback(pa_context *, int, void *);
    static void contextSubscriptionCallback(pa_context *,
                                            pa_subscription_event_type_t,
                                            uint32_t,
                                            void *);
    static void contextEventCallback(pa_context *, const char *, pa_proplist *, void *);

    void retrieveSinks();
    void setupSubscription();
    void requestSinkInfoUpdate(uint32_t idx);

    pa_mainloop_api *m_mainLoopApi;
    pa_threaded_mainloop *m_mainLoop;
    pa_context *m_context;

    pa_context_state_t m_contextState;
    bool m_ready;
    QTimer m_reconnectionTimer;
    int m_maximumVolume;

    QMap<AudioDevice *, pa_cvolume> m_cVolumeMap;
};

#endif // PULSEAUDIOENGINE_H
