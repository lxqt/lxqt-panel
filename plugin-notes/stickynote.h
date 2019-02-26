/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Matteo Fois <giomatfois62@yahoo.it>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */
 
#ifndef LXQT_STICKY_H
#define LXQT_STICKY_H

#include <QWidget>

namespace Ui {
class StickyNote;
}

class StickyNote: public QWidget
{
    Q_OBJECT
public:
    StickyNote(qint64 id = 0, QWidget* parent = 0);
    ~StickyNote();
    
    qint64 id() { return mId; }
    
    bool hasOwnFont() { return mHasOwnFont; }

public slots:
    void changeFont();
    void setColors(const QString &backGround, const QString &foreground);
    void setFont(const QFont &font);
    void requestDelete();
    void savePosition();
    void saveText();

signals:
    void deleteRequested(qint64 id);

protected:
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void load();
    
    QString dataDir(); // GenericDataLocation

    qint64 mId;
    
    QPoint prevPos;
    bool prevPosSet;
    
    bool mHasOwnFont;
    
    Ui::StickyNote *ui;
};


#endif
