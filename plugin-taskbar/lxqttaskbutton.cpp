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


#include <QDebug>
#include <XdgIcon>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPainter>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <QDragEnterEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>

#include "lxqttaskbutton.h"
#include <KWindowSystem/KWindowSystem>
#include "lxqttaskgroup.h"
#include "lxqttaskbar.h"

// Necessary for closeApplication()
#include <KWindowSystem/NETWM>
#include <QX11Info>

bool LxQtTaskButton::sDraggging = false;

/************************************************

************************************************/
void ElidedButtonStyle::drawItemText(QPainter* painter, const QRect& rect,
                    int flags, const QPalette & pal, bool enabled,
                  const QString & text, QPalette::ColorRole textRole) const
{
    QString s = painter->fontMetrics().elidedText(text, Qt::ElideRight, rect.width());
    QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, s, textRole);
}


/************************************************

************************************************/
LxQtTaskButton::LxQtTaskButton(const WId window,LxQtTaskBar * taskbar ,QWidget *parent) :
    QToolButton(parent),
    mWindow(window),
    mUrgencyHint(false),
    mOrigin(Qt::TopLeftCorner), //whatever (avoid conditional on uninitialized value)
    mDrawPixmap(false),
    mParentTaskBar(taskbar),
    mTimer(new QTimer(this))
{
    Q_ASSERT(taskbar);

    setCheckable(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMinimumWidth(1);
    setMinimumHeight(1);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setAcceptDrops(true);

    updateText();
    updateIcon();

    mTimer->setSingleShot(true);
    mTimer->setInterval(600);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(timerTimeout()));
}

/************************************************

************************************************/
LxQtTaskButton::~LxQtTaskButton()
{
}

/************************************************

 ************************************************/
void LxQtTaskButton::updateText()
{
    KWindowInfo info(mWindow, NET::WMVisibleName | NET::WMName);
    QString title = info.visibleName().isEmpty() ? info.name() : info.visibleName();
    setText(title.replace("&", "&&"));
    setToolTip(title);
}

/************************************************

 ************************************************/
void LxQtTaskButton::updateIcon()
{
    QIcon ico;
    QPixmap pix = KWindowSystem::icon(mWindow);
    ico.addPixmap(pix);
    if (!pix.isNull())
        setIcon(ico);
    else
        setIcon(XdgIcon::defaultApplicationIcon());
}

/************************************************

 ************************************************/
void LxQtTaskButton::refreshIconGeometry()
{
    QRect rect = geometry();
    QPoint globalPos = parentTaskBar()->mapToGlobal(pos());
    rect.moveTo(globalPos);

    NETWinInfo info(QX11Info::connection(), windowId(),
                    (WId) QX11Info::appRootWindow(), NET::WMIconGeometry, 0);
    NETRect nrect;
    nrect.pos.x = rect.x();
    nrect.pos.y = rect.y();
    nrect.size.height = rect.height();
    nrect.size.width = rect.width();
    info.setIconGeometry(nrect);
}

/************************************************

 ************************************************/
void LxQtTaskButton::dragEnterEvent(QDragEnterEvent *event)
{
    if (!event->mimeData()->hasFormat(acceptMimeData()) )
    {
        event->ignore();
        return;
    }

    mTimer->start();

    //it must be here otherwise dragLeaveEvent and dragMoveEvent won't be called
    //on the other hand drop and dragmove events of parent widget won't be called
    event->acceptProposedAction();

    QToolButton::dragEnterEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskButton::dragLeaveEvent(QDragLeaveEvent *event)
{
    mTimer->stop();
    event->ignore();;

    QToolButton::dragLeaveEvent(event);
}

void LxQtTaskButton::dropEvent(QDropEvent *event)
{
    mTimer->stop();
    event->ignore();

    emit dropped(mapToParent(event->pos()),event);
    QToolButton::dropEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        mDragStartPosition = event->pos();


    if (parentTaskBar()->settings().closeOnMiddleClick && event->button() == Qt::MidButton)
    {
        closeApplication();
    }

    QToolButton::mousePressEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton  )
    {
 //        qDebug() << "isChecked:" << isChecked();
        if (isChecked())
            minimizeApplication();
        else
            raiseApplication();
    }
    QToolButton::mouseReleaseEvent(event);
}

/************************************************

 ************************************************/
void LxQtTaskButton::arbitraryMimeData(QMimeData *mimedata)
{
    QByteArray ba;
    QDataStream stream(&ba,QIODevice::WriteOnly);
    stream << (qlonglong)(mWindow);
    mimedata->setData(taskButtonMimeDataFormat(), ba);
}

/************************************************

 ************************************************/
void LxQtTaskButton::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - mDragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QMimeData *mime = new QMimeData;
    arbitraryMimeData(mime);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mime);
    QPixmap pixmap = grab();

    //fixme when vertical panel, pixmap is empty
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(mapTo(this, event->pos())));

    //must be called before exec because it is blocking
    connect(drag,SIGNAL(destroyed()),this,SIGNAL(dragging()));
    emit dragging(true);
    sDraggging = true;

    drag->exec();

    //if button is dropped out of panel (e.g. on desktop)
    //it is not deleted automatically by Qt
    drag->deleteLater();
    sDraggging = false;

    QAbstractButton::mouseMoveEvent(event);
}

/************************************************

 ************************************************/
bool LxQtTaskButton::isAppHidden() const
{
    KWindowInfo info(mWindow, NET::WMState);
    return (info.state() & NET::Hidden);
}

/************************************************

 ************************************************/
bool LxQtTaskButton::isApplicationActive() const
{
    return KWindowSystem::activeWindow() == mWindow;
}

/************************************************

 ************************************************/
void LxQtTaskButton::activateWithDraggable()
{
    // raise app in any time when there is a drag
    // in progress to allow drop it into an app
    raiseApplication();
}

/************************************************

 ************************************************/
void LxQtTaskButton::raiseApplication()
{
    KWindowInfo info(mWindow, NET::WMDesktop);
    int winDesktop = info.desktop();
    if (KWindowSystem::currentDesktop() != winDesktop)
        KWindowSystem::setCurrentDesktop(winDesktop);
    KWindowSystem::activateWindow(mWindow);

    setUrgencyHint(false);
}

/************************************************

 ************************************************/
void LxQtTaskButton::minimizeApplication()
{
    KWindowSystem::minimizeWindow(mWindow);
}

/************************************************

 ************************************************/
void LxQtTaskButton::maximizeApplication()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    int state = act->data().toInt();
    switch (state)
    {
        case NET::MaxHoriz:
            KWindowSystem::setState(mWindow, NET::MaxHoriz);
            break;

        case NET::MaxVert:
            KWindowSystem::setState(mWindow, NET::MaxVert);
            break;

        default:
            KWindowSystem::setState(mWindow, NET::Max);
            break;
    }
}

/************************************************

 ************************************************/
void LxQtTaskButton::deMaximizeApplication()
{
    KWindowSystem::clearState(mWindow, NET::Max);
}

/************************************************

 ************************************************/
void LxQtTaskButton::shadeApplication()
{
    KWindowSystem::setState(mWindow, NET::Shaded);
}

/************************************************

 ************************************************/
void LxQtTaskButton::unShadeApplication()
{
    KWindowSystem::clearState(mWindow, NET::Shaded);
}

/************************************************

 ************************************************/
void LxQtTaskButton::closeApplication()
{
    // FIXME: Why there is no such thing in KWindowSystem??
    NETRootInfo(QX11Info::connection(), NET::CloseWindow).closeWindowRequest(mWindow);
}

/************************************************

 ************************************************/
void LxQtTaskButton::setApplicationLayer()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    int layer = act->data().toInt();
    switch(layer)
    {
        case NET::KeepAbove:
            KWindowSystem::clearState(mWindow, NET::KeepBelow);
            KWindowSystem::setState(mWindow, NET::KeepAbove);
            break;

        case NET::KeepBelow:
            KWindowSystem::clearState(mWindow, NET::KeepAbove);
            KWindowSystem::setState(mWindow, NET::KeepBelow);
            break;

        default:
            KWindowSystem::clearState(mWindow, NET::KeepBelow);
            KWindowSystem::clearState(mWindow, NET::KeepAbove);
            break;
    }
}

/************************************************

 ************************************************/
void LxQtTaskButton::moveApplicationToDesktop()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act)
        return;

    bool ok;
    int desk = act->data().toInt(&ok);

    if (!ok)
        return;

    KWindowSystem::setOnDesktop(mWindow, desk);
}

/************************************************

 ************************************************/
void LxQtTaskButton::contextMenuEvent(QContextMenuEvent* event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        event->ignore();
        return;
    }

    KWindowInfo info(mWindow, 0, NET::WM2AllowedActions);
    unsigned long state = KWindowInfo(mWindow, NET::WMState).state();

    QMenu menu(tr("Application"));
    QAction* a;

    /* KDE menu *******

      + To &Desktop >
      +     &All Desktops
      +     ---
      +     &1 Desktop 1
      +     &2 Desktop 2
      + &To Current Desktop
        &Move
        Re&size
      + Mi&nimize
      + Ma&ximize
      + &Shade
        Ad&vanced >
            Keep &Above Others
            Keep &Below Others
            Fill screen
        &Layer >
            Always on &top
            &Normal
            Always on &bottom
      ---
      + &Close
    */

    /********** Desktop menu **********/
    int deskNum = KWindowSystem::numberOfDesktops();
    if (deskNum > 1)
    {
        int winDesk = KWindowInfo(mWindow, NET::WMDesktop).desktop();
        QMenu* deskMenu = menu.addMenu(tr("To &Desktop"));

        a = deskMenu->addAction(tr("&All Desktops"));
        a->setData(NET::OnAllDesktops);
        a->setEnabled(winDesk != NET::OnAllDesktops);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        deskMenu->addSeparator();

        for (int i = 0; i < deskNum; ++i)
        {
            a = deskMenu->addAction(tr("Desktop &%1").arg(i + 1));
            a->setData(i + 1);
            a->setEnabled(i + 1 != winDesk);
            connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
        }

        int curDesk = KWindowSystem::currentDesktop();
        a = menu.addAction(tr("&To Current Desktop"));
        a->setData(curDesk);
        a->setEnabled(curDesk != winDesk);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(moveApplicationToDesktop()));
    }

    /********** State menu **********/
    menu.addSeparator();

    a = menu.addAction(tr("Ma&ximize"));
    a->setEnabled(info.actionSupported(NET::ActionMax) && !(state & NET::Max));
    a->setData(NET::Max);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

    if (event->modifiers() & Qt::ShiftModifier)
    {
        a = menu.addAction(tr("Maximize vertically"));
        a->setEnabled(info.actionSupported(NET::ActionMaxVert) && !((state & NET::MaxVert) || (state & NET::Hidden)));
        a->setData(NET::MaxVert);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));

        a = menu.addAction(tr("Maximize horizontally"));
        a->setEnabled(info.actionSupported(NET::ActionMaxHoriz) && !((state & NET::MaxHoriz) || (state & NET::Hidden)));
        a->setData(NET::MaxHoriz);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(maximizeApplication()));
    }

    a = menu.addAction(tr("&Restore"));
    a->setEnabled((state & NET::Hidden) || (state & NET::Max) || (state & NET::MaxHoriz) || (state & NET::MaxVert));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(deMaximizeApplication()));

    a = menu.addAction(tr("Mi&nimize"));
    a->setEnabled(info.actionSupported(NET::ActionMinimize) && !(state & NET::Hidden));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(minimizeApplication()));

    if (state & NET::Shaded)
    {
        a = menu.addAction(tr("Roll down"));
        a->setEnabled(info.actionSupported(NET::ActionShade) && !(state & NET::Hidden));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(unShadeApplication()));
    }
    else
    {
        a = menu.addAction(tr("Roll up"));
        a->setEnabled(info.actionSupported(NET::ActionShade) && !(state & NET::Hidden));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(shadeApplication()));
    }

    /********** Layer menu **********/
    menu.addSeparator();

    QMenu* layerMenu = menu.addMenu(tr("&Layer"));

    a = layerMenu->addAction(tr("Always on &top"));
    // FIXME: There is no info.actionSupported(NET::ActionKeepAbove)
    a->setEnabled(!(state & NET::KeepAbove));
    a->setData(NET::KeepAbove);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("&Normal"));
    a->setEnabled((state & NET::KeepAbove) || (state & NET::KeepBelow));
    // FIXME: There is no NET::KeepNormal, so passing 0
    a->setData(0);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    a = layerMenu->addAction(tr("Always on &bottom"));
    // FIXME: There is no info.actionSupported(NET::ActionKeepBelow)
    a->setEnabled(!(state & NET::KeepBelow));
    a->setData(NET::KeepBelow);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setApplicationLayer()));

    /********** Kill menu **********/
    menu.addSeparator();
    a = menu.addAction(XdgIcon::fromTheme("process-stop"), tr("&Close"));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(closeApplication()));
    menu.exec(mapToGlobal(event->pos()));
}

/************************************************

 ************************************************/
void LxQtTaskButton::setUrgencyHint(bool set)
{
    if (mUrgencyHint == set)
        return;

    if (!set)
        KWindowSystem::demandAttention(mWindow, false);

    mUrgencyHint = set;
    setProperty("urgent", set);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

/************************************************

 ************************************************/
int LxQtTaskButton::desktopNum() const
{
    return KWindowInfo(mWindow, NET::WMDesktop).desktop();
}

Qt::Corner LxQtTaskButton::origin() const
{
    return mOrigin;
}

void LxQtTaskButton::setOrigin(Qt::Corner newOrigin)
{
    if (mOrigin != newOrigin)
    {
        mOrigin = newOrigin;
        update();
    }
}

void LxQtTaskButton::setAutoRotation(bool value, ILxQtPanel::Position position)
{
    if (value)
    {
        switch (position)
        {
        case ILxQtPanel::PositionTop:
        case ILxQtPanel::PositionBottom:
            setOrigin(Qt::TopLeftCorner);
            break;

        case ILxQtPanel::PositionLeft:
            setOrigin(Qt::BottomLeftCorner);
            break;

        case ILxQtPanel::PositionRight:
            setOrigin(Qt::TopRightCorner);
            break;
        }
    }
    else
        setOrigin(Qt::TopLeftCorner);
}

void LxQtTaskButton::paintEvent(QPaintEvent *event)
{
    if (mOrigin == Qt::TopLeftCorner)
    {
        QToolButton::paintEvent(event);
        return;
    }

    QSize sz = size();
    QSize adjSz = sz;
    QTransform transform;
    QPoint originPoint;

    switch (mOrigin)
    {
    case Qt::TopLeftCorner:
        transform.rotate(0.0);
        originPoint = QPoint(0.0, 0.0);
        break;

    case Qt::TopRightCorner:
        transform.rotate(90.0);
        originPoint = QPoint(0.0, -sz.width());
        adjSz.transpose();
        break;

    case Qt::BottomRightCorner:
        transform.rotate(180.0);
        originPoint = QPoint(-sz.width(), -sz.height());
        break;

    case Qt::BottomLeftCorner:
        transform.rotate(270.0);
        originPoint = QPoint(-sz.height(), 0.0);
        adjSz.transpose();
        break;
    }

    bool drawPixmapNextTime = false;

    if (!mDrawPixmap)
    {
        mPixmap = QPixmap(adjSz);
        mPixmap.fill(QColor(0, 0, 0, 0));

        if (adjSz != sz)
            resize(adjSz); // this causes paint event to be repeated - next time we'll paint the pixmap to the widget surface.

        // copied from QToolButton::paintEvent   {
        QStylePainter painter(&mPixmap, this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        painter.drawComplexControl(QStyle::CC_ToolButton, opt);
        // }

        if (adjSz != sz)
        {
            resize(sz);
            drawPixmapNextTime = true;
        }
        else
            mDrawPixmap = true; // transfer the pixmap to the widget now!
    }
    if (mDrawPixmap)
    {
        QPainter painter(this);
        painter.setTransform(transform);
        painter.drawPixmap(originPoint, mPixmap);

        drawPixmapNextTime = false;
    }

    mDrawPixmap = drawPixmapNextTime;
}
