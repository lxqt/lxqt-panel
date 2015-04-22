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

#ifndef LXQTTASKGROUP_H
#define LXQTTASKGROUP_H

#include "../panel/ilxqtpanel.h"
#include "../panel/ilxqtpanelplugin.h"
#include "lxqttaskbar.h"
#include "lxqtgrouppopup.h"
#include "lxqttaskbutton.h"
#include <KF5/KWindowSystem/kwindowsystem.h>

class QVBoxLayout;
class ILxQtPanelPlugin;

class LxQtGroupPopup;
class LxQtMasterPopup;

class LxQtTaskGroup: public LxQtTaskButton
{
    Q_OBJECT

public:
    LxQtTaskGroup(const QString & groupName, QIcon icon ,ILxQtPanelPlugin * plugin, LxQtTaskBar * parent);

    QString groupName() const { return mGroupName; }

    void removeButton(WId window);
    int buttonsCount() const;
    int visibleButtonsCount() const;

    LxQtTaskButton * addWindow(WId id);
    LxQtTaskButton * checkedButton() const;

    // Returns the next or the previous button in the popup
    // if circular is true, then it will go around the list of buttons
    LxQtTaskButton * getNextPrevChildButton(bool next, bool circular);

    bool onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2);
    void refreshIconsGeometry();
    void showOnlySettingChanged();
    void setAutoRotation(bool value, ILxQtPanel::Position position);
    void setToolButtonsStyle(Qt::ToolButtonStyle style);

    void setPopupVisible(bool visible = true, bool fast = false);

    static QString mimeDataFormat() { return QLatin1String("lxqt/lxqttaskgroup"); }

protected:
    QMimeData * mimeData();

    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void dragEnterEvent(QDragEnterEvent * event);
    void dragLeaveEvent(QDragLeaveEvent * event);
    void contextMenuEvent(QContextMenuEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    int recalculateFrameHeight() const;
    int recalculateFrameWidth() const;

    void draggingTimerTimeout();

private slots:
    void onClicked(bool checked);
    void onChildButtonClicked();
    void onActiveWindowChanged(WId window);
    void onWindowRemoved(WId window);
    void onDesktopChanged(int number);

    void closeGroup();

signals:
    void groupBecomeEmpty(QString name);
    void visibilityChanged(bool visible);
    void popupShown(LxQtTaskGroup* sender);
    void windowDisowned(WId window);

private:
    QString mGroupName;
    LxQtGroupPopup * mPopup;
    LxQtTaskButtonHash mButtonHash;
    ILxQtPanelPlugin * mPlugin;
    bool mPreventPopup;

    QSize recalculateFrameSize();
    QPoint recalculateFramePosition();
    void recalculateFrameIfVisible();
    void refreshVisibility();
    void regroup();
};

#endif // LXQTTASKGROUP_H
