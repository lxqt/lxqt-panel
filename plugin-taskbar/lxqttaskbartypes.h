#ifndef LXQTTASKBARTYPES_H
#define LXQTTASKBARTYPES_H

#include <QtGlobal>

typedef quintptr WId;

enum class LXQtTaskBarBackendAction
{
    Move = 0,
    Resize,
    Maximize,
    MaximizeVertically,
    MaximizeHorizontally,
    Minimize,
    RollUp,
    FullScreen
};

enum class LXQtTaskBarWindowProperty
{
    Title = 0,
    Icon,
    State,
    Geometry,
    Urgency,
    WindowClass,
    Workspace
};

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
