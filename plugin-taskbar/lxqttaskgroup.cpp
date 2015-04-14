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

#include "lxqttaskgroup.h"
#include "lxqttaskbar.h"

#include <QDebug>
#include <QMimeData>
#include <QFocusEvent>
#include <QDragLeaveEvent>
#include <QStringBuilder>
#include <QMenu>
#include <XdgIcon>
#include <KF5/KWindowSystem/KWindowSystem>

/************************************************

 ************************************************/
LxQtTaskGroup::LxQtTaskGroup(const QString &groupName, QIcon icon, ILxQtPanelPlugin * plugin, LxQtTaskBar *parent)
    : LxQtTaskButton(0, parent, parent),
    mGroupName(groupName),
    mPopup(new LxQtGroupPopup(this)),
    mPlugin(plugin),
    mPreventPopup(false)
{
    Q_ASSERT(parent);

    setObjectName(groupName);
    setText(groupName);
    setIcon(icon);

    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), this, SLOT(onDesktopChanged(int)));
    connect(KWindowSystem::self(), SIGNAL(windowRemoved(WId)), this, SLOT(onWindowRemoved(WId)));
    connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)), this, SLOT(onActiveWindowChanged(WId)));
    connect(KWindowSystem::self(), SIGNAL(windowChanged(WId, NET::Properties, NET::Properties2)),
            SLOT(onWindowChanged(WId, NET::Properties, NET::Properties2)));
}

/************************************************

 ************************************************/
void LxQtTaskGroup::contextMenuEvent(QContextMenuEvent *event)
{
    setPopupVisible(false, true);
    mPreventPopup = true;
    if (windowId())
    {
        LxQtTaskButton::contextMenuEvent(event);
        return;
    }

    QMenu menu(tr("Group"));
    QAction *a = menu.addAction(XdgIcon::fromTheme("process-stop"), tr("Close group"));
    connect(a, SIGNAL(triggered()), this, SLOT(closeGroup()));
    menu.exec(mapToGlobal(event->pos()));
    mPreventPopup = false;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::closeGroup()
{
    foreach (LxQtTaskButton * button, mButtonHash.values())
        if (button->isOnDesktop(KWindowSystem::currentDesktop()))
            button->closeApplication();
}

/************************************************

 ************************************************/
LxQtTaskButton * LxQtTaskGroup::addWindow(WId id)
{
    if (mButtonHash.contains(id))
        return mButtonHash.value(id);

    LxQtTaskButton *btn = new LxQtTaskButton(id, parentTaskBar(), mPopup);

    if (btn->isApplicationActive())
    {
        btn->setChecked(true);
        setChecked(true);
    }

    btn->setParentGroup(this);

    mButtonHash.insert(id, btn);
    mPopup->addButton(btn);

    connect(btn, SIGNAL(clicked()), this, SLOT(onChildButtonClicked()));
    refreshVisibility();

    return btn;
}

/************************************************

 ************************************************/
LxQtTaskButton * LxQtTaskGroup::checkedButton() const
{
    foreach (LxQtTaskButton* button, mButtonHash.values())
        if (button->isChecked())
            return button;

    return NULL;
}

/************************************************

 ************************************************/
LxQtTaskButton * LxQtTaskGroup::getNextPrevChildButton(bool next, bool circular)
{
    LxQtTaskButton *button = checkedButton();
    int idx = mPopup->indexOf(button);
    int inc = next ? 1 : -1;
    idx += inc;

    // if there is no cheked button, get the first one if next equals true
    // or the last one if not
    if (!button)
    {
        idx = -1;
        if (next)
        {
            for (int i = 0; i < mPopup->count() && idx == -1; i++)
                if (mPopup->itemAt(i)->widget()->isVisibleTo(mPopup))
                    idx = i;
        }
        else
        {
            for (int i = mPopup->count() - 1; i >= 0 && idx == -1; i--)
                if (mPopup->itemAt(i)->widget()->isVisibleTo(mPopup))
                    idx = i;
        }
    }

    if (circular)
        idx = (idx + mButtonHash.count()) % mButtonHash.count();
    else if (mPopup->count() <= idx || idx < 0)
        return NULL;

    // return the next or the previous child
    QLayoutItem *item = mPopup->itemAt(idx);
    if (item)
    {
        button = qobject_cast<LxQtTaskButton*>(item->widget());
        if (button->isVisibleTo(mPopup))
            return button;
    }

    return NULL;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onActiveWindowChanged(WId window)
{
    LxQtTaskButton *button = mButtonHash.value(window, NULL);
    foreach (LxQtTaskButton *btn, mButtonHash.values())
        btn->setChecked(false);

    if (button)
    {
        button->setChecked(true);
        if (button->hasUrgencyHint())
            button->setUrgencyHint(false);
    }
    setChecked(!!button);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onDesktopChanged(int number)
{
    refreshVisibility();
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onWindowRemoved(WId window)
{
    if (mButtonHash.contains(window))
    {
        LxQtTaskButton *button = mButtonHash.value(window);
        mButtonHash.remove(window);
        mPopup->removeWidget(button);
        button->deleteLater();

        if (mButtonHash.count())
            regroup();
        else
        {
            if (isVisible())
                emit visibilityChanged(false);
            hide();
            emit groupBecomeEmpty(groupName());

        }
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onChildButtonClicked()
{
    setPopupVisible(false, true);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::setToolButtonsStyle(Qt::ToolButtonStyle style)
{
    setToolButtonStyle(style);
}

/************************************************

 ************************************************/
int LxQtTaskGroup::buttonsCount() const
{
    return mButtonHash.count();
}

/************************************************

 ************************************************/
int LxQtTaskGroup::visibleButtonsCount() const
{
    int i = 0;
    foreach (LxQtTaskButton *btn, mButtonHash.values())
        if (btn->isVisibleTo(mPopup))
            i++;
    return i;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::draggingTimerTimeout()
{
    if (windowId())
        setPopupVisible(false);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onClicked(bool)
{
    if (visibleButtonsCount() > 1)
    {
        setChecked(mButtonHash.contains(KWindowSystem::activeWindow()));
        setPopupVisible(true);
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::regroup()
{
    int cont = visibleButtonsCount();
    recalculateFrameIfVisible();

    if (cont == 1)
    {
        // Get first visible button
        LxQtTaskButton * button = NULL;
        foreach (LxQtTaskButton *btn, mButtonHash.values())
        {
            if (btn->isVisibleTo(mPopup))
            {
                button = btn;
                break;
            }
        }

        if (button)
        {
            setText(button->text());
            setToolTip(button->toolTip());
            setWindowId(button->windowId());
        }
    }
    else if (cont == 0)
        hide();
    else
    {
        QString t = QString("%1 - %2 windows").arg(mGroupName).arg(cont);
        setText(t);
        setToolTip(parentTaskBar()->isShowGroupOnHover() ? QStringLiteral() : t);
        setWindowId(0);
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::showOnAllDesktopSettingChanged()
{
    refreshVisibility();
}

/************************************************

 ************************************************/
void LxQtTaskGroup::recalculateFrameIfVisible()
{
    if (mPopup->isVisible())
    {
        recalculateFrameSize();
        if (mPlugin->panel()->position() == ILxQtPanel::PositionBottom)
            recalculateFramePosition();
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::setAutoRotation(bool value, ILxQtPanel::Position position)
{
    foreach (LxQtTaskButton *button, mButtonHash.values())
        button->setAutoRotation(false, position);

    LxQtTaskButton::setAutoRotation(value, position);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::refreshVisibility()
{
    foreach(LxQtTaskButton * btn, mButtonHash.values())
    {
        if (parentTaskBar()->isShowOnlyCurrentDesktopTasks())
            btn->setVisible(btn->isOnDesktop(KWindowSystem::currentDesktop()));
        else
            btn->setVisible(true);
    }

    bool is = isVisible();
    bool will = visibleButtonsCount();
    setVisible(will);
    regroup();

    if (is != will)
        emit visibilityChanged(will);
}

/************************************************

 ************************************************/
QMimeData * LxQtTaskGroup::mimeData()
{
    QMimeData *mimedata = new QMimeData;
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << groupName();
    mimedata->setData(mimeDataFormat(), byteArray);
    return mimedata;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::setPopupVisible(bool visible, bool fast)
{
    if (visible && !mPreventPopup && 0 == windowId())
    {
        if (!mPopup->isVisible())
        {
            // setup geometry
            recalculateFrameSize();
            recalculateFramePosition();
        }

        mPopup->show();
        emit popupShown(this);
    }
    else
        mPopup->hide(fast);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::refreshIconsGeometry()
{
    foreach(LxQtTaskButton *but, mButtonHash.values())
    {
        but->refreshIconGeometry();
        but->setIconSize(QSize(mPlugin->panel()->iconSize(), mPlugin->panel()->iconSize()));
    }

    if (windowId())
        refreshIconGeometry();
}

/************************************************

 ************************************************/
QSize LxQtTaskGroup::recalculateFrameSize()
{
    int height = recalculateFrameHeight();
    mPopup->setMaximumHeight(1000);
    mPopup->setMinimumHeight(0);

    int hh = recalculateFrameWidth();
    mPopup->setMaximumWidth(hh);
    mPopup->setMinimumWidth(0);

    QSize newSize(hh, height);
    mPopup->resize(newSize);

    return newSize;
}

/************************************************

 ************************************************/
int LxQtTaskGroup::recalculateFrameHeight() const
{
    int cont = visibleButtonsCount();
    int h = !mPlugin->panel()->isHorizontal() && parentTaskBar()->isAutoRotate() ? width() : height();
    return cont * h + (cont + 1) * mPopup->spacing();
}

/************************************************

 ************************************************/
int LxQtTaskGroup::recalculateFrameWidth() const
{
    // FIXME: 300?
    int minimum = 300;
    int hh = width();

    if (!mPlugin->panel()->isHorizontal() && !parentTaskBar()->isAutoRotate())
        hh = height();

    if (hh < minimum)
        hh = minimum;

    return hh;
}

/************************************************

 ************************************************/
QPoint LxQtTaskGroup::recalculateFramePosition()
{
    // Set position
    int x_offset = 0, y_offset = 0;
    switch (mPlugin->panel()->position())
    {
    case ILxQtPanel::PositionTop:
        y_offset += height();
        break;
    case ILxQtPanel::PositionBottom:
        y_offset = -recalculateFrameHeight();
        break;
    case ILxQtPanel::PositionLeft:
        x_offset += width();
        break;
    case ILxQtPanel::PositionRight:
        x_offset = -recalculateFrameWidth();
        break;
    }

    QPoint pos = mapToGlobal(QPoint(x_offset, y_offset));
    mPopup->move(pos);

    return pos;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::leaveEvent(QEvent *event)
{
    setPopupVisible(false);
    QToolButton::leaveEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::enterEvent(QEvent *event)
{
    QToolButton::enterEvent(event);

    if (sDraggging)
        return;

    if (parentTaskBar()->isShowGroupOnHover())
        setPopupVisible(true);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::dragEnterEvent(QDragEnterEvent *event)
{
    sDraggging = true;
    // only show the popup if we aren't dragging a taskgroup
    if (!event->mimeData()->hasFormat(mimeDataFormat()))
    {
        setPopupVisible(true);
        LxQtTaskButton::dragEnterEvent(event);
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::dragLeaveEvent(QDragLeaveEvent *event)
{
    // if draggind something into the taskgroup or the taskgroups' popup,
    // do not close the popup
    if (!sDraggging)
        setPopupVisible(false);
    else
        sDraggging = false;
    LxQtTaskButton::dragLeaveEvent(event);
}

void LxQtTaskGroup::mouseMoveEvent(QMouseEvent* event)
{
    // if dragging the taskgroup, do not show the popup
    setPopupVisible(false, true);
    LxQtTaskButton::mouseMoveEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{
    QVector<LxQtTaskButton *> buttons;
    buttons.append(mButtonHash.value(window, NULL));

    // If group contains only one window properties must be changed also on button group
    if (window == windowId())
        buttons.append(this);

    foreach (LxQtTaskButton * button, buttons)
    {
        if (!button)
            continue;

        // if class is changed the window won't belong to our group any more
        if (parentTaskBar()->isGroupingEnabled() && prop2.testFlag(NET::WM2WindowClass) && this != button)
        {
            KWindowInfo info(window, 0, NET::WM2WindowClass);
            if (info.windowClassClass() != mGroupName)
            {
                //remove this window from this group
                //Note: can't optimize case when there is only one window in this group
                //      because mGroupName is a hash key in taskbar
                emit windowDisowned(window);
                onWindowRemoved(window);
                continue;
            }
        }
        // window changed virtual desktop
        if (prop.testFlag(NET::WMDesktop))
        {
            if (parentTaskBar()->isShowOnlyCurrentDesktopTasks())
            {
                button->setHidden(button->isOnDesktop(KWindowSystem::currentDesktop()));
                refreshVisibility();
            }
        }

        if (prop.testFlag(NET::WMVisibleName) || prop.testFlag(NET::WMName))
            button->updateText();

        // FIXME: NET::WMIconGeometry is causing high CPU and memory usage
        if (prop.testFlag(NET::WMIcon) /*|| prop.testFlag(NET::WMIconGeometry)*/)
            button->updateIcon();

        if (prop.testFlag(NET::WMState))
            button->setUrgencyHint(KWindowInfo(window, NET::WMState).hasState(NET::DemandsAttention));
    }
}
