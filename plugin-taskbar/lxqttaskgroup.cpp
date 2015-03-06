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
#include <QVBoxLayout>
#include <QDebug>
#include "../panel/ilxqtpanel.h"
#include <QMimeData>
#include "../panel/ilxqtpanelplugin.h"
#include <QDialog>
#include <KF5/KWindowSystem/KWindowSystem>
#include <QFocusEvent>
#include "lxqttaskbar.h"
#include <QTimer>
#include <QDragLeaveEvent>
#include <QMenu>
#include <XdgIcon>
#include "lxqtgrouppopup.h"
#include "lxqtmasterpopup.h"
#include <QSignalMapper>

/************************************************

 ************************************************/
LxQtTaskGroup::LxQtTaskGroup(const QString &groupName,QIcon icon,ILxQtPanelPlugin * plugin, LxQtTaskBar *parent):
    LxQtTaskButton(0,parent,parent),
    mGroupName(groupName),
    mFrame(LxQtMasterPopup::instance(parent)->createFrame(this,mButtonHash)),
    mLayout(new QVBoxLayout()),
    mPlugin(plugin),
    mSwitchTimer(new QTimer(this)),
    mPreventPopup(false)
{
    Q_ASSERT(parent);

    setText(groupName);
    setIcon(icon);

    mFrame->setLayout(mLayout);

    mLayout->setSpacing(3);
    mLayout->setMargin(3);

    connect(this,SIGNAL(clicked(bool)),this,SLOT(onClicked(bool)));
    connect(KWindowSystem::self(),SIGNAL(activeWindowChanged(WId)),this,SLOT(onActiveWindowChanged(WId)));
    connect(KWindowSystem::self(),SIGNAL(windowRemoved(WId)),this,SLOT(onWindowRemoved(WId)));
    connect(KWindowSystem::self(),SIGNAL(currentDesktopChanged(int)),this,SLOT(onDesktopChanged(int)));
    connect(KWindowSystem::self(), SIGNAL(windowChanged(WId, NET::Properties, NET::Properties2)),
            SLOT(windowChanged(WId, NET::Properties, NET::Properties2)));

    mSwitchTimer->setSingleShot(true);
    mSwitchTimer->setInterval(300);
    connect(mSwitchTimer,SIGNAL(timeout()),this,SLOT(raisePopup()));

    setObjectName(groupName);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::contextMenuEvent(QContextMenuEvent *event)
{
    raisePopup(false);
    mPreventPopup = true;
    if (windowId())
    {
        LxQtTaskButton::contextMenuEvent(event);
        return;
    }

    QMenu menu(tr("Group"));
    QSignalMapper mapper;
    QAction * a;
    a = menu.addAction(XdgIcon::fromTheme("process-stop"), tr("Close current desktop windows"));
    connect(a,SIGNAL(triggered()),&mapper,SLOT(map()));
    mapper.setMapping(a,1);

    menu.addSeparator();
    a = menu.addAction(XdgIcon::fromTheme("process-stop"), tr("Close whole group"));
    mapper.setMapping(a,0);
    connect(a,SIGNAL(triggered()),&mapper,SLOT(map()));

    connect(&mapper,SIGNAL(mapped(int)),SLOT(closeGroup(int)));
    menu.exec(mapToGlobal(event->pos()));
    mPreventPopup = false;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::closeGroup(int id)
{
    foreach (LxQtTaskButton * button, mButtonHash)
    {
        if (id == 0 || button->desktopNum() == KWindowSystem::currentDesktop())
            button->closeApplication();
    }
}

/************************************************

 ************************************************/
LxQtTaskButton * LxQtTaskGroup::createButton(WId id)
{
    if (mButtonHash.contains(id))
        return mButtonHash.value(id);

    LxQtTaskButton * btn = new LxQtTaskButton(id,parentTaskBar(),mFrame);

    if (btn->isApplicationActive())
    {
        btn->setChecked(true);
        setChecked(true);
    }

    btn->setParentGroup(this);

    mButtonHash.insert(id,btn);
    mFrame->layout()->addWidget(btn);


    connect(btn,SIGNAL(clicked()),this,SLOT(onChildButtonClicked()));
    connect(btn,SIGNAL(dropped(QPoint,QDropEvent*)),mFrame,SLOT(buttonDropped(QPoint,QDropEvent*)));
    connect(btn,SIGNAL(dragging(bool)),popup(),SLOT(dragging(bool)));

    refreshVisibility();
    regroup();

    return btn;
}

/************************************************

 ************************************************/
LxQtTaskButton * LxQtTaskGroup::checkedButton() const
{
    foreach (LxQtTaskButton* button, mButtonHash)
    {
        if (button->isChecked())
        {
            return button;
        }
    }

    return NULL;
}

/************************************************

 ************************************************/
bool LxQtTaskGroup::checkNextPrevChild(bool next,bool modulo)
{
    int idx = mLayout->indexOf(checkedButton());
    int inc;
    if (next)
        inc = 1;
    else
        inc = -1;

    idx += inc;

    if (!checkedButton())
    {
        if (next)
        {
            idx = 0;
        }
        else
        {
            for(int i = mLayout->count() - 1; i >= 0; i--)
            {
                if(mLayout->itemAt(i)->widget()->isVisibleTo(mFrame))
                {
                    idx = i;
                    break;
                }
            }
        }
    }

    while(true)
    {
        if (!modulo)
        {
            if (mLayout->count() <= idx || idx < 0)
                return false;
        }
        else
        {
            idx = (idx + mButtonHash.count()) % mButtonHash.count();
        }

        QWidget * w = mLayout->itemAt(idx)->widget();
        LxQtTaskButton * button = qobject_cast<LxQtTaskButton*>(w);
        if (button->isVisibleTo(mFrame))
        {
            button->raiseApplication();
            return true;
        }
        idx += inc;
    }
    return false;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onActiveWindowChanged(WId window)
{
    bool contains = mButtonHash.contains(window);
    foreach(LxQtTaskButton * btn, mButtonHash)
    {
        btn->setChecked(false);
    }

    if (contains)
    {
        LxQtTaskButton * btn = mButtonHash.value(window);
        btn->setChecked(true);
        if (btn->hasUrgencyHint())
            btn->setUrgencyHint(false);
    }
    setChecked(contains);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onDesktopChanged(int number)
{
    refreshVisibility();
    regroup();
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onWindowRemoved(WId window)
{
    removeButton(window);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onChildButtonClicked()
{
    raisePopup(false);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::removeButton(WId window)
{
    if (mButtonHash.contains(window))
    {
        LxQtTaskButton * button = mButtonHash.value(window);
        mButtonHash.remove(window);
        mFrame->layout()->removeWidget(button);

        delete button;

        if (mButtonHash.count())
        {
            regroup();
        }
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
int LxQtTaskGroup::visibleButtonsCount(LxQtTaskButton ** first) const
{
    int i = 0;
    if (first)
        *first = NULL;

    foreach(LxQtTaskButton * btn, mButtonHash.values())
    {

        if (btn->isVisibleTo(mFrame))
        {
            i++;
            if (first && !*first)
                *first = btn;
        }
    }

    return i;
}

/************************************************

 ************************************************/
void LxQtTaskGroup::draggingTimerTimeout()
{
    if (windowId())
    {
        raisePopup(false);
        LxQtTaskButton::draggingTimerTimeout();
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onClicked(bool )
{
    if (visibleButtonsCount() > 1)
    {
        setChecked(mButtonHash.contains(KWindowSystem::activeWindow()));
        if (mFrame->isVisible())
        {
            raisePopup(false);
            return;
        }
        raisePopup(true);
        startStopFrameCloseTimer(false);
    }
    else
    {
        raisePopup(false);
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::regroup()
{
    LxQtTaskButton * btn;
    int cont = visibleButtonsCount(&btn);

    recalculateFrameIfVisible();

    if (cont == 1)
    {
        setText(btn->text());
        setWindowId(btn->windowId());
    }
    else if (cont == 0)
    {
        hide();
    }
    else
    {
        setText(mGroupName + QString(" - %1 ").arg(cont) + tr("Windows"));
        setWindowId(0);
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::showOnAllDesktopSettingChanged()
{
    refreshVisibility();
    regroup();
}

/************************************************

 ************************************************/
void LxQtTaskGroup::recalculateFrameIfVisible()
{
    if (mFrame->isVisible())
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

    foreach (LxQtTaskButton * button, mButtonHash)
    {
        button->setAutoRotation(false,position);
    }

    LxQtTaskButton::setAutoRotation(value,position);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::refreshVisibility()
{
    foreach(LxQtTaskButton * btn, mButtonHash)
    {
        if (parentTaskBar()->isShowOnlyCurrentDesktopTasks())
            btn->setVisible(btn->desktopNum() == KWindowSystem::currentDesktop());
        else
            btn->setVisible(true);
    }

    bool is = isVisible();
    bool will = visibleButtonsCount();
    setVisible(will);

    if (is != will)
        emit visibilityChanged(will);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::arbitraryMimeData(QMimeData *mimedata)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    qDebug() << QString("Dragging group button: %1").arg(groupName());
    stream << groupName();
    mimedata->setData(taskGroupMimeDataFormat(), byteArray);

    if (windowId())
        LxQtTaskButton::arbitraryMimeData(mimedata);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::raisePopup(bool raise)
{
    if (raise && !mPreventPopup && 0 == windowId())
    {
        // setup geometry
        recalculateFrameSize();
        recalculateFramePosition();

        popup()->activateGroup(this, true);
    }
    else
        popup()->activateGroup(this, false);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::refreshIconsGeometry()
{
    foreach(LxQtTaskButton * but, mButtonHash)
    {
        but->refreshIconGeometry();
        but->setIconSize(QSize(mPlugin->panel()->iconSize(),mPlugin->panel()->iconSize()));
    }

    if (windowId())
        refreshIconGeometry();
}

/************************************************

 ************************************************/
QSize LxQtTaskGroup::recalculateFrameSize()
{
    LxQtMasterPopup * p = popup();
    int height = recalculateFrameHeight();
    p->setMaximumHeight(1000);
    p->setMinimumHeight(0);

    int hh = recalculateFrameWidth();
    p->setMaximumWidth(hh);
    p->setMinimumWidth(0);

    p->resize(QSize(hh,height));

    return QSize(hh,height);
}

/************************************************

 ************************************************/
int LxQtTaskGroup::recalculateFrameHeight() const
{
    int cont = visibleButtonsCount();
    return cont * height() + (cont + 1) * mLayout->spacing();
}

/************************************************

 ************************************************/
int LxQtTaskGroup::recalculateFrameWidth() const
{
    int minimum = parentTaskBar()->buttonWidth();
    int hh = width();

    if (!mPlugin->panel()->isHorizontal())
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
    LxQtMasterPopup * p = popup();
    int x_offset = 0, y_offset = 0;
    int rows = mPlugin->panel()->lineCount();
    switch (mPlugin->panel()->position())
    {
    case ILxQtPanel::PositionBottom:
        y_offset = -recalculateFrameHeight()  - 5 ; break;
    case ILxQtPanel::PositionTop:
        y_offset = mPlugin->panel()->globalGometry().height() / rows + 5; break;
    case ILxQtPanel::PositionLeft:
        x_offset = mPlugin->panel()->globalGometry().width() + 5; break;
    case ILxQtPanel::PositionRight:
        x_offset = -recalculateFrameWidth() - 5;
        break;
    }

    int x, y;
    x = parentWidget()->mapToGlobal(pos()).x() + x_offset ;
    y = parentWidget()->mapToGlobal(pos()).y() + y_offset;

    p->move(QPoint(x,y));

    return QPoint(x,y);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::startStopFrameCloseTimer(bool start)
{
    LxQtMasterPopup * p = popup();
    if (parentTaskBar()->isShowGroupOnHover())
        p->mouseEnterAnyGroup(!start);
    else
        p->mouseEnterCurrentGroup(this, !start);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::leaveEvent(QEvent *event)
{
    startStopFrameCloseTimer(true);
    mSwitchTimer->stop();
    QToolButton::leaveEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::enterEvent(QEvent *event)
{
    QToolButton::enterEvent(event);

    if (sDraggging)
        return;

    startStopFrameCloseTimer(false);

    if (parentTaskBar()->isShowGroupOnHover())
    {
        int t;
        popup()->isVisible() ? t = 200 : t = 400;
        mSwitchTimer->start(t);
        return;
    }

    if (popup()->isVisible())
        mSwitchTimer->start(100);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::dragEnterEvent(QDragEnterEvent *event)
{
    mSwitchTimer->stop();
    raisePopup(false);
    LxQtTaskButton::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::dragLeaveEvent(QDragLeaveEvent *event)
{
    raisePopup(false);
    mSwitchTimer->stop();
    LxQtTaskButton::dragLeaveEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::windowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{
    QVector<LxQtTaskButton *> buttons;
    buttons.append(mButtonHash.value(window,NULL));

    // If group contains only one window properties must be changed also on groupbutton
    if (window == windowId())
        buttons.append(this);

    foreach (LxQtTaskButton * button, buttons)
    {
        if (!button)
            continue;

        // window changed virtual desktop
        if (prop.testFlag(NET::WMDesktop))
        {
            if (parentTaskBar()->isShowOnlyCurrentDesktopTasks())
            {
                int desktop = button->desktopNum();
                button->setHidden(desktop != NET::OnAllDesktops && desktop != KWindowSystem::currentDesktop());
                refreshVisibility();
                regroup();
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

LxQtMasterPopup * LxQtTaskGroup::popup()
{
    return LxQtMasterPopup::instance(parentTaskBar());
}
