#include "lxqttaskpopup.h"
#include "lxqttaskbutton.h"
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QLayout>
#include "lxqttaskbar.h"

LxQtLooseFocusFrame::LxQtLooseFocusFrame(const QHash<WId,LxQtTaskButton* > & buttons,QWidget *parent):
    QDialog(parent)
{
    //setWindowFlags( Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup | Qt::X11BypassWindowManagerHint);
    setWindowFlags(  Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::ToolTip);
//    mFrame->setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    //setMouseTracking(true);
    setAcceptDrops(true);

    mPosAnimation = new QPropertyAnimation(this,"pos",this);
    mPosAnimation->setDuration(200);

    mSizeAnimation = new QPropertyAnimation(this,"size",this);
    mSizeAnimation->setDuration(200);
}
/************************************************

 ************************************************/
LxQtLooseFocusFrame::~LxQtLooseFocusFrame()
{
}

/************************************************

 ************************************************/
void LxQtLooseFocusFrame::enterEvent(QEvent *event)
{
    QEvent::Type t = event->type();
    if (t == QEvent::Enter)
    {
        mouseLeft(false);
    }

    QDialog::enterEvent(event);
}

/************************************************

 ************************************************/
void LxQtLooseFocusFrame::leaveEvent(QEvent *event)
{
    QEvent::Type t = event->type();
    if(t == QEvent::Leave)
    {
        emit mouseLeft(true);
    }

    QDialog::leaveEvent(event);
}

/************************************************

 ************************************************/
void LxQtLooseFocusFrame::dropEvent(QDropEvent *event)
{
    buttonDropped(event->pos(),event);
}

/************************************************

 ************************************************/
void LxQtLooseFocusFrame::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasFormat("lxqt/lxqttaskbutton"))
    {
        event->ignore();
        return;
    }
    if (mButtonHash.count() == 1)
    {
        event->ignore();
        return;
    }

    event->acceptProposedAction();

    //LxQtTaskButton::dragEnterEvent(event);
    QWidget::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LxQtLooseFocusFrame::buttonDropped(const  QPoint& point, QDropEvent *event)
{
    WId window;
    QDataStream stream(event->mimeData()->data("lxqt/lxqttaskbutton"));
    stream >> window;
    if (!mButtonHash.contains(window))
    {
        return;
    }

    LxQtTaskButton * dragged = mButtonHash.value(window);
    int droppedIndex = layout()->indexOf(dragged);
    int newIdx = -1;
    int temp;

    int oldTreshold = 0;
    for (int i = 0 ; i < layout()->count(); i++)
    {
        QWidget * w = layout()->itemAt(i)->widget();
        LxQtTaskButton * b = qobject_cast<LxQtTaskButton*>(w);
        if (b && w->isVisibleTo(this))
        {
            int treshold = b->pos().y() + b->height() ;
            if (oldTreshold <= point.y() && point.y() < treshold)
            {
                newIdx = i;
                break;
            }
            temp = i;
            oldTreshold = treshold;
        }
    }

    /*
    if (newIdx == -1)
        newIdx = temp+ 1;
        */

    QVBoxLayout * l = qobject_cast<QVBoxLayout *>(layout());
    l->insertWidget(newIdx,dragged);
}

void LxQtLooseFocusFrame::moveEyeCandy(const QPoint  & newPos)
{
    LxQtTaskGroup* group = qobject_cast<LxQtTaskGroup*>(parent());
    Q_ASSERT(group);
    bool eyecandy = group->parentTaskBar()->settings().eyeCandy;

    if (eyecandy)
    {
        mPosAnimation->stop();
        mPosAnimation->setStartValue(pos());
        mPosAnimation->setEndValue(newPos);
        mPosAnimation->start();
    }
    else
    {
        move(newPos);
    }
}

void LxQtLooseFocusFrame::resizeEyeCandy(int w, int h)
{
    LxQtTaskGroup* group = qobject_cast<LxQtTaskGroup*>(parent());
    Q_ASSERT(group);
    bool eyecandy = group->parentTaskBar()->settings().eyeCandy;

    if (eyecandy)
    {
        mSizeAnimation->stop();
        mSizeAnimation->setStartValue(size());
        mSizeAnimation->setEndValue(QSize(w,h));
        mSizeAnimation->start();
    }
    else
    {
        resize(w,h);
    }
}



