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

#ifndef VOLUMEPOPUP_H
#define VOLUMEPOPUP_H

#include <QDialog>
#include <QList>

class QSlider;
class QPushButton;
class QScrollArea;
class QWheelEvent;
class AudioDevice;

struct SinkRow {
    AudioDevice *device = nullptr;
    QWidget *rowWidget = nullptr;
    QSlider *slider = nullptr;
    QPushButton *muteButton = nullptr;
    QPushButton *defaultButton = nullptr;
};

class VolumePopup : public QDialog
{
    Q_OBJECT
public:
    VolumePopup(QWidget* parent = nullptr);

    void openAt(QPoint pos, Qt::Corner anchor);
    void handleWheelEvent(QWheelEvent *event);

    AudioDevice *device() const { return m_defaultSink; }
    void setDevice(AudioDevice *device);
    void setSinks(const QList<AudioDevice*> &sinks, AudioDevice *defaultSink);
    void setDefaultSink(AudioDevice *defaultSink);
    void setSliderStep(int step);

signals:
    void mouseEntered();
    void mouseLeft();

    void deviceChanged();
    void launchMixer();
    void stockIconChanged(const QString &iconName);
    void defaultSinkRequested(AudioDevice *device);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool event(QEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private slots:
    void handleSliderValueChanged(int value);
    void handleMuteToggleClicked();
    void handleSetDefaultClicked();
    void handleDeviceVolumeChanged(AudioDevice *device, int volume);
    void handleDeviceMuteChanged(AudioDevice *device, bool mute);
    void onDefaultSinkVolumeOrMuteChanged();

private:
    void realign();
    void updateStockIcon();
    void rebuildSinkRows();
    SinkRow makeSinkRow(AudioDevice *device);
    void updateDefaultButtons();

    QScrollArea *m_sinkScrollArea;
    QWidget *m_sinksContainer;
    QPushButton *m_mixerButton;
    QPoint m_pos;
    Qt::Corner m_anchor;
    QList<SinkRow> m_sinkRows;
    QList<AudioDevice*> m_sinks;
    AudioDevice *m_defaultSink;
    int m_sliderStep;
};

#endif // VOLUMEPOPUP_H
