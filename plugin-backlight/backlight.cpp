/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2020 LXQt team
 * Authors:
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

#include "backlight.h"
#include <QEvent>

#include <cmath>
#include <QCursor>
#include <QToolTip>

BacklightButton::BacklightButton(QWidget *parent):
    QToolButton(parent),
    m_mouseWheelThresholdCounter(0) {}

void BacklightButton::enterEvent(QEnterEvent *event)
{
    if (m_sliderDialog && m_sliderDialog->isVisible())
        return;
    QToolTip::showText(event->globalPosition().toPoint(), toolTip(), this);
}

void BacklightButton::mouseMoveEvent(QMouseEvent *event)
{
    QToolButton::mouseMoveEvent(event);
    if (m_sliderDialog && m_sliderDialog->isVisible())
        return;
    if (!QToolTip::isVisible())
        QToolTip::showText(event->globalPosition().toPoint(), toolTip(), this);
}

void BacklightButton::wheelEvent(QWheelEvent *e)
{
    e->accept();
    QToolTip::showText(e->globalPosition().toPoint(), toolTip(), this);
    QPoint angleDelta = e->angleDelta();
    Qt::Orientation orient = (std::abs(angleDelta.x()) > std::abs(angleDelta.y()) ? Qt::Horizontal : Qt::Vertical);
    int rotationSteps = (orient == Qt::Horizontal ? angleDelta.x() : angleDelta.y());
    m_mouseWheelThresholdCounter += rotationSteps;
    if(std::abs(m_mouseWheelThresholdCounter) < 100)
        return;
    emit wheel(rotationSteps > 0 ? true : false);
    m_mouseWheelThresholdCounter = 0;
}

LXQtBacklight::LXQtBacklight(const ILXQtPanelPluginStartupInfo &startupInfo):
        QObject(),
        ILXQtPanelPlugin(startupInfo)
{
    m_backlightButton = new BacklightButton();
    // use our own icon
    m_backlightButton->setIcon(QIcon::fromTheme(QStringLiteral("brightnesssettings")));
    m_backlightButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_backlight = new LXQt::Backlight(this);
    connect(m_backlight, &LXQt::Backlight::backlightChanged, this, &LXQtBacklight::updateBacklightTooltip);
    updateBacklightTooltip();

    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(2000);
    m_backlightSlider = new SliderDialog(m_backlightButton);
    m_backlightButton->setSliderDialog(m_backlightSlider);
    connect(m_backlightButton, &BacklightButton::wheel, m_backlightSlider, [this](bool up) {
        // Using a timer is only a safeguard against returning the slider to its previous value
        // on updating it, although that should not happen with the code of SliderDialog.
        if (!m_updateTimer.isActive())
            m_backlightSlider->updateBacklight();
        m_updateTimer.start();

        if (up)
            m_backlightSlider->upButtonClicked(true);
        else
            m_backlightSlider->downButtonClicked(true);
    });
    connect(m_backlightButton, &QToolButton::clicked, this, &LXQtBacklight::toggleSlider);
}


LXQtBacklight::~LXQtBacklight()
{
    delete m_backlightButton;
}


QWidget *LXQtBacklight::widget()
{
    return m_backlightButton;
}

void LXQtBacklight::toggleSlider()
{
    if (m_backlightSlider->isVisible())
        m_backlightSlider->hide();
    else
    {
        QToolTip::hideText();
        QSize size = m_backlightSlider->sizeHint();
        QRect rect = calculatePopupWindowPos(size);
        m_backlightSlider->setGeometry(rect);
        m_backlightSlider->updateBacklight();
        m_backlightSlider->show();
        m_backlightSlider->setFocus();
    }
}

void LXQtBacklight::updateBacklightTooltip()
{
    if (m_backlight->isBacklightAvailable() && m_backlight->getMaxBacklight() > 0)
        m_backlightButton->setToolTip(tr("Backlight: %1%").arg(m_backlight->getBacklight() * 100 / m_backlight->getMaxBacklight()));
    else
        m_backlightButton->setToolTip(QString());
    if (m_backlightButton->underMouse())
        QToolTip::showText(QCursor::pos(), m_backlightButton->toolTip(), m_backlightButton);
}



