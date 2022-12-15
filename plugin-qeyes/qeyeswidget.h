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

#include <QtWidgets/QWidget>
#include <QtWidgets/QMenu>
#include <QtCore/QTimer>
#include <QtGui/QPixmap>

class QAbstractEyesWidget : public QWidget
{
    Q_OBJECT

    QTimer timer;
    QPoint previousPos;
    int timerTimeout = 100; /* unit ms */
    QString bgColor = QString::fromUtf8("white");
    bool transparent = false;

private slots:
    void timeout();

protected:
    int numEyes = 3;

private:
    void leaveEvent(QEvent *) override;
    void enterEvent(QEvent *) override;
    void mouseMoveEvent(QMouseEvent  *) override;

protected:
    virtual void drawEye(QPainter &painter, int x, int y, int dx, int dy) = 0;
    virtual void drawPupil(QPainter &painter, int x, int y) = 0;
    virtual void eyeBorder(float &bx, float &by) = 0;

    void paintEvent(QPaintEvent *event) override;

public:
    QAbstractEyesWidget(QWidget *parent = nullptr);
    ~QAbstractEyesWidget();
    void setNumEyes(int n) { numEyes = n; }
    int getNumEyes() { return numEyes; }
    void setBGColor(const QString &color) { bgColor = color; }
    void setTransparent(bool t = true) { transparent = t; }

};
