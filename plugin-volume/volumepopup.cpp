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

#include "volumepopup.h"

#include "audiodevice.h"

#include <QIcon>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QFrame>
#include <QApplication>
#include <QFontMetrics>
#include <QToolTip>
#include <QTimer>
#include <QWheelEvent>
#include <QScreen>

VolumePopup::VolumePopup(QWidget* parent):
    QDialog(parent, Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup | Qt::X11BypassWindowManagerHint),
    m_pos(0, 0),
    m_anchor(Qt::TopLeftCorner),
    m_defaultSink(nullptr),
    m_sliderStep(3)
{
    // Under some Wayland compositors, setting window flags in the c-tor of the base class
    // may not be enough for a correct positioning of the popup.
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup | Qt::X11BypassWindowManagerHint);

    m_mixerButton = new QPushButton(this);
    m_mixerButton->setObjectName(QStringLiteral("MixerLink"));
    m_mixerButton->setMinimumWidth(1);
    m_mixerButton->setToolTip(tr("Launch mixer"));
    m_mixerButton->setText(tr("Mi&xer"));
    m_mixerButton->setAutoDefault(false);

    m_sinkScrollArea = new QScrollArea(this);
    m_sinkScrollArea->setWidgetResizable(true);
    m_sinkScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_sinkScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_sinkScrollArea->setFrameShape(QFrame::NoFrame);
    m_sinkScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_sinksContainer = new QWidget(this);
    QVBoxLayout *sinksLayout = new QVBoxLayout(m_sinksContainer);
    sinksLayout->setContentsMargins(QMargins());
    sinksLayout->setSpacing(0);
    m_sinkScrollArea->setWidget(m_sinksContainer);

    setMinimumWidth(420);
    setMinimumHeight(140);
    m_sinkScrollArea->setMaximumHeight(260);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setSpacing(0);
    l->setContentsMargins(QMargins());

    l->addWidget(m_mixerButton, 0, Qt::AlignHCenter);
    l->addWidget(m_sinkScrollArea);

    connect(m_mixerButton, &QPushButton::released, this, &VolumePopup::launchMixer);
}

bool VolumePopup::event(QEvent *event)
{
    if(event->type() == QEvent::WindowDeactivate)
    {
        // qDebug("QEvent::WindowDeactivate");
        hide();
    }
    return QDialog::event(event);
}

bool VolumePopup::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
        if (wheelEvent)
        {
            for (const SinkRow &row : std::as_const(m_sinkRows))
            {
                if (row.slider && watched == row.slider)
                {
                    handleWheelEvent(wheelEvent);
                    return true;
                }
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

void VolumePopup::enterEvent(QEnterEvent * /*event*/)
{
    emit mouseEntered();
}

void VolumePopup::leaveEvent(QEvent * /*event*/)
{
    // qDebug("leaveEvent");
}

void VolumePopup::handleSliderValueChanged(int value)
{
    QSlider *slider = qobject_cast<QSlider*>(sender());
    if (!slider)
        return;
    for (const SinkRow &row : std::as_const(m_sinkRows))
    {
        if (row.slider == slider && row.device)
        {
            // qDebug("VolumePopup::handleSliderValueChanged: %d\n", value);
            row.device->setVolume(value);
            const QString tip = slider->toolTip();
            QTimer::singleShot(0, [this, tip]() { QToolTip::showText(QCursor::pos(), tip, this); });
            return;
        }
    }
}

void VolumePopup::handleMuteToggleClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;
    for (const SinkRow &row : std::as_const(m_sinkRows))
    {
        if (row.muteButton == btn && row.device)
        {
            row.device->toggleMute();
            return;
        }
    }
}

void VolumePopup::handleSetDefaultClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;
    for (const SinkRow &row : std::as_const(m_sinkRows))
    {
        if (row.defaultButton == btn && row.device)
        {
            emit defaultSinkRequested(row.device);
            return;
        }
    }
}

void VolumePopup::handleDeviceVolumeChanged(AudioDevice *device, int volume)
{
    for (const SinkRow &row : std::as_const(m_sinkRows))
    {
        if (row.device == device && row.slider)
        {
            // Calling setValue() would trigger handleSliderValueChanged() and set the device volume
            // again, so we have to block the signals to avoid recursive signal emission.
            row.slider->blockSignals(true);
            row.slider->setValue(volume);
            row.slider->setToolTip(QStringLiteral("%1%").arg(volume));
            row.slider->blockSignals(false);
            if (device == m_defaultSink)
                onDefaultSinkVolumeOrMuteChanged();
            return;
        }
    }
}

void VolumePopup::handleDeviceMuteChanged(AudioDevice *device, bool mute)
{
    for (const SinkRow &row : std::as_const(m_sinkRows))
    {
        if (row.device == device && row.muteButton)
        {
            row.muteButton->setChecked(mute);
            if (device == m_defaultSink)
                onDefaultSinkVolumeOrMuteChanged();
            return;
        }
    }
}

void VolumePopup::onDefaultSinkVolumeOrMuteChanged()
{
    updateStockIcon();
}

void VolumePopup::updateStockIcon()
{
    if (!m_defaultSink)
        return;

    QString iconName;
    if (m_defaultSink->volume() <= 0 || m_defaultSink->mute())
        iconName = QLatin1String("audio-volume-muted");
    else if (m_defaultSink->volume() <= 33)
        iconName = QLatin1String("audio-volume-low");
    else if (m_defaultSink->volume() <= 66)
        iconName = QLatin1String("audio-volume-medium");
    else
        iconName = QLatin1String("audio-volume-high");

    iconName.append(QLatin1String("-panel"));
    emit stockIconChanged(iconName);
}

void VolumePopup::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    realign();
}

void VolumePopup::openAt(QPoint pos, Qt::Corner anchor)
{
    m_pos = pos;
    m_anchor = anchor;
    realign();
    show();
}

void VolumePopup::handleWheelEvent(QWheelEvent *event)
{
    if (m_defaultSink)
    {
        const int step = event->angleDelta().y() / QWheelEvent::DefaultDeltasPerStep * m_sliderStep;
        m_defaultSink->setVolume(m_defaultSink->volume() + step);
        return;
    }
    if (m_sinkRows.size() == 1 && m_sinkRows.at(0).slider && m_sinkRows.at(0).device)
    {
        QSlider *slider = m_sinkRows.at(0).slider;
        const int step = event->angleDelta().y() / QWheelEvent::DefaultDeltasPerStep * slider->singleStep();
        m_sinkRows.at(0).device->setVolume(slider->sliderPosition() + step);
    }
}

void VolumePopup::setDevice(AudioDevice *device)
{
    if (device)
        setSinks({device}, device);
    else
        setSinks({}, nullptr);
}

void VolumePopup::setSinks(const QList<AudioDevice*> &sinks, AudioDevice *defaultSink)
{
    if (m_sinks == sinks && m_defaultSink == defaultSink)
        return;

    // Disconnect old default sink.
    if (m_defaultSink)
        disconnect(m_defaultSink, nullptr, this, nullptr);

    m_sinks = sinks;
    m_defaultSink = defaultSink;

    if (m_defaultSink)
    {
        connect(m_defaultSink, &AudioDevice::volumeChanged, this, [this](int v) { handleDeviceVolumeChanged(m_defaultSink, v); });
        connect(m_defaultSink, &AudioDevice::muteChanged, this, [this](bool m) { handleDeviceMuteChanged(m_defaultSink, m); });
    }

    rebuildSinkRows();
    updateStockIcon();
    emit deviceChanged();
}

void VolumePopup::setDefaultSink(AudioDevice *defaultSink)
{
    if (m_defaultSink == defaultSink)
        return;

    if (m_defaultSink)
        disconnect(m_defaultSink, nullptr, this, nullptr);

    m_defaultSink = defaultSink;

    if (m_defaultSink)
    {
        connect(m_defaultSink, &AudioDevice::volumeChanged, this, [this](int v) { handleDeviceVolumeChanged(m_defaultSink, v); });
        connect(m_defaultSink, &AudioDevice::muteChanged, this, [this](bool m) { handleDeviceMuteChanged(m_defaultSink, m); });
    }

    updateDefaultButtons();
    updateStockIcon();
    emit deviceChanged();
}

void VolumePopup::setSliderStep(int step)
{
    m_sliderStep = step;
    for (const SinkRow &row : std::as_const(m_sinkRows))
    {
        if (row.slider)
        {
            row.slider->setSingleStep(step);
            row.slider->setPageStep(step * 10);
        }
    }
}

void VolumePopup::rebuildSinkRows()
{
    for (const SinkRow &row : std::as_const(m_sinkRows))
    {
        if (row.device)
            disconnect(row.device, nullptr, this, nullptr);
        if (row.rowWidget)
            row.rowWidget->deleteLater();
    }
    m_sinkRows.clear();

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(m_sinksContainer->layout());
    if (!layout)
        return;

    for (AudioDevice *dev : std::as_const(m_sinks))
    {
        SinkRow row = makeSinkRow(dev);
        if (!row.rowWidget)
            continue;

        layout->addWidget(row.rowWidget);
        m_sinkRows.append(row);

        connect(dev, &AudioDevice::volumeChanged, this, [this, dev](int v) { handleDeviceVolumeChanged(dev, v); });
        connect(dev, &AudioDevice::muteChanged, this, [this, dev](bool m) { handleDeviceMuteChanged(dev, m); });

        if (row.slider)
        {
            connect(row.slider, &QSlider::valueChanged, this, &VolumePopup::handleSliderValueChanged);
            row.slider->installEventFilter(this);
        }
        if (row.muteButton)
            connect(row.muteButton, &QPushButton::clicked, this, &VolumePopup::handleMuteToggleClicked);
        if (row.defaultButton)
            connect(row.defaultButton, &QPushButton::clicked, this, &VolumePopup::handleSetDefaultClicked);
    }

    updateDefaultButtons();
    setSliderStep(m_sliderStep);
}

SinkRow VolumePopup::makeSinkRow(AudioDevice *device)
{
    SinkRow row;
    row.device = device;
    if (!device)
        return row;

    QFrame *frame = new QFrame(m_sinksContainer);
    frame->setFrameShape(QFrame::NoFrame);
    row.rowWidget = frame;
    QHBoxLayout *rowLayout = new QHBoxLayout(row.rowWidget);
    rowLayout->setContentsMargins(0, 2, 0, 2);
    rowLayout->setSpacing(8);

    const int labelFixedW = 180;
    const QString desc = device->description();
    QLabel *label = new QLabel(row.rowWidget);
    label->setToolTip(desc);
    label->setFixedWidth(labelFixedW);
    label->setText(QFontMetrics(label->font()).elidedText(desc, Qt::ElideRight, labelFixedW));
    rowLayout->addWidget(label);

    row.slider = new QSlider(Qt::Horizontal, row.rowWidget);
    // The volume sliders show 0-100; volumes of all devices should be converted to percentages.
    row.slider->setRange(0, 100);
    row.slider->setValue(device->volume());
    row.slider->setToolTip(QStringLiteral("%1%").arg(device->volume()));
    row.slider->setSingleStep(m_sliderStep);
    row.slider->setPageStep(m_sliderStep * 10);
    row.slider->setMinimumWidth(120);
    rowLayout->addWidget(row.slider, 1);

    row.muteButton = new QPushButton(row.rowWidget);
    row.muteButton->setCheckable(true);
    row.muteButton->setChecked(device->mute());
    row.muteButton->setIcon(QIcon::fromTheme(QLatin1String("audio-volume-muted-panel")));
    row.muteButton->setToolTip(tr("Mute"));
    row.muteButton->setFixedSize(28, 28);
    row.muteButton->setAutoDefault(false);
    rowLayout->addWidget(row.muteButton);

    row.defaultButton = new QPushButton(row.rowWidget);
    row.defaultButton->setCheckable(true);
    row.defaultButton->setToolTip(tr("Set as default output"));
    row.defaultButton->setFixedSize(28, 28);
    row.defaultButton->setAutoDefault(false);
    rowLayout->addWidget(row.defaultButton);

    return row;
}

void VolumePopup::updateDefaultButtons()
{
    for (SinkRow &row : m_sinkRows)
    {
        if (!row.defaultButton || !row.device)
            continue;
        const bool isDefault = (row.device == m_defaultSink);
        row.defaultButton->setChecked(isDefault);
        row.defaultButton->setEnabled(!isDefault);
        row.defaultButton->setIcon(QIcon::fromTheme(QLatin1String("emblem-default-symbolic")));
        row.defaultButton->setToolTip(isDefault ? tr("Default output") : tr("Set as default output"));
    }
}

void VolumePopup::realign()
{
    QRect rect;
    rect.setSize(sizeHint());
    switch (m_anchor)
    {
    case Qt::TopLeftCorner:
        rect.moveTopLeft(m_pos);
        break;

    case Qt::TopRightCorner:
        rect.moveTopRight(m_pos);
        break;

    case Qt::BottomLeftCorner:
        rect.moveBottomLeft(m_pos);
        break;

    case Qt::BottomRightCorner:
        rect.moveBottomRight(m_pos);
        break;

    }

    if (QScreen const * const screen = QGuiApplication::screenAt(m_pos))
    {
        auto const & geometry = screen->availableGeometry();

        if (rect.left() < geometry.left())
            rect.moveLeft(geometry.left());
        if (rect.right() > geometry.right())
            rect.moveRight(geometry.right());
        if (rect.top() < geometry.top())
            rect.moveTop(geometry.top());
        if (rect.bottom() > geometry.bottom())
            rect.moveBottom(geometry.bottom());
    }

    move(rect.topLeft());
}
