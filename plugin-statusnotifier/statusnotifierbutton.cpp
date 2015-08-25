/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
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

#include <QDir>
#include <QFile>
#include <dbusmenu-qt5/dbusmenuimporter.h>
#include "../panel/ilxqtpanelplugin.h"


StatusNotifierButton::StatusNotifierButton(QString service, QString objectPath, ILXQtPanelPlugin* plugin, QWidget *parent)
    : QToolButton(parent),
    mMenu(NULL),
    mStatus(Passive),
    mValid(true),
    mFallbackIcon(QIcon::fromTheme("application-x-executable")),
    mPlugin(plugin)
{
    interface = new org::kde::StatusNotifierItem(service, objectPath, QDBusConnection::sessionBus(), this);

    QString menuPath = interface->menu().path();
    if (!menuPath.isEmpty())
    {
        mMenu = (new DBusMenuImporter(service, interface->menu().path(), this))->menu();
        dynamic_cast<QObject &>(*mMenu).setParent(this);
        mMenu->setObjectName(QStringLiteral("StatusNotifierMenu"));
    }

    // HACK: sni-qt creates some invalid items (like one for konversarion 1.5)
    if (interface->title().isEmpty() && interface->id().isEmpty())
        mValid = false;

    if (mValid)
    {
        newToolTip();
        refetchIcon(Active);
        refetchIcon(Passive);
        refetchIcon(NeedsAttention);
        newStatus(interface->status());
        resetIcon();

        connect(interface, SIGNAL(NewIcon()), this, SLOT(newIcon()));
        connect(interface, SIGNAL(NewOverlayIcon()), this, SLOT(newOverlayIcon()));
        connect(interface, SIGNAL(NewAttentionIcon()), this, SLOT(newAttentionIcon()));
        connect(interface, SIGNAL(NewToolTip()), this, SLOT(newToolTip()));
        connect(interface, SIGNAL(NewStatus(QString)), this, SLOT(newStatus(QString)));
    }
}

StatusNotifierButton::~StatusNotifierButton()
{
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
    switch (status)
    {
        case Active:
            iconName = interface->overlayIconName();
            break;
        case NeedsAttention:
            iconName = interface->attentionIconName();
            break;
        case Passive:
            iconName = interface->iconName();
            break;
    }

    QIcon nextIcon;
    if (!iconName.isEmpty())
    {
        if (QIcon::hasThemeIcon(iconName))
            nextIcon = QIcon::fromTheme(iconName);
        else
        {
            QDir themeDir(interface->iconThemePath());
            if (themeDir.exists())
            {
                if (themeDir.exists(iconName + ".png"))
                    nextIcon.addFile(themeDir.filePath(iconName + ".png"));

                if (themeDir.cd("hicolor") || (themeDir.cd("icons") && themeDir.cd("hicolor")))
                {
                    QStringList sizes = themeDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                    foreach (QString dir, sizes)
                    {
                        QStringList dirs = QDir(themeDir.filePath(dir)).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                        foreach (QString innerDir, dirs)
                        {
                            QString file = themeDir.absolutePath() + "/" + dir + "/" + innerDir + "/" + iconName + ".png";
                            if (QFile::exists(file))
                                nextIcon.addFile(file);
                        }
                    }
                }
            }
        }
    }
    else
    {
        IconPixmapList iconPixmaps;
        switch (status)
        {
            case Active:
                iconPixmaps = interface->overlayIconPixmap();
                break;
            case NeedsAttention:
                iconPixmaps = interface->attentionIconPixmap();
                break;
            case Passive:
                iconPixmaps = interface->iconPixmap();
                break;
        }


        if (!iconPixmaps.empty() && !iconPixmaps.first().bytes.isNull())
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

    switch (status)
    {
        case Active:
            mOverlayIcon = nextIcon;
            break;
        case NeedsAttention:
            mAttentionIcon = nextIcon;
            break;
        case Passive:
            mIcon = nextIcon;
            break;
    }
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
    //XXX: avoid showing of parent's context menu, we are (optionaly) providing context menu on mouseReleaseEvent
    //QWidget::contextMenuEvent(event);
}

void StatusNotifierButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        interface->Activate(QCursor::pos().x(), QCursor::pos().y());
    else if (event->button() == Qt::MidButton)
        interface->SecondaryActivate(QCursor::pos().x(), QCursor::pos().y());
    else if (Qt::RightButton == event->button())
        mMenu->popup(QCursor::pos());
    QToolButton::mouseReleaseEvent(event);
}

void StatusNotifierButton::wheelEvent(QWheelEvent *event)
{
    interface->Scroll(event->delta(), "vertical");
}

void StatusNotifierButton::resetIcon()
{
    if (mStatus == Active && !mOverlayIcon.isNull())
        setIcon(mOverlayIcon);
    else if (mStatus == NeedsAttention && !mAttentionIcon.isNull())
        setIcon(mAttentionIcon);
    else if (!mIcon.isNull()) // mStatus == Passive
        setIcon(mIcon);
    else if (!mOverlayIcon.isNull())
        setIcon(mOverlayIcon);
    else if (!mAttentionIcon.isNull())
        setIcon(mAttentionIcon);
    else
        setIcon(mFallbackIcon);
}
