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

#include <QVBoxLayout>
#include <QFrame>
#include <QEvent>
#include "sliderdialog.h"

SliderDialog::SliderDialog(QWidget *parent) : QDialog(parent, Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup | Qt::X11BypassWindowManagerHint)
{
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup | Qt::X11BypassWindowManagerHint);
    m_backlight = new LXQt::Backlight(this);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(2);
    
    m_upButton = new QToolButton();
    m_upButton->setText(QStringLiteral("☼"));
    layout->addWidget(m_upButton, 0, Qt::AlignHCenter);
    
    m_slider = new QSlider(this);
    
    layout->addWidget(m_slider, 0, Qt::AlignHCenter);
    
    m_downButton = new QToolButton();
    m_downButton->setText(QStringLiteral("☀"));
    layout->addWidget(m_downButton, 0, Qt::AlignHCenter);
    
    
    if(m_backlight->isBacklightAvailable() || m_backlight->isBacklightOff()) {
        m_slider->setMaximum(m_backlight->getMaxBacklight());
        // set the minimum to 5% of the maximum to prevent a black screen
        m_slider->setMinimum(qMax(qRound((qreal)(m_backlight->getMaxBacklight())*0.05), 1));
        m_slider->setValue(m_backlight->getBacklight());
    } else {
        m_slider->setValue(0);
        m_slider->setEnabled(false);
        m_upButton->setEnabled(false);
        m_downButton->setEnabled(false);
    }
    
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(m_upButton, SIGNAL(clicked(bool)), this, SLOT(upButtonClicked(bool)));
    connect(m_downButton, SIGNAL(clicked(bool)), this, SLOT(downButtonClicked(bool)));
}


void SliderDialog::sliderValueChanged(int value)
{
    m_backlight->setBacklight(value);
}


void SliderDialog::updateBacklight()
{
    m_slider->setValue(m_backlight->getBacklight());
}

void SliderDialog::downButtonClicked(bool)
{
    m_slider->setValue(m_slider->value() - 1);
}

void SliderDialog::upButtonClicked(bool)
{
    m_slider->setValue(m_slider->value() + 1);
}


bool SliderDialog::event(QEvent * event)
{
    if(event->type() == QEvent::WindowDeactivate || event->type() == QEvent::Hide) {
        hide();
        //printf("emit dialogClosed()\n");
        emit dialogClosed();
    }
    return QDialog::event(event);
}

