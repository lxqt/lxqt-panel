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
#include <QList>
#include <QMimeData>
#include <QWheelEvent>
#include <QFlag>
#include <QX11Info>
#include <QDebug>
#include <QTimer>

#include <LXQt/GridLayout>
#include <XdgIcon>

#include "lxqttaskbar.h"
#include "lxqttaskgroup.h"

using namespace LXQt;

/************************************************

************************************************/
LXQtTaskBar::LXQtTaskBar(ILXQtPanelPlugin *plugin, QWidget *parent) :
    QFrame(parent),
    mButtonStyle(Qt::ToolButtonTextBesideIcon),
    mCloseOnMiddleClick(true),
    mRaiseOnCurrentDesktop(true),
    mShowOnlyOneDesktopTasks(false),
    mShowDesktopNum(0),
    mShowOnlyCurrentScreenTasks(false),
    mShowOnlyMinimizedTasks(false),
    mAutoRotate(true),
    mShowGroupOnHover(true),
    mPlugin(plugin),
    mPlaceHolder(new QWidget(this)),
    mStyle(new LeftAlignedTextStyle())
{
    setStyle(mStyle);
    mLayout = new LXQt::GridLayout(this);
    setLayout(mLayout);
    mLayout->setMargin(0);
    mLayout->setStretch(LXQt::GridLayout::StretchHorizontal | LXQt::GridLayout::StretchVertical);
    realign();

    mPlaceHolder->setMinimumSize(1, 1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mLayout->addWidget(mPlaceHolder);

    QTimer::singleShot(0, this, SLOT(settingsChanged()));
    setAcceptDrops(true);

    connect(KWindowSystem::self(), SIGNAL(stackingOrderChanged()), SLOT(refreshTaskList()));
    connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
            , this, &LXQtTaskBar::onWindowChanged);
}

/************************************************

 ************************************************/
LXQtTaskBar::~LXQtTaskBar()
{
    delete mStyle;
}

/************************************************

 ************************************************/
bool LXQtTaskBar::acceptWindow(WId window) const
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
void LXQtTaskBar::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(LXQtTaskGroup::mimeDataFormat()))
    {
        event->acceptProposedAction();
        buttonMove(nullptr, LXQtTaskGroup::mimeDataData(event->mimeData()), event->pos());
    } else
        event->ignore();
    QWidget::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LXQtTaskBar::dragMoveEvent(QDragMoveEvent * event)
{
    //we don't get any dragMoveEvents if dragEnter wasn't accepted
    buttonMove(nullptr, LXQtTaskGroup::mimeDataData(event->mimeData()), event->pos());
    QWidget::dragMoveEvent(event);
}

/************************************************

 ************************************************/
void LXQtTaskBar::buttonMove(LXQtTaskGroup * dst, QString const & srcWindow, QPoint const & pos)
{
    LXQtTaskGroup *src_group = mGroupsHash.value(srcWindow, nullptr);
    if (!src_group)
    {
        qDebug() << "Dropped invalid";
        return;
    }

    const int src_index = mLayout->indexOf(src_group);
    const int size = mLayout->count();
    Q_ASSERT(0 < size);
    //dst is nullptr in case the drop occured on empty space in taskbar
    int dst_index;
    if (nullptr == dst)
    {
        //moving based on taskbar (not signaled by button)
        QRect occupied = mLayout->occupiedGeometry();
        QRect last_empty_row{occupied};
        if (mPlugin->panel()->isHorizontal())
        {
            last_empty_row.setTopLeft(mLayout->itemAt(size - 1)->geometry().topRight());
        } else
        {
            last_empty_row.setTopLeft(mLayout->itemAt(size - 1)->geometry().bottomLeft());
        }

        if (occupied.contains(pos) && !last_empty_row.contains(pos))
            return;

        dst_index = size;
    } else
    {
        //moving based on signal from child button
        dst_index = mLayout->indexOf(dst);
        if (mPlugin->panel()->isHorizontal())
        {
            if (dst->rect().center().x() < pos.x())
                ++dst_index;
        } else
        {
            if (dst->rect().center().y() < pos.y())
                ++dst_index;
        }
    }

    //moving lower index to higher one => consider as the QList::move => insert(to, takeAt(from))
    if (src_index < dst_index)
        --dst_index;

    if (dst_index == src_index)
        return;

    mLayout->moveItem(src_index, dst_index, true);
}

/************************************************

 ************************************************/
void LXQtTaskBar::groupBecomeEmptySlot()
{
    //group now contains no buttons - clean up in hash and delete the group
    LXQtTaskGroup *group = qobject_cast<LXQtTaskGroup*>(sender());
    Q_ASSERT(group);

    mGroupsHash.erase(mGroupsHash.find(group->groupName()));
    group->deleteLater();
}

/************************************************

 ************************************************/

void LXQtTaskBar::addWindow(WId window, QString const & groupId)
{
    LXQtTaskGroup *group = mGroupsHash.value(groupId);

    if (!group)
    {
        group = new LXQtTaskGroup(groupId, KWindowSystem::icon(window), mPlugin, this);
        connect(group, SIGNAL(groupBecomeEmpty(QString)), this, SLOT(groupBecomeEmptySlot()));
        connect(group, SIGNAL(visibilityChanged(bool)), this, SLOT(refreshPlaceholderVisibility()));
        connect(group, &LXQtTaskGroup::popupShown, this, &LXQtTaskBar::groupPopupShown);
        connect(group, SIGNAL(windowDisowned(WId)), this, SLOT(refreshTaskList()));
        connect(group, &LXQtTaskButton::dragging, this, [this] (QString const & mimeWindow, QPoint const & pos) {
            buttonMove(qobject_cast<LXQtTaskGroup *>(sender()), mimeWindow, pos);
        });

        mLayout->addWidget(group);
        mGroupsHash.insert(groupId, group);
        group->setToolButtonsStyle(mButtonStyle);
    }
    group->addWindow(window);
}
/************************************************

 ************************************************/

void LXQtTaskBar::refreshTaskList()
{
    // Just add new windows to groups, deleting is up to the groups
    QList<WId> tmp = KWindowSystem::stackingOrder();

    Q_FOREACH (WId wnd, tmp)
    {
        if (acceptWindow(wnd))
        {
            // If grouping disabled group behaves like regular button
            QString id = mGroupingEnabled ? KWindowInfo(wnd, 0, NET::WM2WindowClass).windowClassClass() : QString("%1").arg(wnd);
            addWindow(wnd, id);
        }
    }

    refreshPlaceholderVisibility();
}

/************************************************

 ************************************************/
void LXQtTaskBar::onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{
    // If grouping disabled group behaves like regular button
    QString id = mGroupingEnabled ? KWindowInfo(window, 0, NET::WM2WindowClass).windowClassClass() : QString("%1").arg(window);
    LXQtTaskGroup *group = mGroupsHash.value(id);

    bool consumed{false};
    if (nullptr != group)
    {
        consumed = group->onWindowChanged(window, prop, prop2);

    }

    if (!consumed && acceptWindow(window))
        addWindow(window, id);
}

/************************************************

 ************************************************/
void LXQtTaskBar::refreshButtonRotation()
{
    bool autoRotate = mAutoRotate && (mButtonStyle != Qt::ToolButtonIconOnly);

    ILXQtPanel::Position panelPosition = mPlugin->panel()->position();
    QHashIterator<QString,LXQtTaskGroup*> j(mGroupsHash);
    while(j.hasNext())
    {
        j.next();
        j.value()->setAutoRotation(autoRotate,panelPosition);
    }
}

/************************************************

 ************************************************/
void LXQtTaskBar::refreshPlaceholderVisibility()
{
    // if no visible group button show placeholder widget
    bool haveVisibleWindow = false;
    QHashIterator<QString, LXQtTaskGroup*> j(mGroupsHash);
    while (j.hasNext())
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
void LXQtTaskBar::refreshIconGeometry()
{
    QHashIterator<QString, LXQtTaskGroup*> i(mGroupsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->refreshIconsGeometry();
    }
}

/************************************************

 ************************************************/
void LXQtTaskBar::setButtonStyle(Qt::ToolButtonStyle buttonStyle)
{
    mButtonStyle = buttonStyle;

    QHashIterator<QString, LXQtTaskGroup*> i(mGroupsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->setToolButtonsStyle(buttonStyle);
    }
}

/************************************************

 ************************************************/
void LXQtTaskBar::settingsChanged()
{
    bool groupingEnabledOld = mGroupingEnabled;
    bool showOnlyOneDesktopTasksOld = mShowOnlyOneDesktopTasks;
    const int showDesktopNumOld = mShowDesktopNum;
    bool showOnlyCurrentScreenTasksOld = mShowOnlyCurrentScreenTasks;
    bool showOnlyMinimizedTasksOld = mShowOnlyMinimizedTasks;

    mButtonWidth = mPlugin->settings()->value("buttonWidth", 400).toInt();
    mButtonHeight = mPlugin->settings()->value("buttonHeight", 100).toInt();
    QString s = mPlugin->settings()->value("buttonStyle").toString().toUpper();

    if (s == "ICON")
        setButtonStyle(Qt::ToolButtonIconOnly);
    else if (s == "TEXT")
        setButtonStyle(Qt::ToolButtonTextOnly);
    else
        setButtonStyle(Qt::ToolButtonTextBesideIcon);

    mShowOnlyOneDesktopTasks = mPlugin->settings()->value("showOnlyOneDesktopTasks", mShowOnlyOneDesktopTasks).toBool();
    mShowDesktopNum = mPlugin->settings()->value("showDesktopNum", mShowDesktopNum).toInt();
    mShowOnlyCurrentScreenTasks = mPlugin->settings()->value("showOnlyCurrentScreenTasks", mShowOnlyCurrentScreenTasks).toBool();
    mShowOnlyMinimizedTasks = mPlugin->settings()->value("showOnlyMinimizedTasks", mShowOnlyMinimizedTasks).toBool();
    mAutoRotate = mPlugin->settings()->value("autoRotate", true).toBool();
    mCloseOnMiddleClick = mPlugin->settings()->value("closeOnMiddleClick", true).toBool();
    mRaiseOnCurrentDesktop = mPlugin->settings()->value("raiseOnCurrentDesktop", false).toBool();
    mGroupingEnabled = mPlugin->settings()->value("groupingEnabled",true).toBool();
    mShowGroupOnHover = mPlugin->settings()->value("showGroupOnHover",true).toBool();

    // Delete all groups if grouping feature toggled and start over
    if (groupingEnabledOld != mGroupingEnabled)
    {
        Q_FOREACH (LXQtTaskGroup *group, mGroupsHash.values())
        {
            mLayout->removeWidget(group);
            group->deleteLater();
        }
        mGroupsHash.clear();
    }

    if (showOnlyOneDesktopTasksOld != mShowOnlyOneDesktopTasks
            || (mShowOnlyOneDesktopTasks && showDesktopNumOld != mShowDesktopNum)
            || showOnlyCurrentScreenTasksOld != mShowOnlyCurrentScreenTasks
            || showOnlyMinimizedTasksOld != mShowOnlyMinimizedTasks
            )
        Q_FOREACH (LXQtTaskGroup *group, mGroupsHash)
            group->showOnlySettingChanged();

    refreshTaskList();
}

/************************************************

 ************************************************/
void LXQtTaskBar::realign()
{
    mLayout->setEnabled(false);
    refreshButtonRotation();

    ILXQtPanel *panel = mPlugin->panel();
    QSize maxSize = QSize(mButtonWidth, mButtonHeight);
    QSize minSize = QSize(0, 0);

    bool rotated = false;

    if (panel->isHorizontal())
    {
        mLayout->setRowCount(panel->lineCount());
        mLayout->setColumnCount(0);
    }
    else
    {
        mLayout->setRowCount(0);

        if (mButtonStyle == Qt::ToolButtonIconOnly)
        {
            // Vertical + Icons
            mLayout->setColumnCount(panel->lineCount());
        }
        else
        {
            rotated = mAutoRotate && (panel->position() == ILXQtPanel::PositionLeft || panel->position() == ILXQtPanel::PositionRight);

            // Vertical + Text
            if (rotated)
            {
                maxSize.rwidth()  = mButtonHeight;
                maxSize.rheight() = mButtonWidth;

                mLayout->setColumnCount(panel->lineCount());
            }
            else
            {
                mLayout->setColumnCount(1);
            }
        }
    }

    mLayout->setCellMinimumSize(minSize);
    mLayout->setCellMaximumSize(maxSize);
    mLayout->setDirection(rotated ? LXQt::GridLayout::TopToBottom : LXQt::GridLayout::LeftToRight);
    mLayout->setEnabled(true);

    //our placement on screen could have been changed
    Q_FOREACH (LXQtTaskGroup *group, mGroupsHash)
        group->showOnlySettingChanged();
    refreshIconGeometry();
}

/************************************************

 ************************************************/
void LXQtTaskBar::wheelEvent(QWheelEvent* event)
{
    static int threshold = 0;
    threshold += abs(event->delta());
    if (threshold < 300)
        return;
    else
        threshold = 0;

    int delta = event->delta() < 0 ? 1 : -1;

    // create temporary list of visible groups in the same order like on the layout
    QList<LXQtTaskGroup*> list;
    LXQtTaskGroup *group = NULL;
    for (int i = 0; i < mLayout->count(); i++)
    {
        QWidget * o = mLayout->itemAt(i)->widget();
        LXQtTaskGroup * g = qobject_cast<LXQtTaskGroup *>(o);
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

    LXQtTaskButton *button = NULL;

    // switching between groups from temporary list in modulo addressing
    while (!button)
    {
        button = group->getNextPrevChildButton(delta == 1, !(list.count() - 1));
        if (button)
            button->raiseApplication();
        int idx = (list.indexOf(group) + delta + list.count()) % list.count();
        group = list.at(idx);
    }
}

/************************************************

 ************************************************/
void LXQtTaskBar::resizeEvent(QResizeEvent* event)
{
    refreshIconGeometry();
    return QWidget::resizeEvent(event);
}

/************************************************

 ************************************************/
void LXQtTaskBar::changeEvent(QEvent* event)
{
    // if current style is changed, reset the base style of the proxy style
    // so we can apply the new style correctly to task buttons.
    if(event->type() == QEvent::StyleChange)
        mStyle->setBaseStyle(NULL);

    QFrame::changeEvent(event);
}

void LXQtTaskBar::groupPopupShown(LXQtTaskGroup * const sender)
{
    //close all popups (should they be visible because of close delay)
    for (auto group : mGroupsHash)
    {
        if (group->isVisible() && sender != group)
            group->setPopupVisible(false, true/*fast*/);
    }
}
