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

#ifndef MENUDISKITEM_H
#define MENUDISKITEM_H

#include <QFrame>
#include <Solid/Device>
#include <Solid/SolidNamespace>


class QToolButton;

class MenuDiskItem : public QFrame
{
    Q_OBJECT

public:
    explicit MenuDiskItem(Solid::Device device, QWidget *parent);
    QString DeviceUdi() const;

    void setMountStatus(bool is_mount);

private:
    void update();
    Solid::Device opticalParent() const;

signals:
    void invalid(QString const & udi);

private slots:
    void ejectButtonClicked();
    void diskButtonClicked();
    void mounted(Solid::ErrorType error, QVariant resultData, const QString &udi);
    void unmounted(Solid::ErrorType error, QVariant resultData, const QString &udi);

private:
    Solid::Device mDevice;
    QToolButton *mDiskButton;
    QToolButton *mEjectButton;
    bool mDiskButtonClicked;
    bool mEjectButtonClicked;
};

#endif // MENUDISKITEM_H
