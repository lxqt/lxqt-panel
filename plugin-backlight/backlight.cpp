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

BacklightButton::BacklightButton(QWidget *parent):
    QToolButton(parent),
    m_mouseWheelThresholdCounter(0) {}

void BacklightButton::wheelEvent(QWheelEvent *e)
{
    e->accept();
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

    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(2000);
    m_backlightSlider = new SliderDialog(m_backlightButton);
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
    connect(m_backlightButton, &QToolButton::clicked, this, &LXQtBacklight::togleSlider);
}


LXQtBacklight::~LXQtBacklight()
{
    delete m_backlightButton;
}


QWidget *LXQtBacklight::widget()
{
    return m_backlightButton;
}

void LXQtBacklight::togleSlider()
{
    if (m_backlightSlider->isVisible())
        m_backlightSlider->hide();
    else
    {
        QSize size = m_backlightSlider->sizeHint();
        QRect rect = calculatePopupWindowPos(size);
        m_backlightSlider->setGeometry(rect);
        m_backlightSlider->updateBacklight();
        m_backlightSlider->show();
        m_backlightSlider->setFocus();
    }
}



