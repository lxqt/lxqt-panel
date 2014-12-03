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

#ifndef LXQTTASKPOPUP_H
#define LXQTTASKPOPUP_H

#include <QHash>
#include <QFrame>

class LxQtTaskButton;
class LxQtTaskBar;
class LxQtTaskGroup;
class LxQtMasterPopup;

class LxQtGroupPopup: public QFrame
{
    Q_OBJECT
public:
    LxQtGroupPopup(LxQtMasterPopup * parent, LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons);
    ~LxQtGroupPopup();

protected:
    void dragEnterEvent(QDragEnterEvent * event);
    void dropEvent(QDropEvent * event);

private:
    const QHash<WId, LxQtTaskButton *> & mButtonHash;
    LxQtTaskBar * parentTaskBar() ;
    LxQtMasterPopup * parentMasterPopup() ;
    LxQtTaskGroup * mGroup;


private slots:
    void buttonDropped(const QPoint & point, QDropEvent* event);
};


#endif // LXQTTASKPOPUP_H
