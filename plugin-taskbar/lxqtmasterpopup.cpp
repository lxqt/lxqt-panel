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

#include "lxqtmasterpopup.h"
#include "lxqtgrouppopup.h"
#include "lxqttaskbutton.h"
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QLayout>
#include "lxqttaskbar.h"
#include <QStackedWidget>
#include <QDebug>
#include <QTimer>
#include "../panel/ilxqtpanelplugin.h"
#include <QPropertyAnimation>

/************************************************
    main purpose of this class is switching
    between group menus and hiding itself
    when mouse is not present
 ************************************************/
LxQtMasterPopup::LxQtMasterPopup(LxQtTaskBar *parent):
    QFrame(parent),
    mStackedWidget(new QStackedWidget(this)),
    mCloseTimer(new QTimer(this)),
    mFlags(0)
{
    mPosAnimation = new QPropertyAnimation(this,"pos",this);
    mPosAnimation->setDuration(200);

    mSizeAnimation = new QPropertyAnimation(this,"size",this);
    mSizeAnimation->setDuration(200);

    setWindowFlags(  Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    QVBoxLayout * l = new QVBoxLayout(this);
    l->addWidget(mStackedWidget);
    l->setSpacing(0);
    l->setMargin(0);

    connect(mCloseTimer,SIGNAL(timeout()),this,SLOT(close()));
    mCloseTimer->setSingleShot(true);
    mCloseTimer->setInterval(400);
}


void LxQtMasterPopup::moveAnimated(const QPoint &newpos)
{
    if (isVisible() && parentTaskBar()->settings().eyeCandy)
    {
        mPosAnimation->setStartValue(pos());
        mPosAnimation->setEndValue(newpos);
        mPosAnimation->start();
    }
    else
    {
        move(newpos);
    }
}

void LxQtMasterPopup::resizeAnimated(const QSize &newsize)
{
    if (isVisible() && parentTaskBar()->settings().eyeCandy)
    {
        mSizeAnimation->stop();
        mSizeAnimation->setStartValue(size());
        mSizeAnimation->setEndValue(newsize);
        mSizeAnimation->start();
    }
    else
    {
        resize(newsize);
    }
}

void LxQtMasterPopup::showEvent(QShowEvent *event)
{
    if (parentTaskBar()->settings().eyeCandy)
    {
        disconnect(mSizeAnimation,SIGNAL(finished()),this,SLOT(hide()));
        mSizeAnimation->stop();
        QSize s = size();
        mSizeAnimation->setStartValue(QSize(0,0));
        mSizeAnimation->setEndValue(s);
        mSizeAnimation->start();
    }
}

void LxQtMasterPopup::closeEvent(QCloseEvent *event)
{
    if (parentTaskBar()->settings().eyeCandy)
    {
        mSizeAnimation->stop();
        QSize s = size();
        mSizeAnimation->setEndValue(QSize(0,0));
        mSizeAnimation->setStartValue(s);
        mSizeAnimation->start();
        connect(mSizeAnimation,SIGNAL(finished()),this,SLOT(hide()),Qt::UniqueConnection);

        event->ignore();
    }
    //hide();
}

/************************************************
    if mouse is not present neither on group button
    nor frame closing timer is activated
 ************************************************/
void LxQtMasterPopup::checkTimer()
{
    if (mFlags)
        mCloseTimer->stop();
    else
        mCloseTimer->start();

}

/************************************************

 ************************************************/
bool LxQtMasterPopup::mouseEnterCurrentGroup(LxQtTaskGroup * group,  bool enter)
{
    //mouse on current group button
    bool is = mStackedWidget->currentWidget() == mGroupHash.value(group);
    if (is)
        setFlag(MOUSE_ON_CURRENT_GROUP, enter);
    checkTimer();

    return is;
}

void LxQtMasterPopup::setFlag(noCloseFlags_t flag, bool set)
{
    if (set)
        mFlags |= flag;
    else
        mFlags &= ~flag;
}

/************************************************

 ************************************************/
void LxQtMasterPopup::leaveEvent(QEvent *event)
{
    mFlags &= ~MOUSE_ON_FRAME;
    checkTimer();
}

/************************************************

 ************************************************/
void LxQtMasterPopup::enterEvent(QEvent *event)
{
    mFlags |= MOUSE_ON_FRAME;
    checkTimer();
}

/************************************************
    switch the current widget in stackwidget
 ************************************************/
void LxQtMasterPopup::activateGroup(LxQtTaskGroup *group, bool show)
{
    if (!show)
    {
        close();
        return;
    }

    QFrame * f = mGroupHash.value(group);
    if(f)
    {
        mStackedWidget->setCurrentWidget(f);
        f->show();
        //resize(f->minimumSize());
        this->show();
    }
}

/************************************************
    create new frame according to group
 ************************************************/
LxQtGroupPopup * LxQtMasterPopup::createFrame(LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons)
{
    LxQtGroupPopup * f = mGroupHash.value(group,NULL);

    if (!f)
    {
        f = new LxQtGroupPopup(this,group,  buttons);
        mStackedWidget->addWidget(f);
        connect(group,SIGNAL(destroyed(QObject *)),this,SLOT(onGroupDestroyed(QObject *)));
        mGroupHash.insert(group,f);
    }

    return f;
}

/************************************************
    when group destroyed clean up in stackwidget
    and hash
 ************************************************/
void LxQtMasterPopup::onGroupDestroyed(QObject * a)
{
    //why does qobject_cast return zero?
    //LxQtTaskGroup * group = qobject_cast<LxQtTaskGroup*>(a);
    LxQtTaskGroup * group = (LxQtTaskGroup*) a;
    Q_ASSERT(group);

    if (mGroupHash.contains(group))
    {
        LxQtGroupPopup * frame = mGroupHash.value(group);
        mStackedWidget->removeWidget(frame);
        mGroupHash.remove(group);
        close();

        delete frame;
    }
}

/************************************************
    "singleton" instance for each taskbar plugin
 ************************************************/
LxQtMasterPopup * LxQtMasterPopup::instance(LxQtTaskBar *parent)
{
    if(!parent->mMasterPopup)
    {
        parent->mMasterPopup = new LxQtMasterPopup(parent);
    }

    return parent->mMasterPopup;
}

/************************************************
     helper function
 ************************************************/
LxQtTaskBar * LxQtMasterPopup::parentTaskBar()
{
    return qobject_cast<LxQtTaskBar *>(parent());
}

