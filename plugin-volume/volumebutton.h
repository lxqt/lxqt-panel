/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2012      Razor team
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

#ifndef VOLUMEBUTTON_H
#define VOLUMEBUTTON_H

#include <QToolButton>
#include <QTimer>

class VolumePopup;
class ILxQtPanel;
class LxQtVolume;
class ILxQtPanelPlugin;

class VolumeButton : public QToolButton
{
    Q_OBJECT
public:
    VolumeButton(ILxQtPanelPlugin *plugin, QWidget* parent = 0);
    ~VolumeButton();

    void setShowOnClicked(bool state);
    void setMuteOnMiddleClick(bool state);
    void setMixerCommand(const QString &command);

    VolumePopup *volumePopup() const { return m_volumePopup; }

public slots:
    void hideVolumeSlider();
    void showVolumeSlider();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void toggleVolumeSlider();
    void handleMixerLaunch();
    void handleStockIconChanged(const QString &iconName);

private:
    VolumePopup *m_volumePopup;
    ILxQtPanelPlugin *mPlugin;
    ILxQtPanel *m_panel;
    QTimer m_popupHideTimer;
    bool m_showOnClick;
    bool m_muteOnMiddleClick;
    QString m_mixerCommand;
};

#endif // VOLUMEBUTTON_H
