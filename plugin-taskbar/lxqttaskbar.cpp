/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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

#include "lxqttaskbar.h"
#include <LXQt/GridLayout>
#include <XdgIcon>
#include <LXQt/XfitMan>
#include <QList>
#include <QMimeData>

#include <QDesktopWidget>
#include <QWheelEvent>

#include <X11/Xlib.h>
#include "lxqttaskbutton.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <QX11Info>
#include "../panel/fixx11h.h"

#include <QDebug>
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

    mRootWindow = QX11Info::appRootWindow();

    settingsChanged();
    setAcceptDrops(true);
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

    XfitMan xf = xfitMan();
    int desktop = xf.getWindowDesktop(window);
    if (desktop == -1) // Show on all desktops
        return true;

    if (desktop == xf.getActiveDesktop())
        return true;

    return false;
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
    XfitMan xf = xfitMan();
    QList<Window> tmp = xf.getClientList();

    //qDebug() << "** Fill ********************************";
    //foreach (Window wnd, tmp)
    // if (xf->acceptWindow(wnd)) qDebug() << XfitMan::debugWindow(wnd);
    //qDebug() << "****************************************";


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

    foreach (Window wnd, tmp)
    {
        if (xf.acceptWindow(wnd))
        {
            LxQtTaskButton* btn = new LxQtTaskButton(wnd, this);
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
        xfitMan().setIconGeometry(button->windowId(), &rect);
    }
}

/************************************************

 ************************************************/
void LxQtTaskBar::activeWindowChanged()
{
    Window window = xfitMan().getActiveAppWindow();
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
void LxQtTaskBar::x11EventFilter(XEventType* event)
{
    int event_type;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    event_type = event->response_type & ~0x80; // XCB
#else
    event_type = event->type; // XLib
#endif
    switch (event_type)
    {
        case PropertyNotify:
            handlePropertyNotify(event);
            break;
        case ConfigureNotify: {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            unsigned long window = reinterpret_cast<xcb_configure_notify_event_t*>(event)->window;
#else
            unsigned long window = event->xconfigure.window;
#endif
	  // if the size or position of our window is changed, update icon geometry
            if(window == effectiveWinId())
            {
                // qDebug() << "configure event";
                refreshIconGeometry();
            }
            break;
        }
#if 0
        case MotionNotify:
            break;

        default:
        {
            qDebug() << "** XEvent ************************";
            qDebug() << "Type:   " << xEventTypeToStr(event);
        }
#endif

    }
}


/************************************************

 ************************************************/
void LxQtTaskBar::handlePropertyNotify(XEventType* event)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    xcb_property_notify_event_t* prop_event = reinterpret_cast<xcb_property_notify_event_t*>(event);
#else
    XPropertyEvent* prop_event = reinterpret_cast<XPropertyEvent*>(event);
#endif
    if (prop_event->window == mRootWindow)
    {
        // Windows list changed ...............................
        if (prop_event->atom == XfitMan::atom("_NET_CLIENT_LIST"))
        {
            refreshTaskList();
            return;
        }

        // Activate window ....................................
        if (prop_event->atom == XfitMan::atom("_NET_ACTIVE_WINDOW"))
        {
            activeWindowChanged();
            return;
        }

        // Desktop switch .....................................
        if (prop_event->atom == XfitMan::atom("_NET_CURRENT_DESKTOP"))
        {
            if (mShowOnlyCurrentDesktopTasks)
                refreshTaskList();
            return;
        }
    }
    else
    {
        LxQtTaskButton* btn = buttonByWindow(prop_event->window);
        if (btn)
        {
            if (prop_event->atom == XfitMan::atom("_NET_WM_DESKTOP"))
            {
                if (mShowOnlyCurrentDesktopTasks)
                {
                    int desktop = btn->desktopNum();
                    btn->setHidden(desktop != -1 && desktop != xfitMan().getActiveDesktop());
                }
            }
            else
                btn->handlePropertyNotify(event);
        }
}

//    char* aname = XGetAtomName(QX11Info::display(), event->atom);
//    qDebug() << "** XPropertyEvent ********************";
//    qDebug() << "  atom:       0x" << hex << event->atom
//            << " (" << (aname ? aname : "Unknown") << ')';
//    qDebug() << "  window:    " << XfitMan::debugWindow(event->window);
//    qDebug() << "  display:   " << event->display;
//    qDebug() << "  send_event:" << event->send_event;
//    qDebug() << "  serial:    " << event->serial;
//    qDebug() << "  state:     " << event->state;
//    qDebug() << "  time:      " << event->time;
//    qDebug();

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

    mShowOnlyCurrentDesktopTasks = mPlugin->settings()->value("showOnlyCurrentDesktopTasks", mShowOnlyCurrentDesktopTasks).toBool();
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
            bool rotated = false;
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

    mLayout->setDirection(panel->isHorizontal() ? LxQt::GridLayout::LeftToRight : LxQt::GridLayout::TopToBottom);
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
                (!mShowOnlyCurrentDesktopTasks || xfitMan().getActiveDesktop() == xfitMan().getWindowDesktop(i.key())))
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
    XfitMan xf = xfitMan();
    QList<Window> winList = xf.getClientList();
    int current = winList.indexOf(xf.getActiveAppWindow());
    int delta = event->delta() < 0 ? 1 : -1;

    for (int ix = current + delta; 0 <= ix && ix < winList.size(); ix += delta)
    {
        Window window = winList.at(ix);
        if (xf.acceptWindow(window) && windowOnActiveDesktop(window))
        {
            xf.raiseWindow(window);
            break;
        }
    }
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
