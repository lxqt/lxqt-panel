/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include "menudiskitem.h"
#include <QDesktopServices>
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QToolButton>
#include <QUrl>
#include <XdgIcon>
#include <Solid/StorageAccess>
#include <Solid/OpticalDrive>
#include <LXQt/Notification>


MenuDiskItem::MenuDiskItem(Solid::Device device, QWidget *parent):
    QFrame(parent),
    mDevice(device),
    mDiskButtonClicked(false),
    mEjectButtonClicked(false)
{
    Q_ASSERT(device.is<Solid::StorageAccess>());
    mDiskButton = new QToolButton(this);
    mDiskButton->setObjectName("DiskButton");
    mDiskButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mDiskButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(mDiskButton, SIGNAL(clicked()), this, SLOT(diskButtonClicked()));

    mEjectButton =  new QToolButton(this);
    mEjectButton->setObjectName("EjectButton");
    connect(mEjectButton, SIGNAL(clicked()), this, SLOT(ejectButtonClicked()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(mDiskButton);
    layout->addWidget(mEjectButton);
    this->setLayout(layout);

    layout->setMargin(0);
    layout->setSpacing(0);

    mEjectButton->setIcon(XdgIcon::fromTheme("media-eject"));

    connect(device.as<Solid::StorageAccess>(), SIGNAL(setupDone(Solid::ErrorType, QVariant, const QString &)),
              this, SLOT(mounted(Solid::ErrorType, QVariant, const QString &)));

    connect(device.as<Solid::StorageAccess>(), SIGNAL(teardownDone(Solid::ErrorType, QVariant, const QString &)),
              this, SLOT(unmounted(Solid::ErrorType, QVariant, const QString &)));

    update();
}

void MenuDiskItem::update()
{
    if (mDevice.isValid())
    {
        const QIcon icon = XdgIcon::fromTheme(mDevice.icon(), QStringLiteral("drive-removable-media"));

        mDiskButton->setIcon(icon);
        mDiskButton->setText(mDevice.description());

        setMountStatus(mDevice.as<Solid::StorageAccess>()->isAccessible() || !opticalParent().udi().isEmpty());
    }
}


void MenuDiskItem::setMountStatus(bool is_mount)
{
    mEjectButton->setEnabled(is_mount);
}


void MenuDiskItem::diskButtonClicked()
{
    mDiskButtonClicked = true;
    Solid::StorageAccess* di = mDevice.as<Solid::StorageAccess>();
    if (!di->isAccessible())
        di->setup();
    else
        mounted(Solid::NoError, QString(), mDevice.udi());

    qobject_cast<QWidget*>(parent())->hide();
}

void MenuDiskItem::mounted(Solid::ErrorType error, QVariant resultData, const QString &udi)
{
    if (mDiskButtonClicked)
    {
        mDiskButtonClicked = false;

        if (Solid::NoError == error)
        {
            QDesktopServices::openUrl(QUrl(mDevice.as<Solid::StorageAccess>()->filePath()));
        } else
        {
            LxQt::Notification::notify(tr("Removable media/devices manager")
                    , tr("Mounting of <strong><nobr>\"%1\"</nobr></strong> failed: %2").arg(mDevice.description()).arg(resultData.toString()), mDevice.icon());
            qWarning() << "MenuDiskItem::mounted" << udi << resultData.toString();
        }
    }
    update();
}

void MenuDiskItem::ejectButtonClicked()
{
    mEjectButtonClicked = true;
    Solid::StorageAccess* di = mDevice.as<Solid::StorageAccess>();
    if (di->isAccessible())
    {
        di->teardown();
    } else
    {
        unmounted(Solid::NoError, QString(), mDevice.udi());
    }

    qobject_cast<QWidget*>(parent())->hide();
}

void MenuDiskItem::unmounted(Solid::ErrorType error, QVariant resultData, const QString &udi)
{
    if (mEjectButtonClicked)
    {
        mEjectButtonClicked = false;
        if (Solid::NoError == error)
        {
            Solid::Device opt_parent = opticalParent();
            if (!opt_parent.udi().isEmpty())
                opt_parent.as<Solid::OpticalDrive>()->eject();
        } else
        {
            LxQt::Notification::notify(tr("Removable media/devices manager")
                    , tr("Unmounting of <strong><nobr>\"%1\"</nobr></strong> failed: %2").arg(mDevice.description()).arg(resultData.toString()), mDevice.icon());
            qWarning() << "MenuDiskItem::unmounted" << udi << resultData.toString();
        }
    }
    update();
}

QString MenuDiskItem::DeviceUdi() const
{
    return mDevice.udi();
}

Solid::Device MenuDiskItem::opticalParent() const
{
    Solid::Device it = mDevice;
    //search for parent drive
    for ( ; !it.udi().isEmpty(); it = it.parent())
    {
        if (it.is<Solid::OpticalDrive>())
            break;
    }
    return it;
}
