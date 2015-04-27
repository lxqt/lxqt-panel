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

using namespace LxQt;

/************************************************

************************************************/
LxQtTaskBar::LxQtTaskBar(ILxQtPanelPlugin *plugin, QWidget *parent) :
    QFrame(parent),
    mButtonStyle(Qt::ToolButtonTextBesideIcon),
    mCloseOnMiddleClick(true),
    mRaiseOnCurrentDesktop(true),
    mShowOnlyCurrentDesktopTasks(false),
    mShowOnlyCurrentScreenTasks(false),
    mShowOnlyMinimizedTasks(false),
    mAutoRotate(true),
    mShowGroupOnHover(true),
    mPlugin(plugin),
    mPlaceHolder(new QWidget(this)),
    mStyle(new LeftAlignedTextStyle())
{
    setStyle(mStyle);
    mLayout = new LxQt::GridLayout(this);
    setLayout(mLayout);
    mLayout->setMargin(0);
    realign();

    mPlaceHolder->setMinimumSize(1, 1);
    mPlaceHolder->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    mPlaceHolder->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mLayout->addWidget(mPlaceHolder);

    QTimer::singleShot(0, this, SLOT(settingsChanged()));
    setAcceptDrops(true);

    connect(KWindowSystem::self(), SIGNAL(stackingOrderChanged()), SLOT(refreshTaskList()));
    connect(KWindowSystem::self(), static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged)
            , this, &LxQtTaskBar::onWindowChanged);
}

/************************************************

 ************************************************/
LxQtTaskBar::~LxQtTaskBar()
{
    delete mStyle;
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
    if (event->mimeData()->hasFormat(LxQtTaskGroup::mimeDataFormat()))
        event->acceptProposedAction();
    else
        event->ignore();
    QWidget::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskBar::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat(LxQtTaskGroup::mimeDataFormat()))
    {
        event->ignore();
        return;
    }

    QString data;
    QDataStream stream(event->mimeData()->data(LxQtTaskGroup::mimeDataFormat()));
    stream >> data;

    LxQtTaskGroup *group = mGroupsHash.value(data, NULL);
    if (!group)
    {
        qDebug() << "Dropped invalid";
        return;
    }

    int droppedIndex = mLayout->indexOf(group);
    int newPos = -1;
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

    mLayout->moveItem(droppedIndex, newPos);
    mLayout->invalidate();

    QWidget::dropEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskBar::groupBecomeEmptySlot()
{
    //group now contains no buttons - clean up in hash and delete the group
    LxQtTaskGroup *group = qobject_cast<LxQtTaskGroup*>(sender());
    Q_ASSERT(group);

    mGroupsHash.erase(mGroupsHash.find(group->groupName()));
    group->deleteLater();
}

/************************************************

 ************************************************/

void LxQtTaskBar::addWindow(WId window, QString const & groupId)
{
    LxQtTaskGroup *group = mGroupsHash.value(groupId);

    if (!group)
    {
        group = new LxQtTaskGroup(groupId, KWindowSystem::icon(window), mPlugin, this);
        connect(group, SIGNAL(groupBecomeEmpty(QString)), this, SLOT(groupBecomeEmptySlot()));
        connect(group, SIGNAL(visibilityChanged(bool)), this, SLOT(refreshPlaceholderVisibility()));
        connect(group, &LxQtTaskGroup::popupShown, this, &LxQtTaskBar::groupPopupShown);
        connect(group, SIGNAL(windowDisowned(WId)), this, SLOT(refreshTaskList()));

        mLayout->addWidget(group);
        mGroupsHash.insert(groupId, group);
        group->setToolButtonsStyle(mButtonStyle);
    }
    group->addWindow(window);
}
/************************************************

 ************************************************/

void LxQtTaskBar::refreshTaskList()
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
void LxQtTaskBar::onWindowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{
    // If grouping disabled group behaves like regular button
    QString id = mGroupingEnabled ? KWindowInfo(window, 0, NET::WM2WindowClass).windowClassClass() : QString("%1").arg(window);
    LxQtTaskGroup *group = mGroupsHash.value(id);

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
    bool showOnlyCurrentScreenTasksOld = mShowOnlyCurrentScreenTasks;
    bool showOnlyMinimizedTasksOld = mShowOnlyMinimizedTasks;

    mButtonWidth = mPlugin->settings()->value("buttonWidth", 400).toInt();
    mButtonHeight = mPlugin->settings()->value("buttonHeight", 100).toInt();
    QString s = mPlugin->settings()->value("buttonStyle").toString().toUpper();

    if (s == "ICON")
    {
        setButtonStyle(Qt::ToolButtonIconOnly);
        mLayout->setStretch(mPlugin->panel()->isHorizontal() ?
                            LxQt::GridLayout::StretchVertical :
                            LxQt::GridLayout::StretchHorizontal);
    }
    else
    {
        if (s == "TEXT")
            setButtonStyle(Qt::ToolButtonTextOnly);
        else
            setButtonStyle(Qt::ToolButtonTextBesideIcon);
        mLayout->setStretch(LxQt::GridLayout::StretchHorizontal | LxQt::GridLayout::StretchVertical);
    }

    mShowOnlyCurrentDesktopTasks = mPlugin->settings()->value("showOnlyCurrentDesktopTasks", mShowOnlyCurrentDesktopTasks).toBool();
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
        Q_FOREACH (LxQtTaskGroup *group, mGroupsHash.values())
        {
            mLayout->removeWidget(group);
            group->deleteLater();
        }
        mGroupsHash.clear();
    }

    if (showOnlyCurrentDesktopTasksOld != mShowOnlyCurrentDesktopTasks
            || showOnlyCurrentScreenTasksOld != mShowOnlyCurrentScreenTasks
            || showOnlyMinimizedTasksOld != mShowOnlyMinimizedTasks
            )
        Q_FOREACH (LxQtTaskGroup *group, mGroupsHash)
            group->showOnlySettingChanged();

    refreshTaskList();
}

/************************************************

 ************************************************/
void LxQtTaskBar::realign()
{
    mLayout->setEnabled(false);
    refreshButtonRotation();

    ILxQtPanel *panel = mPlugin->panel();
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
            rotated = mAutoRotate && (panel->position() == ILxQtPanel::PositionLeft || panel->position() == ILxQtPanel::PositionRight);

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

    if (mButtonStyle == Qt::ToolButtonIconOnly)
        mLayout->setStretch(panel->isHorizontal() ?
                            LxQt::GridLayout::StretchVertical :
                            LxQt::GridLayout::StretchHorizontal);

    mLayout->setCellMinimumSize(minSize);
    mLayout->setCellMaximumSize(maxSize);
    mLayout->setDirection(rotated ? LxQt::GridLayout::TopToBottom : LxQt::GridLayout::LeftToRight);
    mLayout->setEnabled(true);

    //our placement on screen could have been changed
    Q_FOREACH (LxQtTaskGroup *group, mGroupsHash)
        group->showOnlySettingChanged();
    refreshIconGeometry();
}

/************************************************

 ************************************************/
void LxQtTaskBar::wheelEvent(QWheelEvent* event)
{
    static int threshold = 0;
    threshold += abs(event->delta());
    if (threshold < 300)
        return;
    else
        threshold = 0;

    int delta = event->delta() < 0 ? 1 : -1;

    // create temporary list of visible groups in the same order like on the layout
    QList<LxQtTaskGroup*> list;
    LxQtTaskGroup *group = NULL;
    for (int i = 0; i < mLayout->count(); i++)
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

    LxQtTaskButton *button = NULL;

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

void LxQtTaskBar::groupPopupShown(LxQtTaskGroup * const sender)
{
    //close all popups (should they be visible because of close delay)
    for (auto group : mGroupsHash)
    {
        if (group->isVisible() && sender != group)
            group->setPopupVisible(false, true/*fast*/);
    }
}
