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
    connect(btn,SIGNAL(dragging(bool)),LxQtMasterPopup::instance(parentTaskBar()),SLOT(dragging(bool)));

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
        raisePopup(false);
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
        startStopFrameCloseTimer(false);
        raisePopup(true);
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
    if (raise && !mPreventPopup)
    {
        //setup geometry
        recalculateFrameSize();
        recalculateFramePosition();

        if (!windowId() || parentTaskBar()->settings().switchGroupWhenHoverOneWindow)
            LxQtMasterPopup::instance(parentTaskBar())->activateGroup(this,true);
        else
            LxQtMasterPopup::instance(parentTaskBar())->activateGroup(this,false);
    }
    else
    {
        LxQtMasterPopup::instance(parentTaskBar())->activateGroup(this,false);
    }
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
    LxQtMasterPopup * p = LxQtMasterPopup::instance(parentTaskBar());
    int height = recalculateFrameHeight();
    p->setMaximumHeight(1000);
    p->setMinimumHeight(0);

    int hh = recalculateFrameWidth();
    p->setMaximumWidth(hh);
    p->setMinimumWidth(0);

    p->resizeAnimated(QSize(hh,height));

    return QSize(hh,height);
}

/************************************************

 ************************************************/
int LxQtTaskGroup::recalculateFrameHeight() const
{
    int h = parentTaskBar()->settings().groupButtonHeight;
    int cont = visibleButtonsCount();
    int height = cont * h + (cont +1) * mLayout->spacing();

    return height;
}

/************************************************

 ************************************************/
int LxQtTaskGroup::recalculateFrameWidth() const
{
    int hh = parentTaskBar()->settings().groupButtonWidth;
    return hh;
}

/************************************************

 ************************************************/
QPoint LxQtTaskGroup::recalculateFramePosition()
{
    //set position
    LxQtMasterPopup * p = LxQtMasterPopup::instance(parentTaskBar());
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
    y =    parentWidget()->mapToGlobal(pos()).y() + y_offset;

    p->moveAnimated(QPoint(x,y));

    return QPoint(x,y);
}

void LxQtTaskGroup::startStopFrameCloseTimer(bool start)
{
    LxQtTaskGroup * g = this;
    if (parentTaskBar()->settings().showGroupWhenHover || parentTaskBar()->settings().switchGroupWhenHover)
        g = NULL;
    LxQtMasterPopup::instance(parentTaskBar())->activateCloseTimer(g,start);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::leaveEvent(QEvent *event)
{
    startStopFrameCloseTimer(true);
    mSwitchTimer->stop();
}

/************************************************

 ************************************************/
void LxQtTaskGroup::enterEvent(QEvent *event)
{
    startStopFrameCloseTimer(false);

    if (parentTaskBar()->settings().showGroupWhenHover)
        mSwitchTimer->start();


    if (parentTaskBar()->settings().switchGroupWhenHover &&
            LxQtMasterPopup::instance(parentTaskBar())->isVisible())
        mSwitchTimer->start();
}

/************************************************

 ************************************************/
void LxQtTaskGroup::dragEnterEvent(QDragEnterEvent *event)
{
    startStopFrameCloseTimer(false);
    LxQtTaskButton::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskGroup::windowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{
    //LxQtTaskButton* button = NULL;
    QVector<LxQtTaskButton *> buttons;
    buttons.append(mButtonHash.value(window,NULL));
    if (window == windowId())
        buttons.append(this);

    // window changed virtual desktop
    foreach (LxQtTaskButton * button, buttons)
    {
        if (!button)
        {
            continue;
        }
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
}
