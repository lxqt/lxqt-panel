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

#include "qeyesimagewidget.h"


void ImageStretcher::load(QString fn) {
    if (fn.toLower().endsWith(QString::fromUtf8(".svg"))) {
        svg = true;
        svgrender.load(fn);
    } else {
        origImage.load(fn);
        svg = false;
    }
}
    
QPixmap & ImageStretcher::ImageStretcher::getImage(int w, int h) {
    if (w == strechedImage.width() && h == strechedImage.height())
        return strechedImage;
    if (svg) {
        strechedImage = QPixmap(w, h);
        strechedImage.fill(QColor(0,0,0,0));
        QPainter painter(&strechedImage);
        svgrender.render(&painter, strechedImage.rect());
    } else {
        strechedImage = origImage.scaled(w, h,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    return strechedImage;
}

int ImageStretcher::origWidth() {
    if (svg)
        return svgrender.viewBox().width();
    else
        return origImage.width();
}

int ImageStretcher::origHeight() {
    if (svg)
        return svgrender.viewBox().height();
    else
        return origImage.height();
}

int ImageStretcher::strechedWidth() {
    return strechedImage.width();
}
int ImageStretcher::strechedHeight() {
    return strechedImage.height();
}

void QEyesImageWidget::drawEye(QPainter &painter, int x, int y, int dx, int dy) {
    painter.drawPixmap(x, y, background.getImage(dx, dy));
}
void QEyesImageWidget::drawPupil(QPainter &painter, int x, int y) {

    auto & img = pupil.getImage( 
            pupil.origWidth() * background.strechedWidth() / background.origWidth(),
            pupil.origHeight() * background.strechedHeight() / background.origHeight());

    painter.drawPixmap(x - img.width() / 2, y - img.height() / 2, img);
}

void QEyesImageWidget::eyeBorder(float &bx, float &by) {
    bx = borderXStreched;
    by = borderYStreched;
}

void QEyesImageWidget::load(const QString &eye, const QString &pupil_,
                                int wall, int num) {

    borderY = borderX = wall;
    background.load(eye);
    pupil.load(pupil_);
    numEyes = num;
}

void QEyesImageWidget::paintEvent(QPaintEvent *event) {
    if (width() != oldWidth || height() != oldHeight) {
        const auto dx = width() / numEyes;
        background.getImage(dx, height());
        
        borderYStreched = borderY * background.strechedHeight() / background.origHeight();
        borderXStreched = borderX * background.strechedWidth() / background.origWidth();
            
        oldWidth = width();
        oldHeight = height();
    }
    QAbstractEyesWidget::paintEvent(event);
}

