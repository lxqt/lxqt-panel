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

#include "volumebutton.h"

#include "volumepopup.h"
#include "audiodevice.h"

#include <QGuiApplication>
#include <QSlider>
#include <QMouseEvent>
#include <QProcess>
#include <QToolTip>

#include <XdgIcon>
#include "../panel/ilxqtpanel.h"
#include "../panel/ilxqtpanelplugin.h"

VolumeButton::VolumeButton(ILXQtPanelPlugin *plugin, QWidget* parent):
        QToolButton(parent),
        mPlugin(plugin),
        m_muteOnMiddleClick(true)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoRaise(true);
    setMouseTracking(true);
    // initial icon for button. It will be replaced after devices scan.
    // In the worst case - no soundcard/pulse - is found it remains
    // in the button but at least the button is not blank ("invisible")
    handleStockIconChanged(QStringLiteral("dialog-error"));
    m_volumePopup = new VolumePopup(this);

    m_popupHideTimer.setInterval(1000);
    connect(this,              &VolumeButton::clicked, this, &VolumeButton::toggleVolumeSlider);
    connect(&m_popupHideTimer, &QTimer::timeout,       this, &VolumeButton::hideVolumeSlider);

    connect(m_volumePopup, &VolumePopup::mouseEntered, &m_popupHideTimer, &QTimer::stop);
    connect(m_volumePopup, &VolumePopup::mouseLeft,    this, [this] { m_popupHideTimer.start(); } );

    connect(m_volumePopup, &VolumePopup::launchMixer,      this, &VolumeButton::handleMixerLaunch);
    connect(m_volumePopup, &VolumePopup::stockIconChanged, this, &VolumeButton::handleStockIconChanged);
    connect(m_volumePopup, &VolumePopup::popupHidden,      this, [this]() {
        setDown(false);
        suppressTooltipTemporarily();
    });
}

VolumeButton::~VolumeButton() = default;

void VolumeButton::suppressTooltipTemporarily()
{
    QToolTip::hideText();
    m_suppressTooltip = true;
    QTimer::singleShot(500, this, [this]() { m_suppressTooltip = false; });
}

void VolumeButton::setMuteOnMiddleClick(bool state)
{
    m_muteOnMiddleClick = state;
}

void VolumeButton::setMixerCommand(const QString &command)
{
    m_mixerParams = QProcess::splitCommand(command);
    m_mixerCommand = m_mixerParams.empty() ? QString{} : m_mixerParams.takeFirst();
}

void VolumeButton::enterEvent(QEnterEvent *event)
{
    if (m_volumePopup->isVisible() || m_suppressTooltip)
        return;
    // show tooltip immediately on entering widget
    QToolTip::showText(event->globalPosition().toPoint(), toolTip(), this);
}

void VolumeButton::mouseMoveEvent(QMouseEvent *event)
{
    QToolButton::mouseMoveEvent(event);
    if (m_volumePopup->isVisible() || m_suppressTooltip)
        return;
    // show tooltip immediately on moving the mouse
    if (!QToolTip::isVisible()) // prevent sliding of tooltip
        QToolTip::showText(event->globalPosition().toPoint(), toolTip(), this);
}

void VolumeButton::wheelEvent(QWheelEvent *event)
{
    m_volumePopup->handleWheelEvent(event);
}

void VolumeButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton && m_muteOnMiddleClick) {
        if (m_volumePopup->device()) {
            m_volumePopup->device()->toggleMute();
            return;
        }
    }

    QToolButton::mouseReleaseEvent(event);
}

void VolumeButton::toggleVolumeSlider()
{
    if (m_volumePopup->isVisible()) {
        hideVolumeSlider();
    } else {
        showVolumeSlider();
    }
}

void VolumeButton::showVolumeSlider()
{

    if (m_volumePopup->isVisible())
        return;

    QToolTip::hideText();
    m_popupHideTimer.stop();
    m_volumePopup->updateGeometry();
    m_volumePopup->adjustSize();
    QRect pos = mPlugin->calculatePopupWindowPos(m_volumePopup->size());
    const QPoint buttonCenter = mapToGlobal(rect().center());
    const auto panelPosition = mPlugin->panel()->position();
    if (panelPosition == ILXQtPanel::PositionLeft || panelPosition == ILXQtPanel::PositionRight)
    {
        // Vertical panel: popup is already beside the panel; center it vertically with the volume icon.
        pos.moveTop(buttonCenter.y() - pos.height() / 2);
    }
    else
    {
        // Horizontal panel (top/bottom): center the popup horizontally on the volume button.
        pos.moveLeft(buttonCenter.x() - pos.width() / 2);
    }

    // Clamp to available screen geometry so the popup is never cut off (e.g. icon at panel edge).
    QScreen *screen = QGuiApplication::screenAt(buttonCenter);
    const QRect geom = screen ? screen->availableGeometry() : QGuiApplication::primaryScreen()->availableGeometry();
    if (pos.left() < geom.left())
        pos.moveLeft(geom.left());
    if (pos.right() > geom.right())
        pos.moveRight(geom.right());
    if (pos.top() < geom.top())
        pos.moveTop(geom.top());
    if (pos.bottom() > geom.bottom())
        pos.moveBottom(geom.bottom());

    mPlugin->willShowWindow(m_volumePopup);
    m_volumePopup->openAt(pos.topLeft(), Qt::TopLeftCorner);
    m_volumePopup->activateWindow();
    setDown(true);
}

void VolumeButton::hideVolumeSlider()
{
    // qDebug() << "hideVolumeSlider";
    m_popupHideTimer.stop();
    m_volumePopup->hide();
    setDown(false);
    suppressTooltipTemporarily();
}

void VolumeButton::handleMixerLaunch()
{
    QProcess::startDetached(m_mixerCommand, m_mixerParams);
}

void VolumeButton::handleStockIconChanged(const QString &iconName)
{
    setIcon(XdgIcon::fromTheme(iconName));
}
