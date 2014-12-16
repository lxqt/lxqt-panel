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

/************************************************

 ************************************************/
LxQtTaskGroup::LxQtTaskGroup(const QString &groupName,QIcon icon,ILxQtPanelPlugin * plugin, LxQtTaskBar *parent):
    LxQtTaskButton(0,parent,parent),
    mGroupName(groupName),
    mFrame(LxQtMasterPopup::instance(parent)->createFrame(this,mButtonHash)),
    mLayout(new QVBoxLayout()),
    mPlugin(plugin),
    mShowTimer(new QTimer(this))
{
    Q_ASSERT(parent);

    setText(groupName);
    setIcon(icon);

    mFrame->setLayout(mLayout);

    mLayout->setSpacing(5);
    mLayout->setMargin(5);

    connect(this,SIGNAL(clicked(bool)),this,SLOT(onClicked(bool)));
    connect(KWindowSystem::self(),SIGNAL(activeWindowChanged(WId)),this,SLOT(onActiveWindowChanged(WId)));
    connect(KWindowSystem::self(),SIGNAL(windowRemoved(WId)),this,SLOT(onWindowRemoved(WId)));
    connect(KWindowSystem::self(),SIGNAL(currentDesktopChanged(int)),this,SLOT(onDesktopChanged(int)));
    connect(KWindowSystem::self(), SIGNAL(windowChanged(WId, NET::Properties, NET::Properties2)),
            SLOT(windowChanged(WId, NET::Properties, NET::Properties2)));

    mShowTimer->setSingleShot(true);
    mShowTimer->setInterval(400);
    //connect(mShowTimer,SIGNAL(timeout()),this,SLOT(timeoutRaise()));

    setObjectName(groupName);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::contextMenuEvent(QContextMenuEvent *event)
{
    if (windowId())
    {
        LxQtTaskButton::contextMenuEvent(event);
        return;
    }

    QMenu menu(tr("Group"));
    menu.addAction(XdgIcon::fromTheme("process-stop"), tr("Close group"),this,SLOT(closeGroup()));
    menu.exec(mapToGlobal(event->pos()));
}

/************************************************

 ************************************************/
void LxQtTaskGroup::closeGroup()
{
    foreach (LxQtTaskButton * button, mButtonHash)
    {
        button->closeApplication();
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::timeoutRaise()
{
    if (toolButtonStyle() == Qt::ToolButtonIconOnly)
    {
        raisePopup(true);
    }
    else
    {
        if (!windowId())
            raisePopup(true);
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
                if(mLayout->itemAt(i)->widget()->isVisibleTo(mFrame))
                {
                    idx = i;
                    break;
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
    /*
    foreach (LxQtTaskButton* button, mButtonHash)
    {
        button->setToolButtonStyle(style);
    }
    */

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
    if (!windowId())
    {
        raisePopup(true);
    }
    else
    {
        raiseApplication();
    }
}

/************************************************

 ************************************************/
void LxQtTaskGroup::onClicked(bool checked)
{
    if (visibleButtonsCount() > 1)
    {
        setChecked(mButtonHash.contains(KWindowSystem::activeWindow()));
        if (mFrame->isVisible()  )
        {
            raisePopup(false);
            return;
        }
        LxQtMasterPopup::instance(parentTaskBar())->activateCloseTimer(false);
        raisePopup(true);
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
    else
    {
        setText(mGroupName + QString(" - %1 times").arg(cont));
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
    /*
    foreach (LxQtTaskButton * button, mButtonHash)
    {
        //button->setAutoRotation(value,position);
    }
    */
    LxQtTaskButton::setAutoRotation(value,position);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::refreshVisibility()
{
    if (parentTaskBar()->settings().showOnlyCurrentDesktopTasks)
    {
        foreach(LxQtTaskButton * btn, mButtonHash)
        {
            btn->setVisible(btn->desktopNum() == KWindowSystem::currentDesktop());
        }
    }
    else
    {
        foreach(LxQtTaskButton * btn, mButtonHash)
        {
            btn->setVisible(true);
        }
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
    mimedata->setData("lxqt/lxqttaskgroup", byteArray);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::raisePopup(bool raise)
{
    if (raise)
    {
        //setup geometry
        recalculateFrameSize();
        recalculateFramePosition();
    }

    LxQtMasterPopup::instance(parentTaskBar())->activateGroup(this,raise);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::refreshIconsGeometry()
{
    foreach(LxQtTaskButton * but, mButtonHash)
    {
        but->refreshIconGeometry(mPlugin->panel()->iconSize());
    }

    //group icons are set automatically by panel
    //refreshIconGeometry(parentTaskBar());
}

/************************************************

 ************************************************/
QSize LxQtTaskGroup::recalculateFrameSize()
{
    QRect geometry = mPlugin->panel()->globalGometry();
    bool horizontal = mPlugin->panel()->isHorizontal();

    int h = geometry.width() ;
    if (horizontal)
        h = geometry.height();

    if (!horizontal && !parentTaskBar()->settings().autoRotate)
        h = height();

    h /= mPlugin->panel()->lineCount();

    int cont = visibleButtonsCount();
    LxQtMasterPopup * p = LxQtMasterPopup::instance(parentTaskBar());
    p->setMaximumHeight(cont * h + (cont +1) * mLayout->spacing());
    p->setMinimumHeight(p->maximumHeight());


    int hh = height();
    if (mPlugin->panel()->isHorizontal())
        hh = width();
    p->setMaximumWidth(parentTaskBar()->settings().buttonWidth);
    p->setMinimumWidth(p->maximumWidth());

    p->resize(p->maximumWidth(),p->maximumHeight());

}

/************************************************

 ************************************************/
QPoint LxQtTaskGroup::recalculateFramePosition()
{
    //set position
    LxQtMasterPopup * p = LxQtMasterPopup::instance(parentTaskBar());
    int x_offset = 0, y_offset = 0;
    switch (mPlugin->panel()->position())
    {
    case ILxQtPanel::PositionBottom:
        y_offset = -p->height() - 5 ; break;
    case ILxQtPanel::PositionTop:
        y_offset = mPlugin->panel()->globalGometry().height() + 5; break;
    case ILxQtPanel::PositionLeft:
        x_offset = mPlugin->panel()->globalGometry().width() + 5; break;
    case ILxQtPanel::PositionRight:
        x_offset = -p->width() - 5;
        break;
    }

    int x, y;
    x = parentWidget()->mapToGlobal(pos()).x() + x_offset ;
    y =    parentWidget()->mapToGlobal(pos()).y() + y_offset;

    p->move(x,y);

    return QPoint(x,y);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::leaveEvent(QEvent *event)
{
    //timerEnable(true);
    LxQtMasterPopup::instance(parentTaskBar())->activateCloseTimer(true);
    mShowTimer->stop();
}

/************************************************

 ************************************************/
void LxQtTaskGroup::enterEvent(QEvent *event)
{
    //timerEnable(false);
    LxQtMasterPopup::instance(parentTaskBar())->activateCloseTimer(false);
    mShowTimer->start();

    if (LxQtMasterPopup::instance(parentTaskBar())->isVisible())
        raisePopup(true);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::dragEnterEvent(QDragEnterEvent *event)
{
    //timerEnable(false);
    LxQtMasterPopup::instance(parentTaskBar())->activateCloseTimer(false);

    if (LxQtMasterPopup::instance(parentTaskBar())->isVisible())
        raisePopup(true);
    LxQtTaskButton::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::windowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{
    LxQtTaskButton* button = NULL;
    if (window == windowId())
        button = this;
    else
        button = mButtonHash.value(window,NULL);
    if (!button)
        return;

    // window changed virtual desktop
    if (prop.testFlag(NET::WMDesktop))
    {
        if (parentTaskBar()->settings().showOnlyCurrentDesktopTasks)
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
