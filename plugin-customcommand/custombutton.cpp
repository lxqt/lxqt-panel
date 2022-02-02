/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2021 LXQt team
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

#include "custombutton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QProxyStyle>

CustomButton::CustomButton(ILXQtPanelPlugin *plugin, QWidget* parent):
        QToolButton(parent),
        mPlugin(plugin),
        mPanel(plugin->panel()),
        mAutoRotate(false),
        mMaxWidth(200),
        mSizeHint(QSize())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoRaise(true);
    setContentsMargins(0,0,0,0);
    setMinimumWidth(1);
    setMinimumHeight(1);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    updateButton();
}

CustomButton::~CustomButton() = default;

void CustomButton::wheelEvent(QWheelEvent *event)
{
    int y = event->angleDelta().y();
    emit wheelScrolled(y);
    event->accept();
}

void CustomButton::setMaxWidth(int maxWidth)
{
    if (mMaxWidth != maxWidth)
        mMaxWidth = maxWidth;
}

void CustomButton::setOrigin(Qt::Corner newOrigin)
{
    if (mOrigin != newOrigin)
        mOrigin = newOrigin;
}

void CustomButton::setAutoRotation(bool rotate)
{
    if (mSizeHint == sizeHint() && mAutoRotate == rotate && mPanelPosition == mPanel->position())
        return;

    mSizeHint = sizeHint();
    mAutoRotate = rotate;
    mPanelPosition = mPanel->position();
    int length = 0;

    switch (mPanel->position())
    {
    case ILXQtPanel::PositionTop:
    case ILXQtPanel::PositionBottom:
        setOrigin(Qt::TopLeftCorner);
        length = qMin(mSizeHint.width(), mMaxWidth);
        setFixedWidth(length);

        setMinimumHeight(1);
        setMaximumHeight(QWIDGETSIZE_MAX);
        break;

    case ILXQtPanel::PositionLeft:
        if (rotate) {
            setOrigin(Qt::BottomLeftCorner);
            length = qMin(mSizeHint.width(), mMaxWidth);
        }
        else {
            setOrigin(Qt::TopLeftCorner);
            length = qMin(mSizeHint.height(), mMaxWidth);
        }
        setMinimumWidth(1);
        setMaximumWidth(QWIDGETSIZE_MAX);

        setFixedHeight(length);
        break;

    case ILXQtPanel::PositionRight:
        if (rotate) {
            setOrigin(Qt::TopRightCorner);
            length = qMin(mSizeHint.width(), mMaxWidth);
        }
        else {
            setOrigin(Qt::TopLeftCorner);
            length = qMin(mSizeHint.height(), mMaxWidth);
        }
        setMinimumWidth(1);
        setMaximumWidth(QWIDGETSIZE_MAX);

        setFixedHeight(length);
        break;
    }
    update();

}

void CustomButton::updateButton()
{
    setAutoRotation(mAutoRotate);
}

void CustomButton::paintEvent(QPaintEvent *event)
{
    if (mOrigin == Qt::TopLeftCorner) {
        QToolButton::paintEvent(event);
        return;
    }

    QSize sz = size();
    bool transpose = false;
    QTransform transform;

    switch (mOrigin)
    {
    case Qt::TopLeftCorner:
        break;

    case Qt::TopRightCorner:
        transform.rotate(90.0);
        transform.translate(0.0, -sz.width());
        transpose = true;
        break;

    case Qt::BottomRightCorner:
        transform.rotate(180.0);
        transform.translate(-sz.width(), -sz.height());
        break;

    case Qt::BottomLeftCorner:
        transform.rotate(270.0);
        transform.translate(-sz.height(), 0.0);
        transpose = true;
        break;
    }

    QStylePainter painter(this);
    painter.setTransform(transform);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (transpose)
        opt.rect = opt.rect.transposed();

    painter.drawComplexControl(QStyle::CC_ToolButton, opt);
}
