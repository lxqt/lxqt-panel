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

/************************************************
    main purpose of this class is switching
    between group menus and hiding itself
    when mouse is not present
 ************************************************/
LxQtMasterPopup::LxQtMasterPopup(LxQtTaskBar *parent):
    QFrame(parent),
    mStackedWidget(new QStackedWidget(this)),
    mCloseTimer(new QTimer(this)),
    mMouseOnFrame(false),
    mMouseOnGroup(false)
{
    setWindowFlags(  Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    QVBoxLayout * l = new QVBoxLayout(this);
    l->addWidget(mStackedWidget);
    l->setSpacing(0);
    l->setMargin(0);

    connect(mCloseTimer,SIGNAL(timeout()),this,SLOT(hide()));
    mCloseTimer->setSingleShot(true);
    mCloseTimer->setInterval(400);
}

/************************************************
    if mouse is not present neither on group button
    nor frame closing timer is activated
 ************************************************/
void LxQtMasterPopup::checkTimer()
{
    if (mMouseOnFrame || mMouseOnGroup)
        mCloseTimer->stop();
    else
        mCloseTimer->start();
}

/************************************************

 ************************************************/
void LxQtMasterPopup::activateCloseTimer(bool activate)
{
    mMouseOnGroup = !activate;
    checkTimer();
}

/************************************************

 ************************************************/
void LxQtMasterPopup::leaveEvent(QEvent *event)
{
    mMouseOnFrame = false;
    checkTimer();
}

/************************************************

 ************************************************/
void LxQtMasterPopup::enterEvent(QEvent *event)
{
    mMouseOnFrame = true;
    checkTimer();
}

/************************************************
    switch the current widget in stackwidget
 ************************************************/
void LxQtMasterPopup::activateGroup(LxQtTaskGroup *group, bool show)
{
    if (!show)
    {
        hide();
        return;
    }

    QFrame * f = mGroupHash.value(group);
    if(f)
    {
        mStackedWidget->setCurrentWidget(f);
        f->show();
        resize(f->minimumSize());
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

