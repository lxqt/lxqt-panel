#include <QDBusArgument>

#ifndef DBUSTYPES_H
#define DBUSTYPES_H

struct IconPixmap {
    int width;
    int height;
    QByteArray bytes;
};

typedef QList<IconPixmap> IconPixmapList;

struct ToolTip {
    QString iconName;
    QList<IconPixmap> iconPixmap;
    QString title;
    QString description;
};

QDBusArgument &operator<<(QDBusArgument &argument, const IconPixmap &icon);
const QDBusArgument &operator>>(const QDBusArgument &argument, IconPixmap &icon);

QDBusArgument &operator<<(QDBusArgument &argument, const ToolTip &toolTip);
const QDBusArgument &operator>>(const QDBusArgument &argument, ToolTip &toolTip);

Q_DECLARE_METATYPE(IconPixmap)
Q_DECLARE_METATYPE(ToolTip)

#endif // DBUSTYPES_H
