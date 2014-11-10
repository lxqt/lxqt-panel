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
    mButtonStyle(Qt::ToolButtonTextBesideIcon),
    mCheckedBtn(NULL),
    mCloseOnMiddleClick(true),
    mShowOnlyCurrentDesktopTasks(false),
    mAutoRotate(true),
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
    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), SLOT(refreshTaskList()));
    connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)), SLOT(activeWindowChanged(WId)));
    connect(KWindowSystem::self(), SIGNAL(windowChanged(WId, NET::Properties, NET::Properties2)),
            SLOT(windowChanged(WId, NET::Properties, NET::Properties2)));
}

/************************************************

 ************************************************/
LxQtTaskBar::~LxQtTaskBar()
{
    delete mStyle;
}

/************************************************

 ************************************************/
LxQtTaskButton* LxQtTaskBar::buttonByWindow(WId window) const
{
    if (mButtonsHash.contains(window))
        return mButtonsHash.value(window);
    return 0;
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
    if (event->mimeData()->hasFormat("lxqt/lxqttaskbutton"))
        event->acceptProposedAction();
    else
        event->ignore();
    QWidget::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskBar::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat("lxqt/lxqttaskbutton"))
        return;

    QDataStream stream(event->mimeData()->data("lxqt/lxqttaskbutton"));
    // window id for dropped button
    qlonglong temp;
    stream >> temp;
    WId droppedWid = (WId) temp;
    qDebug() << QString("Dropped window: %1").arg(droppedWid);

    int droppedIndex = mLayout->indexOf(mButtonsHash[droppedWid]);
    int newPos = -1;
    const int size = mLayout->count();
    for (int i = 0; i < droppedIndex && newPos == -1; i++)
        if (mLayout->itemAt(i)->widget()->x() + mLayout->itemAt(i)->widget()->width() / 2 > event->pos().x())
            newPos = i;

    for (int i = size - 1; i > droppedIndex && newPos == -1; i--)
        if (mLayout->itemAt(i)->widget()->x() + mLayout->itemAt(i)->widget()->width() / 2 < event->pos().x())
            newPos = i;

    if (newPos == -1 || droppedIndex == newPos)
        return;

    qDebug() << QString("Dropped button shoud go to position %1").arg(newPos);

    mLayout->moveItem(droppedIndex, newPos);
    mLayout->invalidate();

    QWidget::dropEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskBar::refreshTaskList()
{
    QList<WId> tmp = KWindowSystem::stackingOrder();

    QMutableHashIterator<WId, LxQtTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        int n = tmp.removeAll(i.key());

        if (!n)
        {
            // if the button we're removing is the currently selected app
            if(i.value() == mCheckedBtn)
                mCheckedBtn = NULL;
            delete i.value();
            i.remove();
        }
    }

    foreach (WId wnd, tmp)
    {
        if (acceptWindow(wnd))
        {
            LxQtTaskButton* btn = new LxQtTaskButton(wnd, this);
            if (!mShowOnlyCurrentDesktopTasks)
                btn->setShowDesktopName(true);
            btn->setStyle(mStyle);
            btn->setToolButtonStyle(mButtonStyle);

            mButtonsHash.insert(wnd, btn);
            mLayout->addWidget(btn);
        }
    }
    refreshButtonVisibility();
    mLayout->invalidate();
    activeWindowChanged();
    realign();
}

/************************************************

 ************************************************/
void LxQtTaskBar::refreshButtonRotation()
{
    bool autoRotate = mAutoRotate && (mButtonStyle != Qt::ToolButtonIconOnly);

    ILxQtPanel::Position panelPosition = mPlugin->panel()->position();

    QHashIterator<WId, LxQtTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->setAutoRotation(autoRotate, panelPosition);
    }
}
/************************************************

 ************************************************/

void LxQtTaskBar::refreshButtonVisibility()
{
    bool haveVisibleWindow = false;
    QHashIterator<WId, LxQtTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        bool isVisible = windowOnActiveDesktop(i.key());
        haveVisibleWindow |= isVisible;
        i.value()->setVisible(isVisible);
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
    // FIXME: sometimes we get wrong globalPos here, especially
    // after changing the pos or size of the panel.
    // this might be caused by bugs in lxqtpanel.cpp.
    QHashIterator<WId, LxQtTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        LxQtTaskButton* button = i.value();
        QRect rect = button->geometry();
        QPoint globalPos = mapToGlobal(button->pos());
        rect.moveTo(globalPos);

        NETWinInfo info(QX11Info::connection(), button->windowId(),
                        (WId) QX11Info::appRootWindow(), NET::WMIconGeometry, 0);
        NETRect nrect;
        nrect.pos.x = rect.x();
        nrect.pos.y = rect.y();
        nrect.size.height = rect.height();
        nrect.size.width = rect.width();
        info.setIconGeometry(nrect);
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::activeWindowChanged(WId window)
{
    if (!window)
        window = KWindowSystem::activeWindow();

    LxQtTaskButton* btn = buttonByWindow(window);

    if (mCheckedBtn != btn)
    {
        if (mCheckedBtn)
            mCheckedBtn->setChecked(false);
        if (btn)
        {
            btn->setChecked(true);
            if (btn->hasUrgencyHint())
                btn->setUrgencyHint(false);
        }
        mCheckedBtn = btn;
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::windowChanged(WId window, NET::Properties prop, NET::Properties2 prop2)
{
    LxQtTaskButton* button = buttonByWindow(window);
    if (!button)
        return;

    // window changed virtual desktop
    if (prop.testFlag(NET::WMDesktop))
    {
        if (mShowOnlyCurrentDesktopTasks)
        {
            int desktop = button->desktopNum();
            button->setHidden(desktop != NET::OnAllDesktops && desktop != KWindowSystem::currentDesktop());
        }
    }

    if (prop.testFlag(NET::WMVisibleName) || prop.testFlag(NET::WMName))
        button->updateText();

    if (prop.testFlag(NET::WMIcon) || prop.testFlag(NET::WMIconGeometry))
    {
        // FIXME: This shouldn't be commented, but it's causing high CPU and memory usage =/
        // button->updateIcon();
    }

    if (prop.testFlag(NET::WMState))
    {
        KWindowInfo info(window, NET::WMState);
        button->setUrgencyHint(info.hasState(NET::DemandsAttention));
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::setButtonStyle(Qt::ToolButtonStyle buttonStyle)
{
    mButtonStyle = buttonStyle;

    QHashIterator<WId, LxQtTaskButton*> i(mButtonsHash);
    while (i.hasNext())
    {
        i.next();
        i.value()->setToolButtonStyle(mButtonStyle);
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::settingsChanged()
{
    mButtonWidth = mPlugin->settings()->value("buttonWidth", 400).toInt();
    QString s = mPlugin->settings()->value("buttonStyle").toString().toUpper();

    if (s == "ICON")
        setButtonStyle(Qt::ToolButtonIconOnly);
    else if (s == "TEXT")
        setButtonStyle(Qt::ToolButtonTextOnly);
    else
        setButtonStyle(Qt::ToolButtonTextBesideIcon);

    if (mShowOnlyCurrentDesktopTasks != mPlugin->settings()->value("showOnlyCurrentDesktopTasks", mShowOnlyCurrentDesktopTasks).toBool())
    {
        mShowOnlyCurrentDesktopTasks = mPlugin->settings()->value("showOnlyCurrentDesktopTasks", mShowOnlyCurrentDesktopTasks).toBool();
        foreach (LxQtTaskButton* btn, mButtonsHash)
        {
            btn->setShowDesktopName(!mShowOnlyCurrentDesktopTasks);
        }
    }

    mAutoRotate = mPlugin->settings()->value("autoRotate", true).toBool();
    mCloseOnMiddleClick = mPlugin->settings()->value("closeOnMiddleClick", true).toBool();

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
            // Vertical + Icons ****************
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

            // Vertical + Text *****************
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

void LxQtTaskBar::mousePressEvent(QMouseEvent *event)
{
    // close the app on mouse middle click
    if (mCloseOnMiddleClick && event->button() == Qt::MidButton)
    {
        // find the button at current cursor pos
        QHashIterator<WId, LxQtTaskButton*> i(mButtonsHash);
        while (i.hasNext())
        {
            i.next();
            LxQtTaskButton* btn = i.value();
            if (btn->geometry().contains(event->pos()) &&
                (!mShowOnlyCurrentDesktopTasks || KWindowSystem::currentDesktop() == KWindowInfo(i.key(), NET::WMDesktop).desktop()))
            {
                btn->closeApplication();
                break;
            }
        }
    }
    QFrame::mousePressEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskBar::wheelEvent(QWheelEvent* event)
{
    if (!mCheckedBtn)
        return;

    int current = mLayout->indexOf(mCheckedBtn);
    if (current == -1)
        return;

    int delta = event->delta() < 0 ? 1 : -1;
    for (int ix = current + delta; 0 <= ix && ix < mLayout->count(); ix += delta)
    {
        QLayoutItem *item = mLayout->itemAt(ix);
        if (!item)
            continue;

        WId window = ((LxQtTaskButton *) item->widget())->windowId();
        if (acceptWindow(window) && windowOnActiveDesktop(window))
        {
            KWindowSystem::activateWindow(window);
            break;
        }
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
