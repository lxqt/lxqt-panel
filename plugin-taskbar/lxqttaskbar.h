/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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
#include <X11/Xlib.h>
#include "../panel/fixx11h.h"
#include "../panel/ilxqtpanel.h"

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

    virtual void x11EventFilter(XEventType* event);
    virtual void settingsChanged();

    void realign();

public slots:
    void activeWindowChanged();
    void refreshIconGeometry();

protected:
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent * event);

private:
    void refreshTaskList();
    void refreshButtonRotation();
    void refreshButtonVisibility();
    QHash<WId, LxQtTaskButton*> mButtonsHash;
    LxQt::GridLayout *mLayout;
    LxQtTaskButton* buttonByWindow(WId window) const;
    bool windowOnActiveDesktop(WId window) const;
    WId mRootWindow;
    Qt::ToolButtonStyle mButtonStyle;
    int mButtonWidth;

    LxQtTaskButton* mCheckedBtn;
    bool mCloseOnMiddleClick;
    void setButtonStyle(Qt::ToolButtonStyle buttonStyle);
    bool mShowOnlyCurrentDesktopTasks;
    bool mAutoRotate;

    void handlePropertyNotify(XEventType* event);
    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);
    void mousePressEvent(QMouseEvent *event);

    ILxQtPanelPlugin *mPlugin;
    QWidget *mPlaceHolder;
    ElidedButtonStyle* mStyle;
};

#endif // LXQTTASKBAR_H
