set(PLUGIN "mount")


set(HEADERS
    lxqtmountplugin.h
    mountbutton.h
    menudiskitem.h
    lxqtmountconfiguration.h
    popup.h
    actions/deviceaction.h
    actions/deviceaction_info.h
    actions/deviceaction_menu.h
    actions/deviceaction_nothing.h
)

set(SOURCES
    lxqtmountplugin.cpp
    mountbutton.cpp
    menudiskitem.cpp
    popup.cpp
    lxqtmountconfiguration.cpp
    actions/deviceaction.cpp
    actions/deviceaction_info.cpp
    actions/deviceaction_menu.cpp
    actions/deviceaction_nothing.cpp
)

set(MOCS
    lxqtmountplugin.h
    mountbutton.h
    menudiskitem.h
    popup.h
    lxqtmountconfiguration.h
    actions/deviceaction.h
    actions/deviceaction_info.h
    actions/deviceaction_menu.h
    actions/deviceaction_nothing.h
)

set(UIS
    lxqtmountconfiguration.ui
)

# FIXME: we probably should either add lxqtmount-config.cmake to 
# lxqt-mount, or merge it with liblxqt again since it's small enough.
set(LIBRARIES lxqtmount)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
