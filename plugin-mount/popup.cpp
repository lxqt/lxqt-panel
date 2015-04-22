/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011-2013 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#include "popup.h"
#include <QDesktopWidget>
#include <QGridLayout>
#include <QLabel>
#include "menudiskitem.h"
#include "popup.h"
#include <Solid/StorageAccess>
#include <Solid/StorageDrive>
#include <Solid/DeviceNotifier>


static bool hasRemovableParent(Solid::Device device)
{
    qDebug() << "acess:" << device.udi();
    for ( ; !device.udi().isEmpty(); device = device.parent())
    {
        Solid::StorageDrive* drive = device.as<Solid::StorageDrive>();
        if (drive && drive->isRemovable())
        {
            qDebug() << "removable parent drive:" << device.udi();
            return true;
        }
    }
    return false;
}

Popup::Popup(ILxQtPanelPlugin *plugin, QWidget* parent):
    QDialog(parent,  Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup | Qt::X11BypassWindowManagerHint),
    mPlugin(plugin),
    mDisplayCount(0)
{
    setObjectName("LxQtMountPopup");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(new QVBoxLayout(this));
    layout()->setMargin(0);

    setAttribute(Qt::WA_AlwaysShowToolTips);


    mPlaceholder = new QLabel(tr("No devices are available"), this);
    mPlaceholder->setObjectName("NoDiskLabel");
    layout()->addWidget(mPlaceholder);
    mPlaceholder->hide();

    foreach(Solid::Device device, Solid::Device::listFromType(Solid::DeviceInterface::StorageAccess))
    {
        if (hasRemovableParent(device))
            addItem(device);
    }

    connect(Solid::DeviceNotifier::instance(), SIGNAL(deviceAdded(QString const &))
            , this, SLOT(deviceAdded(QString const &)));
    connect(Solid::DeviceNotifier::instance(), SIGNAL(deviceRemoved(QString const &))
            , this, SLOT(deviceRemoved(QString const &)));
}


MenuDiskItem *Popup::addItem(Solid::Device device)
{
    MenuDiskItem  *item   = new MenuDiskItem(device, this);
    connect(item, &MenuDiskItem::invalid, this, &Popup::deviceRemoved);
    layout()->addWidget(item);
    item->setVisible(true);
    mDisplayCount++;
    if (mDisplayCount != 0)
        mPlaceholder->hide();

    if (isVisible())
        realign();
    return item;
}


void Popup::hideEvent(QHideEvent *event)
{
    mPlaceholder->hide();

    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}


void Popup::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    realign();
}


void Popup::showEvent(QShowEvent *event)
{
    if (mDisplayCount == 0)
        mPlaceholder->show();

    realign();

    this->setFocus();
    this->activateWindow();
    QWidget::showEvent(event);
    emit visibilityChanged(true);
}


void Popup::realign()
{
    updateGeometry();
    adjustSize();

    QRect rect = mPlugin->calculatePopupWindowPos(size());
    setGeometry(rect);
}


void Popup::showHide()
{
    setHidden(!isHidden());
}

void Popup::deviceRemoved(QString const & udi)
{
    MenuDiskItem* item(0);
    for (int i = layout()->count() - 1; 0 <= i; --i)
    {
        QWidget* w = layout()->itemAt(i)->widget();
        if (w == mPlaceholder)
            continue;

        MenuDiskItem& it = dynamic_cast<MenuDiskItem&>(*layout()->itemAt(i)->widget());
        if (udi == it.DeviceUdi())
        {
            item = &it;
            break;
        }
    }
    if (0 != item)
    {
        layout()->removeWidget(item);
        delete item;
        --mDisplayCount;
        if (mDisplayCount == 0)
            mPlaceholder->show();
    }
}

void Popup::deviceAdded(QString const & udi)
{
    Solid::Device device(udi);
    if (device.is<Solid::StorageAccess>() && hasRemovableParent(device)/*is this superfluous? if device was just added?*/)
    {
        addItem(device);
    }
}
