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
#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <stdio.h>
#include <math.h>

#include "qeyeswidget.h"

QAbstractEyesWidget::QAbstractEyesWidget(QWidget *parent) : QWidget(parent) {
    connect(&timer, &QTimer::timeout, this, &QAbstractEyesWidget::timeout);
    setMouseTracking(true);
    if (!underMouse()) {
        timer.setInterval(timerTimeout);
        timer.start();
    }
    //setContextMenuPolicy(Qt::CustomContextMenu);

    //connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
      //  this, SLOT(showContextMenu(const QPoint &)));
}

QAbstractEyesWidget::~QAbstractEyesWidget() {
    timer.stop();
}

void QAbstractEyesWidget::leaveEvent(QEvent *) {
    timer.setInterval(timerTimeout);
    timer.start();
}

void QAbstractEyesWidget::enterEvent(QEvent *) {
    timer.stop();
}

void QAbstractEyesWidget::mouseMoveEvent(QMouseEvent  *) {
    repaint();
}

void QAbstractEyesWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    if (!transparent) {
        painter.fillRect(0, 0, width(), height(), bgColor);
    } else {
        painter.fillRect(0, 0, width(), height(), Qt::transparent);
    }
    const auto dx = 1.0 * width() / numEyes;

    for (int i = 0 ; i < numEyes ; i++)
        drawEye(painter, i * dx, 0, dx+0.9, height());

    const auto pos = mapFromGlobal(QCursor::pos());
    float borderX, borderY;
    eyeBorder(borderX, borderY);
    for (int i = 0 ; i < numEyes ; i++) {

        /* center of the eye */
        const auto x0 = i * dx + dx / 2 ;
        const auto y0 = height() / 2;

        /* radius of the ellipse */
        const float ry = height() / 2 - borderY;
        const float rx = dx / 2 - borderX;

        /* angle */
        const float dx = pos.x() - x0;
        const float dy = pos.y() - y0;

        /*
         *  dy      y      ry      sin(alpha)
         * ----  = --- =  ---- * -------------  
         *  dx      x      rx      cos(alpha)
         * 
         * 
         *  dy / ry      sin(alpha)
         * ---------  = ------------- = tan(alpha)
         *  dx / rx      cos(alpha)
         * 
         * 
         * alpha = atan2( dy/rx, dx/rx)
         * 
         */

        const auto alpha = atan2(dy/ry, dx/rx);

        /* pupil center */
        auto y = ry * sin(alpha);
        auto x = rx * cos(alpha);

        /* 
         * if the cursor is inside the eye, the pupil position is
         * the cursor
         */
        if (y < 0 && dy < 0 && dy > y)
            y = dy;
        else if (y > 0 && dy > 0 && dy < y)
            y = dy;
        if (x < 0 && dx < 0 && dx > x)
            x = dx;
        else if (x > 0 && dx > 0 && dx < x)
            x = dx;

        //painter.drawEllipse(QPoint(x0 + x, y0 + y), psize / 2, psize / 2);
        drawPupil(painter, x0 +x, y0 + y);

    }

}

void QAbstractEyesWidget::timeout() {
    const auto pos = mapFromGlobal(QCursor::pos());
    if (pos == previousPos)
        return;
    previousPos = pos;

    update();
}
