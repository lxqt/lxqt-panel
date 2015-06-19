/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2011      Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#include "lxqtgrouppopup.h"
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QLayout>
#include <QDebug>

/************************************************
    this class is just a container of window buttons
    the main purpose is showing window buttons in
    vertical layout and drag&drop feature inside
    group
 ************************************************/
LxQtGroupPopup::LxQtGroupPopup(LxQtTaskGroup *group):
    QFrame(group),
    mGroup(group)
{
    Q_ASSERT(group);
    setAcceptDrops(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    setLayout(new QVBoxLayout);
    layout()->setSpacing(3);
    layout()->setMargin(3);

    connect(&mCloseTimer, &QTimer::timeout, this, &LxQtGroupPopup::closeTimerSlot);
    mCloseTimer.setSingleShot(true);
    mCloseTimer.setInterval(400);
}

LxQtGroupPopup::~LxQtGroupPopup()
{
}

void LxQtGroupPopup::dropEvent(QDropEvent *event)
{
    qlonglong temp;
    WId window;
    QDataStream stream(event->mimeData()->data(LxQtTaskButton::mimeDataFormat()));
    stream >> temp;
    window = (WId) temp;

    LxQtTaskButton *button;
    int oldIndex(0);
    // get current position of the button being dragged
    for (int i = 0; i < layout()->count(); i++)
    {
        LxQtTaskButton *b = qobject_cast<LxQtTaskButton*>(layout()->itemAt(i)->widget());
        if (b && b->windowId() == window)
        {
            button = b;
            oldIndex = i;
            break;
        }
    }

    int newIndex = -1;
    // find the new position to place it in
    for (int i = 0; i < oldIndex && newIndex == -1; i++)
    {
        QWidget *w = layout()->itemAt(i)->widget();
        if (w && w->pos().y() + w->height() / 2 > event->pos().y())
            newIndex = i;
    }
    const int size = layout()->count();
    for (int i = size - 1; i > oldIndex && newIndex == -1; i--)
    {
        QWidget *w = layout()->itemAt(i)->widget();
        if (w && w->pos().y() + w->height() / 2 < event->pos().y())
            newIndex = i;
    }

    if (newIndex == -1 || newIndex == oldIndex)
        return;

    QVBoxLayout * l = qobject_cast<QVBoxLayout *>(layout());
    l->takeAt(oldIndex);
    l->insertWidget(newIndex, button);
    l->invalidate();
}

void LxQtGroupPopup::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    QWidget::dragEnterEvent(event);
}

void LxQtGroupPopup::dragLeaveEvent(QDragLeaveEvent *event)
{
    hide(false/*not fast*/);
    QFrame::dragLeaveEvent(event);
}

/************************************************
 *
 ************************************************/
void LxQtGroupPopup::leaveEvent(QEvent *event)
{
    mCloseTimer.start();
}

/************************************************
 *
 ************************************************/
void LxQtGroupPopup::enterEvent(QEvent *event)
{
    mCloseTimer.stop();
}

void LxQtGroupPopup::hide(bool fast)
{
    if (fast)
        close();
    else
        mCloseTimer.start();
}

void LxQtGroupPopup::show()
{
    mCloseTimer.stop();
    QFrame::show();
}

void LxQtGroupPopup::closeTimerSlot()
{
    bool button_has_dnd_hover = false;
    QLayout* l = layout();
    for (int i = 0; l->count() > i; ++i)
    {
        LxQtTaskButton const * const button = dynamic_cast<LxQtTaskButton const *>(l->itemAt(i)->widget());
        if (0 != button && button->hasDragAndDropHover())
        {
            button_has_dnd_hover = true;
            break;
        }
    }
    if (!button_has_dnd_hover)
        close();
}
