/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2.1+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 David Edmundson <davidedmundson@kde.org>
 *            2022 LXQt team
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

#pragma once

#include <QByteArray>
#include <QDBusArgument>
#include <QImage>
#include <QString>
#include <QList>

// Custom message type for DBus
struct KDbusImageStruct {
    KDbusImageStruct();
    KDbusImageStruct(const QImage &image);
    int width;
    int height;
    QByteArray data;
};

typedef QList<KDbusImageStruct> KDbusImageVector;

struct KDbusToolTipStruct {
    QString icon;
    KDbusImageVector image;
    QString title;
    QString subTitle;
};

const QDBusArgument &operator<<(QDBusArgument &argument, const KDbusImageStruct &icon);
const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusImageStruct &icon);

Q_DECLARE_METATYPE(KDbusImageStruct)

const QDBusArgument &operator<<(QDBusArgument &argument, const KDbusImageVector &iconVector);
const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusImageVector &iconVector);

Q_DECLARE_METATYPE(KDbusImageVector)

const QDBusArgument &operator<<(QDBusArgument &argument, const KDbusToolTipStruct &toolTip);
const QDBusArgument &operator>>(const QDBusArgument &argument, KDbusToolTipStruct &toolTip);

Q_DECLARE_METATYPE(KDbusToolTipStruct)
