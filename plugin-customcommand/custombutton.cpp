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

class LeftAlignedTextStyle : public QProxyStyle
{
    using QProxyStyle::QProxyStyle;
public:

    virtual void drawItemText(QPainter * painter, const QRect & rect, int flags
            , const QPalette & pal, bool enabled, const QString & text
            , QPalette::ColorRole textRole = QPalette::NoRole) const override;
};

void LeftAlignedTextStyle::drawItemText(QPainter * painter, const QRect & rect, int flags
            , const QPalette & pal, bool enabled, const QString & text
            , QPalette::ColorRole textRole) const
{
    QString txt = text;
    // get the button text because the text that's given to this function may be middle-elided
    if (const QToolButton *tb = dynamic_cast<const QToolButton*>(painter->device()))
        txt = tb->text();
    txt = QFontMetrics(painter->font()).elidedText(txt, Qt::ElideRight, rect.width());
    QProxyStyle::drawItemText(painter, rect, (flags & ~Qt::AlignHCenter) | Qt::AlignLeft, pal, enabled, txt, textRole);
}


CustomButton::CustomButton(ILXQtPanelPlugin *plugin, QWidget* parent):
        QToolButton(parent),
        mPlugin(plugin),
        mPanel(plugin->panel()),
        mMaxWidth(200)

{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoRaise(true);
    setContentsMargins(0,0,0,0);
    setMinimumWidth(1);
    setMinimumHeight(1);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setStyle(new LeftAlignedTextStyle());
    updateWidth();

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
    mMaxWidth = maxWidth;
    updateWidth();
}

void CustomButton::updateWidth()
{
    int newWidth = qMin(sizeHint().width(), mMaxWidth);
    if (mOrigin == Qt::TopLeftCorner) {
        setFixedWidth(newWidth);

        setMinimumHeight(1);
        setMaximumHeight(QWIDGETSIZE_MAX);
    }
    else {
        setMinimumWidth(1);
        setMaximumWidth(QWIDGETSIZE_MAX);

        setFixedHeight(newWidth);
    }
    update();
}

void CustomButton::setOrigin(Qt::Corner newOrigin)
{
    if (mOrigin != newOrigin) {
        mOrigin = newOrigin;
        updateWidth();
    }
}

void CustomButton::setAutoRotation(bool value)
{
    if (value) {
        switch (mPanel->position())
        {
        case ILXQtPanel::PositionTop:
        case ILXQtPanel::PositionBottom:
            setOrigin(Qt::TopLeftCorner);
            break;

        case ILXQtPanel::PositionLeft:
            setOrigin(Qt::BottomLeftCorner);
            break;

        case ILXQtPanel::PositionRight:
            setOrigin(Qt::TopRightCorner);
            break;
        }
    }
    else
        setOrigin(Qt::TopLeftCorner);


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
