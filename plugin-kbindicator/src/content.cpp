/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Dmitriy Zhukov <zjesclean@gmail.com>
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

#include <QBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QEvent>
#include "kbdstate.h"
#include "content.h"

Content::Content(bool layoutEnabled):
    QWidget(),
    m_layoutEnabled(layoutEnabled)
{
    QBoxLayout *box = new QBoxLayout(QBoxLayout::LeftToRight);
    box->setContentsMargins(0, 0, 0, 0);
    box->setSpacing(0);
    setLayout(box);

    m_capsLock = new QLabel(tr("C", "Label for CapsLock indicator"));
    m_capsLock->setObjectName("CapsLockLabel");
    m_capsLock->setAlignment(Qt::AlignCenter);
    m_capsLock->setToolTip(tr("CapsLock", "Tooltip for CapsLock indicator"));
    m_capsLock->installEventFilter(this);
    layout()->addWidget(m_capsLock);

    m_numLock = new QLabel(tr("N", "Label for NumLock indicator"));
    m_numLock->setObjectName("NumLockLabel");
    m_numLock->setToolTip(tr("NumLock", "Tooltip for NumLock indicator"));
    m_numLock->setAlignment(Qt::AlignCenter);
    m_numLock->installEventFilter(this);
    layout()->addWidget(m_numLock);

    m_scrollLock = new QLabel(tr("S", "Label for ScrollLock indicator"));
    m_scrollLock->setObjectName("ScrollLockLabel");
    m_scrollLock->setToolTip(tr("ScrollLock", "Tooltip for ScrollLock indicator"));
    m_scrollLock->setAlignment(Qt::AlignCenter);
    m_scrollLock->installEventFilter(this);
    layout()->addWidget(m_scrollLock);

    m_layout = new QLabel;
    m_layout->setObjectName("LayoutLabel");
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->installEventFilter(this);
    layout()->addWidget(m_layout);
    m_layout->setEnabled(false);
}

Content::~Content()
{}

bool Content::setup()
{
    m_capsLock->setVisible(Settings::instance().showCapLock());
    m_numLock->setVisible(Settings::instance().showNumLock());
    m_scrollLock->setVisible(Settings::instance().showScrollLock());
    m_layout->setVisible(m_layoutEnabled && Settings::instance().showLayout());
    return true;
}

void Content::layoutChanged(const QString & sym, const QString & name, const QString & variant)
{
    m_layout->setText(sym.toUpper());
    QString txt = QString("<html><table>\
    <tr><td>%1: </td><td>%3</td></tr>\
    <tr><td>%2: </td><td>%4</td></tr>\
    </table></html>").arg(tr("Layout")).arg(tr("Variant")).arg(name).arg(variant);
    m_layout->setToolTip(txt);
}

void Content::modifierStateChanged(Controls mod, bool active)
{
    setEnabled(mod, active);
}


void Content::setEnabled(Controls cnt, bool enabled)
{
    widget(cnt)->setEnabled(enabled);
}

QWidget* Content::widget(Controls cnt) const
{
    switch(cnt){
    case Caps:   return m_capsLock;
    case Num:    return m_numLock;
    case Scroll: return m_scrollLock;
    case Layout: return m_layout;
    }
    return 0;
}

bool Content::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::QEvent::MouseButtonRelease)
    {
        if (object == m_capsLock)
            emit controlClicked(Controls::Caps);
        else if (object == m_numLock)
            emit controlClicked(Controls::Num);
        else if (object == m_scrollLock)
            emit controlClicked(Controls::Scroll);
        else if(object == m_layout){
            emit controlClicked(Controls::Layout);
        }
        return true;
    }

    return QObject::eventFilter(object, event);
}

void Content::showHorizontal()
{
    qobject_cast<QBoxLayout*>(layout())->setDirection(QBoxLayout::LeftToRight);
}

void Content::showVertical()
{
    qobject_cast<QBoxLayout*>(layout())->setDirection(QBoxLayout::TopToBottom);
}
