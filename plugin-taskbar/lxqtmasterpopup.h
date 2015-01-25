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

#ifndef LXQTMASTERPOPUP_H
#define LXQTMASTERPOPUP_H

#include <QFrame>

class QStackedWidget;
class LxQtTaskBar;
class LxQtTaskGroup;
class LxQtTaskButton;
class LxQtGroupPopup;
class ILxQtPanelPlugin;
class QPropertyAnimation;

class LxQtMasterPopup: public QFrame
{
    Q_OBJECT
public:
    static LxQtMasterPopup * instance(LxQtTaskBar * parent);
    LxQtGroupPopup * createFrame(LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons);
    LxQtTaskBar * parentTaskBar();
    void activateGroup(LxQtTaskGroup * group, bool show);

    void mouseEnterAnyGroup(bool enter) {setFlag(MOUSE_ON_ANY_GROUP,enter); checkTimer();}
    bool mouseEnterCurrentGroup(LxQtTaskGroup * group,bool enter);

    void moveAnimated(const QPoint & newpos);
    void resizeAnimated(const QSize & newsize);

    typedef enum {
        MOUSE_ON_FRAME = 1,
        MOUSE_ON_CURRENT_GROUP = 2,
        DRAGGING_BUTTON_ACTIVE = 4,
        MOUSE_ON_ANY_GROUP = 8
    } close_flags_enum;
    Q_DECLARE_FLAGS(noCloseFlags_t,close_flags_enum)

public slots:
    void dragging(bool executing) {setFlag(DRAGGING_BUTTON_ACTIVE, executing); checkTimer();}

protected:
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void showEvent(QShowEvent * event) ;
    void closeEvent(QCloseEvent * event);

private:
    QStackedWidget * mStackedWidget;
    QHash<LxQtTaskGroup *, LxQtGroupPopup *> mGroupHash;
    QTimer * mCloseTimer;
    noCloseFlags_t mFlags;

    void checkTimer();
    LxQtMasterPopup(LxQtTaskBar * parent);
    QPropertyAnimation * mPosAnimation;
    QPropertyAnimation * mSizeAnimation;
    void setFlag(noCloseFlags_t flag , bool set = true);

private slots:
    void onGroupDestroyed(QObject *);
};


Q_DECLARE_OPERATORS_FOR_FLAGS(LxQtMasterPopup::noCloseFlags_t)

#endif // LXQTMASTERPOPUP_H
