#include "lxqtmasterpopup.h"
#include "lxqttaskpopup.h"
#include "lxqttaskbutton.h"
#include <QEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QLayout>
#include "lxqttaskbar.h"
#include <QDebug>

/************************************************
    this class is just a container of window buttons
    the main purpose is showing window buttons in
    vertical layout and drag&drop feature inside
    group
 ************************************************/
LxQtLooseFocusFrame::LxQtLooseFocusFrame(LxQtMasterPopup * parent, LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons):
    QFrame(parent),
    mButtonHash(buttons),
    mGroup(group)
{
    Q_ASSERT(group);
    Q_ASSERT(parent);
    setAcceptDrops(true);
}

/************************************************

 ************************************************/
LxQtLooseFocusFrame::~LxQtLooseFocusFrame()
{
}

/************************************************

 ************************************************/
LxQtMasterPopup * LxQtLooseFocusFrame::parentMasterPopup()
{
    return LxQtMasterPopup::instance(parentTaskBar());
}

/************************************************

 ************************************************/
LxQtTaskBar * LxQtLooseFocusFrame::parentTaskBar()
{
    return mGroup->parentTaskBar();
}

/************************************************

 ************************************************/
void LxQtLooseFocusFrame::dropEvent(QDropEvent *event)
{
    buttonDropped(event->pos(),event);
}

/************************************************
    dragging buttons inside group
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
    QWidget::dragEnterEvent(event);
}

/************************************************
    button dragged inside group dropped
    reorder layout
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
