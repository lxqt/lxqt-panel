/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
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

#include <QFrame>
#include <QBoxLayout>
#include <QMap>

#include "../panel/ilxqtpanel.h"

class ILXQtPanel;
class ILXQtPanelPlugin;

class QSignalMapper;

class LXQtTaskGroup;

class LeftAlignedTextStyle;

class ILXQtAbstractWMInterface;

namespace LXQt {
class GridLayout;
}

namespace GlobalKeyShortcut
{
class Action;
}

class LXQtTaskBar : public QFrame
{
    Q_OBJECT

public:
    explicit LXQtTaskBar(ILXQtPanelPlugin *plugin, QWidget* parent = nullptr);
    virtual ~LXQtTaskBar();

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
    bool isIconByClass() const { return mIconByClass; }
    int wheelEventsAction() const { return mWheelEventsAction; }
    int wheelDeltaThreshold() const { return mWheelDeltaThreshold; }

    ILXQtPanel * panel() const;
    inline ILXQtPanelPlugin * plugin() const { return mPlugin; }

    inline ILXQtAbstractWMInterface *getBackend() const { return mBackend; }

public slots:
    void settingsChanged();

signals:
    void buttonRotationRefreshed(bool autoRotate, ILXQtPanel::Position position);
    void buttonStyleRefreshed(Qt::ToolButtonStyle buttonStyle);
    void refreshIconGeometry();
    void showOnlySettingChanged();
    void iconByClassChanged();
    void popupShown(LXQtTaskGroup* sender);

protected:
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dragMoveEvent(QDragMoveEvent * event);

private slots:
    void refreshButtonRotation();
    void refreshPlaceholderVisibility();
    void groupBecomeEmptySlot();

    void onWindowChanged(WId window, int prop);
    void onWindowAdded(WId window);
    void onWindowRemoved(WId window);

    void registerShortcuts();
    void shortcutRegistered();
    void activateTask(int pos);

private:
    typedef QMap<WId, LXQtTaskGroup*> windowMap_t;

private:
    void addWindow(WId window);
    windowMap_t::iterator removeWindow(windowMap_t::iterator pos);
    void buttonMove(LXQtTaskGroup * dst, LXQtTaskGroup * src, QPoint const & pos);

private:
    QMap<WId, LXQtTaskGroup*> mKnownWindows; //!< Ids of known windows (mapping to buttons/groups)
    LXQt::GridLayout *mLayout;
    QList<GlobalKeyShortcut::Action*> mKeys;
    QSignalMapper *mSignalMapper;

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
    bool mUngroupedNextToExisting;
    bool mIconByClass;
    int mWheelEventsAction;
    int mWheelDeltaThreshold;

    void setButtonStyle(Qt::ToolButtonStyle buttonStyle);

    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
    void resizeEvent(QResizeEvent *event);

    ILXQtPanelPlugin *mPlugin;
    QWidget *mPlaceHolder;
    LeftAlignedTextStyle *mStyle;

    ILXQtAbstractWMInterface *mBackend;
};

#endif // LXQTTASKBAR_H
