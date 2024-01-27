#ifndef LXQTTASKBARTYPES_H
#define LXQTTASKBARTYPES_H

#include <QtGlobal>

typedef quintptr WId;

enum class LXQtTaskBarWindowState
{
    Hidden = 0,
    FullScreen,
    Minimized,
    Maximized,
    MaximizedVertically,
    MaximizedHorizontally,
    Normal,
    RolledUp //Shaded
};

enum class LXQtTaskBarWindowLayer
{
    KeepBelow = 0,
    Normal,
    KeepAbove
};

enum class LXQtTaskBarWorkspace
{
    ShowOnAll = -1
};

#endif // LXQTTASKBARTYPES_H
