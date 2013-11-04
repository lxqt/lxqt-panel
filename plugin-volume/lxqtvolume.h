/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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

#ifndef LXQTVOLUME_H
#define LXQTVOLUME_H

#include "../panel/ilxqtpanelplugin.h"
#include <QtGui/QToolButton>

#include <QtGui/QSlider>

class VolumeButton;
class AudioEngine;
class AudioDevice;
namespace LxQt {
class Notification;
}
namespace GlobalKeyShortcut
{
class Action;
}

class LxQtVolume : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    LxQtVolume(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtVolume();

    virtual QWidget *widget();
    virtual QString themeId() const { return "Volume"; }
    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
    void realign();
    QDialog *configureDialog();

    void setAudioEngine(AudioEngine *engine);
protected slots:
    virtual void settingsChanged();
    void updateConfigurationSinkList();
    void handleShortcutVolumeUp();
    void handleShortcutVolumeDown();
    void handleShortcutVolumeMute();

private:
    AudioEngine *m_engine;
    VolumeButton *m_volumeButton;
    int m_defaultSinkIndex;
    AudioDevice *m_defaultSink;
    GlobalKeyShortcut::Action *m_keyVolumeUp;
    GlobalKeyShortcut::Action *m_keyVolumeDown;
    GlobalKeyShortcut::Action *m_keyMuteToggle;
    LxQt::Notification *m_notification;
};


class LxQtVolumePluginLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo)
    {
        return new LxQtVolume(startupInfo);
    }
};

#endif // LXQTVOLUME_H
