#include "dbustypes.h"

// Marshall the IconPixmap data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const IconPixmap &icon)
{
    argument.beginStructure();
    argument << icon.width;
    argument << icon.height;
    argument << icon.bytes;
    argument.endStructure();
    return argument;
}

// Retrieve the ImageStruct data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, IconPixmap &icon)
{
    argument.beginStructure();
    argument >> icon.width;
    argument >> icon.height;
    argument >> icon.bytes;
    argument.endStructure();
    return argument;
}

// Marshall the ToolTip data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const ToolTip &toolTip)
{
    argument.beginStructure();
    argument << toolTip.iconName;
    argument << toolTip.iconPixmap;
    argument << toolTip.title;
    argument << toolTip.description;
    argument.endStructure();
    return argument;
}

// Retrieve the ToolTip data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, ToolTip &toolTip)
{
    argument.beginStructure();
    argument >> toolTip.iconName;
    argument >> toolTip.iconPixmap;
    argument >> toolTip.title;
    argument >> toolTip.description;
    argument.endStructure();
    return argument;
}
