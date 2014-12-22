/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 * http://lxqt.org
 *
 * Copyright: 2011 Razor team
 *            2014 LXQt team
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

#include "lxqtmasterpopup.h"
#include "lxqtgrouppopup.h"
#include "lxqttaskbutton.h"
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QLayout>
#include "lxqttaskbar.h"
#include <QDebug>

/************************************************
    this class is just a container of window buttons
    the main purpose is showing window buttons in
    vertical layout and drag&drop feature inside
    group
 ************************************************/
LxQtGroupPopup::LxQtGroupPopup(LxQtMasterPopup * parent, LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons):
    QFrame(parent),
    mButtonHash(buttons),
    mGroup(group)
{
    Q_ASSERT(group);
    Q_ASSERT(parent);
    setAcceptDrops(true);
}

/************************************************

 ************************************************/
LxQtGroupPopup::~LxQtGroupPopup()
{
}

/************************************************

 ************************************************/
LxQtMasterPopup * LxQtGroupPopup::parentMasterPopup()
{
    return LxQtMasterPopup::instance(parentTaskBar());
}

/************************************************

 ************************************************/
LxQtTaskBar * LxQtGroupPopup::parentTaskBar()
{
    return mGroup->parentTaskBar();
}

/************************************************

 ************************************************/
void LxQtGroupPopup::dropEvent(QDropEvent *event)
{
    buttonDropped(event->pos(),event);
}

/************************************************
    dragging buttons inside group
 ************************************************/
void LxQtGroupPopup::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasFormat("lxqt/lxqttaskbutton"))
    {
        event->ignore();
        return;
    }
    if (mButtonHash.count() == 1)
    {
        event->ignore();
        return;
    }

    event->acceptProposedAction();
    QWidget::dragEnterEvent(event);
}

/************************************************
    button dragged inside group dropped
    reorder layout
 ************************************************/
void LxQtGroupPopup::buttonDropped(const  QPoint& point, QDropEvent *event)
{
    WId window;
    QDataStream stream(event->mimeData()->data("lxqt/lxqttaskbutton"));
    stream >> window;
    if (!mButtonHash.contains(window))
    {
        return;
    }

    LxQtTaskButton * dragged = mButtonHash.value(window);
    int droppedIndex = layout()->indexOf(dragged);
    int newIdx = -1;
    int temp;

    int oldTreshold = 0;
    for (int i = 0 ; i < layout()->count(); i++)
    {
        QWidget * w = layout()->itemAt(i)->widget();
        LxQtTaskButton * b = qobject_cast<LxQtTaskButton*>(w);
        if (b && w->isVisibleTo(this))
        {
            int treshold = b->pos().y() + b->height() ;
            if (oldTreshold <= point.y() && point.y() < treshold)
            {
                newIdx = i;
                break;
            }
            temp = i;
            oldTreshold = treshold;
        }
    }

    /*
    if (newIdx == -1)
        newIdx = temp+ 1;
        */

    QVBoxLayout * l = qobject_cast<QVBoxLayout *>(layout());
    l->insertWidget(newIdx,dragged);
}
