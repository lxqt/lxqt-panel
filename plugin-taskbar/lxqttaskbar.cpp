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

#include <QApplication>
#include <QDebug>
#include <QToolButton>
#include <QSettings>

#include <LXQt/GridLayout>
#include <XdgIcon>
#include <QList>
#include <QMimeData>
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QFlag>
#include <QX11Info>
#include <QDebug>

#include "lxqttaskbar.h"
#include "lxqttaskbutton.h"
#include "../panel/ilxqtpanelplugin.h"



using namespace LxQt;

/************************************************

************************************************/
LxQtTaskBar::LxQtTaskBar(ILxQtPanelPlugin *plugin, QWidget *parent) :
    QFrame(parent),
    mMasterPopup(NULL),
    mShowGroupOnHover(true),
    mPlugin(plugin),
    mPlaceHolder(new QWidget(this)),
    mStyle(new ElidedButtonStyle())
{
    mLayout = new LxQt::GridLayout(this);
    setLayout(mLayout);
    mLayout->setMargin(0);
    realign();

    mPlaceHolder->setStyle(mStyle);
    mPlaceHolder->setMinimumSize(1, 1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mLayout->addWidget(mPlaceHolder);

    settingsChanged();
    setAcceptDrops(true);

    connect(KWindowSystem::self(), SIGNAL(stackingOrderChanged()), SLOT(refreshTaskList()));
}

/************************************************

 ************************************************/
LxQtTaskBar::~LxQtTaskBar()
{
    delete mStyle;
}

/************************************************

 ************************************************/
bool LxQtTaskBar::windowOnActiveDesktop(WId window) const
{
    if (!mShowOnlyCurrentDesktopTasks)
        return true;

    int desktop = KWindowInfo(window, NET::WMDesktop).desktop();
    if (desktop == NET::OnAllDesktops)
        return true;

    return desktop == KWindowSystem::currentDesktop();
}

/************************************************

 ************************************************/
bool LxQtTaskBar::acceptWindow(WId window) const
{
    QFlags<NET::WindowTypeMask> ignoreList;
    ignoreList |= NET::DesktopMask;
    ignoreList |= NET::DockMask;
    ignoreList |= NET::SplashMask;
    ignoreList |= NET::ToolbarMask;
    ignoreList |= NET::MenuMask;
    ignoreList |= NET::PopupMenuMask;
    ignoreList |= NET::NotificationMask;

    KWindowInfo info(window, NET::WMWindowType | NET::WMState, NET::WM2TransientFor);
    if (!info.valid())
        return false;

    if (NET::typeMatchesMask(info.windowType(NET::AllTypesMask), ignoreList))
        return false;

    if (info.state() & NET::SkipTaskbar)
        return false;

    // WM_TRANSIENT_FOR hint not set - normal window
    WId transFor = info.transientFor();
    if (transFor == 0 || transFor == window || transFor == (WId) QX11Info::appRootWindow())
        return true;

    info = KWindowInfo(transFor, NET::WMWindowType);

    QFlags<NET::WindowTypeMask> normalFlag;
    normalFlag |= NET::NormalMask;
    normalFlag |= NET::DialogMask;
    normalFlag |= NET::UtilityMask;

    return !NET::typeMatchesMask(info.windowType(NET::AllTypesMask), normalFlag);
}

/************************************************

 ************************************************/
void LxQtTaskBar::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(LxQtTaskGroup::taskGroupMimeDataFormat()))
        event->acceptProposedAction();
    else
        event->ignore();
    QWidget::dragEnterEvent(event);
}

int LxQtTaskBar::dropValue(int idx)
{
    QWidget * w = mLayout->itemAt(idx)->widget();

    if (mPlugin->panel()->isHorizontal())
        return w->x() + w->width() / 2;
    else
        return w->y() + w->height() / 2;
}

void LxQtTaskBar::groupDroppedSlot(const QPoint &point, QDropEvent *event)
{
    int droppedIndex;

    qDebug() << event->mimeData()->text();
    if (event->mimeData()->hasFormat(LxQtTaskGroup::taskGroupMimeDataFormat()))
    {
        QString mime;
        QString data;

        if (event->mimeData()->hasFormat(LxQtTaskButton::taskButtonMimeDataFormat())
                && !mGroupingEnabled)
        {
            mime = LxQtTaskButton::taskButtonMimeDataFormat();
            QDataStream stream(event->mimeData()->data(mime));
            qlonglong w;
            stream >> w;
            data = QString("%1").arg(w);
        }
        else
        {
            mime = LxQtTaskGroup::taskGroupMimeDataFormat();
            QDataStream stream(event->mimeData()->data(mime));
            stream >> data;
        }

        qDebug() << mime << data;
        droppedIndex = mLayout->indexOf(mGroupsHash.value(data,NULL));
        mMasterPopup->hide();
    }

    int newPos = -1;
    int p;
    mPlugin->panel()->isHorizontal() ? p = point.x() : p = point.y();

    const int size = mLayout->count();
    if (mPlugin->panel()->isHorizontal())
    {
        for (int i = 0; i < droppedIndex && newPos == -1; i++)
            if (mLayout->itemAt(i)->widget()->x() + mLayout->itemAt(i)->widget()->width() / 2 > event->pos().x())
                newPos = i;

        for (int i = size - 1; i > droppedIndex && newPos == -1; i--)
            if (mLayout->itemAt(i)->widget()->x() + mLayout->itemAt(i)->widget()->width() / 2 < event->pos().x())
                newPos = i;
    }
    else
    {
        for (int i = 0; i < droppedIndex && newPos == -1; i++)
            if (mLayout->itemAt(i)->widget()->y() + mLayout->itemAt(i)->widget()->height() / 2 > event->pos().y())
                newPos = i;

        for (int i = size - 1; i > droppedIndex && newPos == -1; i--)
            if (mLayout->itemAt(i)->widget()->y() + mLayout->itemAt(i)->widget()->height() / 2 < event->pos().y())
                newPos = i;
    }

    if (newPos == -1 || droppedIndex == newPos)
        return;

    qDebug() << QString("Dropped button should go to position %1").arg(newPos);

    mLayout->moveItem(droppedIndex, newPos);
    mLayout->invalidate();
}

/************************************************

 ************************************************/
void LxQtTaskBar::dropEvent(QDropEvent* event)
{
    groupDroppedSlot(event->pos(),event);
    QWidget::dropEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskBar::groupBecomeEmptySlot()
{
    //group now contains no buttons - clean up in hash and delete the group
    LxQtTaskGroup *group = qobject_cast<LxQtTaskGroup*>(sender());

    Q_ASSERT(group);

    QString id = mGroupingEnabled ? group->groupName() : QString("%1").arg(group->windowId());
    group->deleteLater();
}

/************************************************

 ************************************************/

void LxQtTaskBar::refreshTaskList()
{
    //just add new windows to groups, deleting is up to the groups

    QList<WId> tmp = KWindowSystem::stackingOrder();

    foreach (WId wnd, tmp)
    {
        if (acceptWindow(wnd))
        {
            KWindowInfo info(wnd, 0, NET::WM2WindowClass);

            LxQtTaskGroup *group = NULL;
            QString id = mGroupingEnabled ? info.windowClassClass() : QString("%1").arg(wnd);

            // If grouping disabled group behaves like regular button
            group = mGroupsHash.value(id);

            if (!group)
            {
                group = new LxQtTaskGroup(id, KWindowSystem::icon(wnd), mPlugin, this);
                connect(group, SIGNAL(groupBecomeEmpty(QString)), this, SLOT(groupBecomeEmptySlot()));
                connect(group, SIGNAL(dropped(QPoint,QDropEvent*)), this, SLOT(groupDroppedSlot(QPoint, QDropEvent*)));
                connect(group, SIGNAL(visibilityChanged(bool)), this, SLOT(refreshPlaceholderVisibility()));

                mLayout->addWidget(group);
                mGroupsHash.insert(id, group);
                group->setToolButtonsStyle(mButtonStyle);
            }
            group->createButton(wnd);
        }
    }

    refreshPlaceholderVisibility();
    mLayout->invalidate();
    realign();
}

/************************************************

 ************************************************/
void LxQtTaskBar::refreshButtonRotation()
{
    bool autoRotate = mAutoRotate && (mButtonStyle != Qt::ToolButtonIconOnly);

    ILxQtPanel::Position panelPosition = mPlugin->panel()->position();
    QHashIterator<QString,LxQtTaskGroup*> j(mGroupsHash);
    while(j.hasNext())
    {
        j.next();
        j.value()->setAutoRotation(autoRotate,panelPosition);
    }
}
/************************************************

 ************************************************/
void LxQtTaskBar::refreshPlaceholderVisibility()
{
    // if no visible group button show placeholder widget

    bool haveVisibleWindow = false;
    QHashIterator<QString, LxQtTaskGroup*> j(mGroupsHash);
    while(j.hasNext())
    {
        j.next();
        if (j.value()->isVisible())
            haveVisibleWindow = true;
    }
    mPlaceHolder->setVisible(!haveVisibleWindow);
    if (haveVisibleWindow)
        mPlaceHolder->setFixedSize(0, 0);
    else
    {
        mPlaceHolder->setMinimumSize(1, 1);
        mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }

}

/************************************************

 ************************************************/
void LxQtTaskBar::refreshIconGeometry()
{
    QHashIterator<QString, LxQtTaskGroup*> i(mGroupsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->refreshIconsGeometry();
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::setButtonStyle(Qt::ToolButtonStyle buttonStyle)
{
    mButtonStyle = buttonStyle;

    QHashIterator<QString, LxQtTaskGroup*> i(mGroupsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->setToolButtonsStyle(buttonStyle);
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::settingsChanged()
{
    bool groupingEnabledOld = mGroupingEnabled;
    bool showOnlyCurrentDesktopTasksOld = mShowOnlyCurrentDesktopTasks;

    mShowOnlyCurrentDesktopTasks = mPlugin->settings()->value("showOnlyCurrentDesktopTasks").toBool();
    mButtonWidth = mPlugin->settings()->value("buttonWidth", 400).toInt();
    QString s = mPlugin->settings()->value("buttonStyle").toString().toUpper();

    if (s == "ICON")
        setButtonStyle(Qt::ToolButtonIconOnly);
    else if (s == "TEXT")
        setButtonStyle(Qt::ToolButtonTextOnly);
    else
        setButtonStyle(Qt::ToolButtonTextBesideIcon);

    mShowOnlyCurrentDesktopTasks = mPlugin->settings()->value("showOnlyCurrentDesktopTasks", false).toBool();
    mAutoRotate = mPlugin->settings()->value("autoRotate", true).toBool();
    mCloseOnMiddleClick = mPlugin->settings()->value("closeOnMiddleClick", true).toBool();
    mGroupingEnabled = mPlugin->settings()->value("groupingEnabled",true).toBool();
    mShowGroupOnHover = mPlugin->settings()->value("showGroupOnHover",true).toBool();

    // Delete all groups if grouping feature toggled and start over
    if (groupingEnabledOld != mGroupingEnabled)
    {
        foreach(LxQtTaskGroup *group, mGroupsHash)
        {
            mLayout->removeWidget(group);
            group->deleteLater();
        }
        mGroupsHash.clear();
    }

    if (showOnlyCurrentDesktopTasksOld != mShowOnlyCurrentDesktopTasks)
        foreach (LxQtTaskGroup *group, mGroupsHash)
            group->showOnAllDesktopSettingChanged();

    refreshTaskList();
}

/************************************************

 ************************************************/
void LxQtTaskBar::realign()
{
    mLayout->setEnabled(false);
    refreshButtonRotation();

    ILxQtPanel *panel = mPlugin->panel();
    QSize maxSize = QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    QSize minSize = QSize(0, 0);

    bool rotated = false;

    if (panel->isHorizontal())
    {
        if (mButtonStyle == Qt::ToolButtonIconOnly)
        {
            // Horizontal + Icons **************
            mLayout->setRowCount(panel->lineCount());
            mLayout->setColumnCount(0);
            mLayout->setStretch(LxQt::GridLayout::StretchVertical);

            minSize.rheight() = 0;
            minSize.rwidth()  = 0;

            maxSize.rheight() = QWIDGETSIZE_MAX;
            maxSize.rwidth()  = mButtonWidth;
        }
        else
        {
            // Horizontal + Text ***************
            mLayout->setRowCount(panel->lineCount());
            mLayout->setColumnCount(0);
            mLayout->setStretch(LxQt::GridLayout::StretchHorizontal | LxQt::GridLayout::StretchVertical);

            minSize.rheight() = 0;
            minSize.rwidth()  = 0;

            maxSize.rheight() = QWIDGETSIZE_MAX;
            maxSize.rwidth()  = mButtonWidth;
        }
    }
    else
    {
        if (mButtonStyle == Qt::ToolButtonIconOnly)
        {
            // Vertical + Icons
            mLayout->setRowCount(0);
            mLayout->setColumnCount(panel->lineCount());
            mLayout->setStretch(LxQt::GridLayout::StretchHorizontal);

            minSize.rheight() = 0;
            minSize.rwidth()  = 0;

            maxSize.rheight() = QWIDGETSIZE_MAX;
            maxSize.rwidth()  = QWIDGETSIZE_MAX;

        }
        else
        {
            if (mAutoRotate)
            {
                switch (panel->position())
                {
                case ILxQtPanel::PositionLeft:
                case ILxQtPanel::PositionRight:
                    rotated = true;
                    break;

                default:;
                }
            }

            // Vertical + Text
            if (rotated)
            {
                mLayout->setColumnCount(panel->lineCount());
                mLayout->setRowCount(0);
                mLayout->setStretch(LxQt::GridLayout::StretchHorizontal | LxQt::GridLayout::StretchVertical);

                minSize.rheight() = 0;
                minSize.rwidth()  = 0;

                maxSize.rheight() = mButtonWidth;
                maxSize.rwidth()  = QWIDGETSIZE_MAX;
            }
            else
            {
                mLayout->setColumnCount(1);
                mLayout->setRowCount(0);
                mLayout->setStretch(LxQt::GridLayout::StretchHorizontal);

                minSize.rheight() = 0;
                minSize.rwidth()  = mButtonWidth;

                maxSize.rheight() = QWIDGETSIZE_MAX;
                maxSize.rwidth()  = QWIDGETSIZE_MAX;
            }
        }
    }

    mLayout->setCellMinimumSize(minSize);
    mLayout->setCellMaximumSize(maxSize);
    mLayout->setDirection(rotated ? LxQt::GridLayout::TopToBottom : LxQt::GridLayout::LeftToRight);
    mLayout->setEnabled(true);

    refreshIconGeometry();
}

/************************************************

 ************************************************/
void LxQtTaskBar::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta() < 0 ? 1 : -1;

    // create temporary list of visible groups in the same order like on the layout
    QList<LxQtTaskGroup *>list;
    LxQtTaskGroup * group = NULL;
    for (int i = 0 ; i < mLayout->count(); i++)
    {
        QWidget * o = mLayout->itemAt(i)->widget();
        LxQtTaskGroup * g = qobject_cast<LxQtTaskGroup *>(o);
        if (!g)
            continue;

        if (g->isVisible())
            list.append(g);
        if (g->isChecked())
            group = g;
    }

    if (list.isEmpty())
        return;

    if (!group)
        group = list.at(0);

    bool ok = false;
    int i = 100;

    //switching between groups from temporary list in modulo addressing
    while (!ok && i--)
    {
        ok = group->checkNextPrevChild(delta == 1 ? true : false, !(list.count() - 1));
        int idx = (list.indexOf(group) + delta) + list.count();
        idx %= list.count();
        group = list.at(idx);
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::resizeEvent(QResizeEvent* event)
{
    refreshIconGeometry();
    return QWidget::resizeEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskBar::changeEvent(QEvent* event)
{
    // if current style is changed, reset the base style of the proxy style
    // so we can apply the new style correctly to task buttons.
    if(event->type() == QEvent::StyleChange)
        mStyle->setBaseStyle(NULL);

    QFrame::changeEvent(event);
}
