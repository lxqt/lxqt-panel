/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt-project.org/
 *
 * Copyright: 2025 LXQt team
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

#include "colorLabel.h"
#include <QColorDialog>
#include <QStyleOptionFrame>
#include <QPainter>

ColorLabel::ColorLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setLineWidth(1);
    setToolTip(tr("Click to change color."));
}

ColorLabel::~ColorLabel() {}

void ColorLabel::setColor(const QColor& color, bool announceChange)
{
    if (!color.isValid() || color == color_)
        return;
    color_ = color;
    color_.setAlpha(255); // ignore translucency
    update();
    if (announceChange)
        emit colorChanged();
}

QColor ColorLabel::getColor() const
{
    return color_;
}

void ColorLabel::reset()
{
    color_ = QColor();
    update();
}

void ColorLabel::mousePressEvent(QMouseEvent* /*event*/)
{
    QColor color = QColorDialog::getColor(color_, window(), tr("Select Color"));
    setColor(color, true);
}

void ColorLabel::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    if (color_.isValid())
        p.fillRect(contentsRect(), color_);
    QStyleOptionFrame opt;
    initStyleOption(&opt);
    style()->drawControl(QStyle::CE_ShapedFrame, &opt, &p, this);
}

