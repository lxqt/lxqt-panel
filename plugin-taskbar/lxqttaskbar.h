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


#ifndef LXQTTASKBAR_H
#define LXQTTASKBAR_H

#include "lxqttaskbarconfiguration.h"
#include <QFrame>
#include <QBoxLayout>
#include <QHash>
#include "../panel/ilxqtpanel.h"
#include <KF5/KWindowSystem/KWindowSystem>
#include <KF5/KWindowSystem/KWindowInfo>
#include <KF5/KWindowSystem/NETWM>

#include "lxqttaskgroup.h"
//#include "lxqttaskbutton.h"

class LxQtTaskButton;
class ElidedButtonStyle;
class ILxQtPanelPlugin;
//class LxQtMasterPopup;
#include "lxqtmasterpopup.h"

namespace LxQt {
class GridLayout;
}

class LxQtTaskBar : public QFrame
{
    Q_OBJECT

public:
    explicit LxQtTaskBar(ILxQtPanelPlugin *plugin, QWidget* parent = 0);
    virtual ~LxQtTaskBar();

    virtual void settingsChanged();
    void realign();

    typedef struct
    {
        bool closeOnMiddleClick;
        bool showOnlyCurrentDesktopTasks;
        bool autoRotate;
        bool enabledGrouping;
        int buttonWidth;
        Qt::ToolButtonStyle toolButtonStyle;
        bool eyeCandy;
        bool showGroupWhenHover;
        bool switchGroupWhenHover;
        //bool showGroupWhenHoverOneWindow;
        bool switchGroupWhenHoverOneWindow;
        int groupButtonWidth;
        int groupButtonHeight;
    } settings_t;

    const settings_t & settings() const {return mSettings;}


public slots:
    void refreshIconGeometry();

protected:
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent * event);

private slots:
    void refreshTaskList();
    void refreshButtonRotation();
    void refreshPlaceholderVisibility();
    void groupBecomeEmptySlot();
    void groupDroppedSlot(const QPoint & point, QDropEvent * event);

private:
    QHash<QString, LxQtTaskGroup*> mGroupsHash;
    LxQt::GridLayout *mLayout;
    settings_t mSettings;
    LxQtTaskGroup * mCheckedGroup;

    bool windowOnActiveDesktop(WId window) const;
    bool acceptWindow(WId window) const;
    void setButtonStyle(Qt::ToolButtonStyle buttonStyle);
    int dropValue(int idx);

    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
    void resizeEvent(QResizeEvent *event);


    ILxQtPanelPlugin *mPlugin;
    QWidget *mPlaceHolder;
    ElidedButtonStyle* mStyle;

public:
    //used for plugin single instance of LxQtMasterPopup
    LxQtMasterPopup * mMasterPopup;
};

#endif // LXQTTASKBAR_H
