/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LxQt team
 * Authors:
 *  Balázs Béla <balazsbela[at]gmail.com>
 *  Paulo Lieuthier <paulolieuthier@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "statusnotifierbutton.h"

StatusNotifierButton::StatusNotifierButton(QString service, QString objectPath, QWidget *parent)
    : QToolButton(parent),
    mStatus(Passive)
{
    interface = new org::kde::StatusNotifierItem(service, objectPath, QDBusConnection::sessionBus());

    newToolTip();
    newStatus(interface->status());
    resetIcon();

    connect(interface, SIGNAL(NewIcon()), this, SLOT(newIcon()));
    connect(interface, SIGNAL(NewOverlayIcon()), this, SLOT(newOverlayIcon()));
    connect(interface, SIGNAL(NewAttentionIcon()), this, SLOT(newAttentionIcon()));
    connect(interface, SIGNAL(NewToolTip()), this, SLOT(newToolTip()));
    connect(interface, SIGNAL(NewStatus(QString)), this, SLOT(newStatus(QString)));
}

void StatusNotifierButton::newIcon()
{
    refetchIcon(Passive);
    resetIcon();
}

void StatusNotifierButton::newOverlayIcon()
{
    refetchIcon(Active);
    resetIcon();
}

void StatusNotifierButton::newAttentionIcon()
{
    refetchIcon(NeedsAttention);
    resetIcon();
}

void StatusNotifierButton::refetchIcon(Status status)
{
    QString iconName;
    if (status == Active)
        iconName = interface->overlayIconName();
    else if (status == NeedsAttention)
        iconName = interface->attentionIconName();
    else // status == Passive
        iconName = interface->iconName();

    QIcon nextIcon;
    if (!iconName.isEmpty())
    {
        if (nextIcon.hasThemeIcon(iconName))
            nextIcon = QIcon::fromTheme(iconName);
        else
        {
            QDir themeDir(interface->iconThemePath());
            if (themeDir.exists(iconName + ".png"))
                nextIcon.addFile(themeDir.filePath(iconName + ".png"));

            if (themeDir.cd("hicolor"))
            {
                QStringList sizes = themeDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                foreach (QString dir, sizes)
                {
                    QStringList dirs = QDir(themeDir.filePath(dir)).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                    nextIcon.addFile(QDir(themeDir.path() + "/" + dir + "/" + dirs.at(0)).filePath(iconName + ".png"));
                }
            }

            if (nextIcon.isNull())
                nextIcon = QIcon::fromTheme("application-x-executable");
        }
    }
    else
    {
        IconPixmapList iconPixmaps;
        if (status == Active)
            iconPixmaps = interface->overlayIconPixmap();
        else if (status == NeedsAttention)
            iconPixmaps = interface->attentionIconPixmap();
        else // status == Passive
            iconPixmaps = interface->iconPixmap();


        if (iconPixmaps.empty() || iconPixmaps.first().bytes.isNull())
            nextIcon = QIcon::fromTheme("application-x-executable");
        else
        {
            IconPixmap iconPixmap = iconPixmaps.first();
            QImage image((uchar*) iconPixmap.bytes.data(), iconPixmap.width, iconPixmap.height, QImage::Format_ARGB32);
            const uchar *end = image.constBits() + image.byteCount();
            uchar *dest = reinterpret_cast<uchar*>(iconPixmap.bytes.data());
            for (const uchar *src = image.constBits(); src < end; src += 4, dest += 4)
                qToUnaligned(qToBigEndian<quint32>(qFromUnaligned<quint32>(src)), dest);

            QPixmap pixmap = QPixmap::fromImage(image);
            nextIcon = QIcon(pixmap);
        }
    }

    if (status == Active)
        overlayIcon = nextIcon;
    else if (status == NeedsAttention)
        attentionIcon = nextIcon;
    else // status == Passive
        icon = nextIcon;
}

void StatusNotifierButton::newToolTip()
{
    QString toolTipTitle = interface->toolTip().title;
    setToolTip(toolTipTitle.isEmpty() ? interface->title() : toolTipTitle);
}

void StatusNotifierButton::newStatus(QString status)
{
    Status newStatus;
    if (status == QStringLiteral("Passive"))
        newStatus = Passive;
    else if (status == QStringLiteral("Active"))
        newStatus = Active;
    else
        newStatus = NeedsAttention;

    if (mStatus == newStatus)
        return;

    mStatus = newStatus;
    resetIcon();
}

void StatusNotifierButton::contextMenuEvent(QContextMenuEvent* event)
{
    interface->ContextMenu(QCursor::pos().x(), QCursor::pos().y());
    // QWidget::contextMenuEvent(event);
}

void StatusNotifierButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        interface->Activate(QCursor::pos().x(), QCursor::pos().y());
    else if (event->button() == Qt::MidButton)
        interface->SecondaryActivate(QCursor::pos().x(), QCursor::pos().y());
    QToolButton::mouseReleaseEvent(event);
}

void StatusNotifierButton::wheelEvent(QWheelEvent *event)
{
    interface->Scroll(event->delta(), "vertical");
}

void StatusNotifierButton::resetIcon()
{
    if (mStatus == Active)
    {
        if (overlayIcon.isNull())
            refetchIcon(Active);
        setIcon(overlayIcon);
    }
    else if (mStatus == NeedsAttention)
    {
        if (attentionIcon.isNull())
            refetchIcon(NeedsAttention);
        setIcon(attentionIcon);
    }
    else // mStatus == Passive
    {
        if (icon.isNull())
            refetchIcon(Passive);
        setIcon(icon);
    }
}
