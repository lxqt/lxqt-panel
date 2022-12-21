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

#include "qeyeswidget.h"

class QEyesVectorWidget : public QAbstractEyesWidget
{
    Q_OBJECT

    float lineWidth = 0.08f;
    int lineMinWidth = 1;
    float pupilSize = 3.0f; /* relative to line width */
    QString borderColor = QString::fromUtf8("black");
    QString pupilColor = QString::fromUtf8("black");
    QString centerColor = QString::fromUtf8("white");
    QString peripheralColor = QString::fromUtf8("light-gray");

    virtual void drawEye(QPainter &painter, int x, int y, int dx, int dy) override;
    virtual void drawPupil(QPainter &painter, int x, int y) override;
    virtual void eyeBorder(float &bx, float &by) override;

public:
    QEyesVectorWidget(QWidget *parent = nullptr);
    void setBorderColor(const QString &color) { borderColor = color; }
    void setCenterColor(const QString &color) { centerColor = color; }
    void setPupilColor(const QString &color) { pupilColor = color; }
    void setPeripheralColor(const QString &color) { peripheralColor = color; }
};
