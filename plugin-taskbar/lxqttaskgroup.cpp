#include "lxqttaskgroup.h"
#include <QVBoxLayout>
#include <QDebug>
#include "../panel/ilxqtpanel.h"
#include <QMimeData>
#include "../panel/ilxqtpanelplugin.h"
#include <QDialog>

LxQtTaskGroup::LxQtTaskGroup(const QString &groupName,QIcon icon,ILxQtPanelPlugin * plugin, QWidget *parent):
    LxQtTaskButton(0,parent),
    mGroupName(groupName),
    mFrame(new QFrame),
    mLayout(new QVBoxLayout(NULL)),
    mPlugin(plugin)
{
    setText(groupName);
    setIcon(icon);

    mFrame->setLayout(mLayout);
    mFrame->setHidden(true);

    mFrame->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SplashScreen);
    mFrame->setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    mFrame->setAttribute(Qt::WA_AlwaysShowToolTips);
    //mFrame->setAttribute(Qt::WA_TranslucentBackground);


    //mFrame->setStyle(parentWidget()->style());
    QFrame * p = qobject_cast<QFrame*>(parentWidget());
    //mFrame->setFrameStyle(p->frameStyle());


    mLayout->setSpacing(5);
    mLayout->setMargin(5);


    connect(this,SIGNAL(clicked(bool)),this,SLOT(onClicked(bool)));
}

bool LxQtTaskGroup::addButton(LxQtTaskButton *button)
{
    if (mButtonHash.contains(button->windowId()))
        return false;

    mButtonHash.insert(button->windowId(),button);
    mLayout->addWidget(button);

    button->setMaximumHeight(height());

    regroup();
    return true;
}

void LxQtTaskGroup::removeButton(LxQtTaskButton *button)
{
    mButtonHash.remove(button->windowId());
    regroup();
}

int LxQtTaskGroup::buttonsCount() const
{
    return mButtonHash.count();
}

void LxQtTaskGroup::onClicked(bool checked)
{
    if (buttonsCount() > 1)
    {
        mFrame->setHidden(!checked);
        int x, y;
        x = mapToGlobal(pos()).x() ;
        y =    mapToGlobal(pos()).y()+50;

        mFrame->move(x,y);
    }
     //mPlugin->calculatePopupWindowPos();
    QPoint p = pos();
    QPoint p2 = mapToGlobal(pos());
    qDebug() << p << " " << p2.x();
    //qDebug() <<  mPlugin->calculatePopupWindowPos(mFrame->size());
}

void LxQtTaskGroup::regroup()
{
    if (buttonsCount() == 1)
    {
        LxQtTaskButton * btn = mButtonHash.values().at(0);
        setText(btn->text());
        setWindowId(btn->windowId());
    }
    else
    {
        setText(mGroupName + QString(" - %1 times").arg(buttonsCount()));
        setWindowId(0);
        mFrame->setMaximumHeight(buttonsCount() * height() + (buttonsCount() +1) * mLayout->spacing());
        mFrame->setMinimumWidth(width());

    }
}

void LxQtTaskGroup::arbitraryMimeData(QMimeData *mimedata)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    qDebug() << QString("Dragging group button: %1").arg(groupName());
    stream << groupName();
    mimedata->setData("lxqt/lxqttaskgroup", byteArray);

    if (winId())
    {
        byteArray.clear();
        stream << winId();
        mimedata->setData("lxqt/lxqttaskgroupsinglebutton",byteArray);
    }
}
