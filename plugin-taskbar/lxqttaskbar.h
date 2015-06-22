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


#ifndef LXQTTASKBAR_H
#define LXQTTASKBAR_H

#include "../panel/ilxqtpanel.h"
#include "../panel/ilxqtpanelplugin.h"
#include "lxqttaskbarconfiguration.h"
#include "lxqttaskgroup.h"
#include "lxqttaskbutton.h"

#include <QFrame>
#include <QBoxLayout>
#include <QHash>
#include "../panel/ilxqtpanel.h"
#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/KWindowInfo>
#include <KWindowSystem/NETWM>

class LxQtTaskButton;
class ElidedButtonStyle;
class ILxQtPanelPlugin;

namespace LxQt {
class GridLayout;
}

class LxQtTaskBar : public QFrame
{
    Q_OBJECT

public:
    explicit LxQtTaskBar(ILxQtPanelPlugin *plugin, QWidget* parent = 0);
    virtual ~LxQtTaskBar();

    void realign();

    Qt::ToolButtonStyle buttonStyle() const { return mButtonStyle; }
    int buttonWidth() const { return mButtonWidth; }
    bool closeOnMiddleClick() const { return mCloseOnMiddleClick; }
    bool raiseOnCurrentDesktop() const { return mRaiseOnCurrentDesktop; }
    bool isShowOnlyOneDesktopTasks() const { return mShowOnlyOneDesktopTasks; }
    int showDesktopNum() const { return mShowDesktopNum; }
    bool isShowOnlyCurrentScreenTasks() const { return mShowOnlyCurrentScreenTasks; }
    bool isShowOnlyMinimizedTasks() const { return mShowOnlyMinimizedTasks; }
    bool isAutoRotate() const { return mAutoRotate; }
    bool isGroupingEnabled() const { return mGroupingEnabled; }
    bool isShowGroupOnHover() const { return mShowGroupOnHover; }

public slots:
    void settingsChanged();

protected:
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent * event);

private slots:
    void refreshIconGeometry();
    void refreshTaskList();
    void refreshButtonRotation();
    void refreshPlaceholderVisibility();
    void groupBecomeEmptySlot();
    void groupPopupShown(LxQtTaskGroup * const sender);
    void onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2);

private:
    void addWindow(WId window, QString const & groupId);

private:
    QHash<QString, LxQtTaskGroup*> mGroupsHash;
    LxQt::GridLayout *mLayout;

    // Settings
    Qt::ToolButtonStyle mButtonStyle;
    int mButtonWidth;
    int mButtonHeight;
    bool mCloseOnMiddleClick;
    bool mRaiseOnCurrentDesktop;
    bool mShowOnlyOneDesktopTasks;
    int mShowDesktopNum;
    bool mShowOnlyCurrentScreenTasks;
    bool mShowOnlyMinimizedTasks;
    bool mAutoRotate;
    bool mGroupingEnabled;
    bool mShowGroupOnHover;

    bool acceptWindow(WId window) const;
    void setButtonStyle(Qt::ToolButtonStyle buttonStyle);

    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
    void resizeEvent(QResizeEvent *event);

    ILxQtPanelPlugin *mPlugin;
    QWidget *mPlaceHolder;
    LeftAlignedTextStyle *mStyle;
};

#endif // LXQTTASKBAR_H
