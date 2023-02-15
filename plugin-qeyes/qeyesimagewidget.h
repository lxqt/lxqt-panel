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

#pragma once

#include <QtSvg/QSvgRenderer>
#include "qeyeswidget.h"


class ImageStretcher {
    bool svg;
    QSvgRenderer svgrender;
    QPixmap origImage, stretchedImage;
public:
    bool load(QString fn);
    QPixmap &getImage(int w, int h);
    int origWidth();
    int origHeight();
    int stretchedWidth();
    int stretchedHeight();
};

class QEyesImageWidget : public QAbstractEyesWidget {
private:
    void drawEye(QPainter &painter, int x, int y, int dx, int dy) override;
    void drawPupil(QPainter &painter, int x, int y) override;
    virtual void eyeBorder(float &bx, float &by) override;
    void paintEvent(QPaintEvent *event) override;

    ImageStretcher pupil, background;

    int oldWidth = -1, oldHeight = -1;
    float borderY = 0, borderXStretched = 0;
    float borderX = 0, borderYStretched = 0;

public:
    //QEyesImageWidget(const QString &path = QString(), QWidget *parent = nullptr);
    QEyesImageWidget(QWidget *parent = nullptr) : QAbstractEyesWidget(parent) {}
    bool load(const QString &eye, const QString &pupil, int wall, int num);
};
