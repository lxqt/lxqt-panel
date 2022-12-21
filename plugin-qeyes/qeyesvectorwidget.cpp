/*
 *  qeyes - an xeyes clone
 *
 *  Copyright (C) 2022 Goffredo Baroncelli <kreijack@inwind.it>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QCursor>
#include <QtGui/QRadialGradient>

#include "qeyesvectorwidget.h"

QEyesVectorWidget::QEyesVectorWidget(QWidget *parent) : QAbstractEyesWidget(parent) {}

void QEyesVectorWidget::drawEye(QPainter &painter, int x, int y, int dx, int dy) {

    float w = width() / numEyes < height() ? width() / numEyes: height();
    w *= lineWidth;
    if (w < lineMinWidth)
        w = lineMinWidth;

    painter.setPen(borderColor);
    auto pen = painter.pen();
    pen.setWidth(w);
    painter.setPen(pen);
    QRadialGradient grad(x + dx / 2.0, y + dy / 2.0, std::max(dx, dy));
    grad.setColorAt(0, centerColor);
    grad.setColorAt(1, peripheralColor);
    painter.setBrush(grad);
    painter.drawEllipse(x + w, y + w, dx - 2.0 * w, dy - 2.0 * w);

}
void QEyesVectorWidget::drawPupil(QPainter &painter, int x, int y) {
    float w = width() / numEyes < height() ? width() / numEyes: height();
    w *= lineWidth;
    if (w < lineMinWidth)
        w = lineMinWidth;
    const auto psize = w * pupilSize;

    auto pen = painter.pen();
    pen.setWidth(1);
    painter.setBrush(QColor(pupilColor));
    painter.setPen(QColor(pupilColor));

    painter.drawEllipse(QPoint(x, y), (int)(psize / 2.0), (int)(psize / 2.0));
}

void QEyesVectorWidget::eyeBorder(float &bx, float &by) {
    float w = width() / numEyes < height() ? width() / numEyes: height();
    w *= lineWidth;
    if (w < lineMinWidth)
        w = lineMinWidth;
    const auto psize = w * pupilSize;

    bx = by = 1.5 * w + psize/2;
}
