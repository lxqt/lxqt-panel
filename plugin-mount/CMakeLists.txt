set(PLUGIN "mount")


set(HEADERS
    razormountplugin.h
    mountbutton.h
    menudiskitem.h
    razormountconfiguration.h
    popup.h
    actions/deviceaction.h
    actions/deviceaction_info.h
    actions/deviceaction_menu.h
    actions/deviceaction_nothing.h
)

set(SOURCES
    razormountplugin.cpp
    mountbutton.cpp
    menudiskitem.cpp
    popup.cpp
    razormountconfiguration.cpp
    actions/deviceaction.cpp
    actions/deviceaction_info.cpp
    actions/deviceaction_menu.cpp
    actions/deviceaction_nothing.cpp
)

set(MOCS
    razormountplugin.h
    mountbutton.h
    menudiskitem.h
    popup.h
    razormountconfiguration.h
    actions/deviceaction.h
    actions/deviceaction_info.h
    actions/deviceaction_menu.h
    actions/deviceaction_nothing.h
)

set(UIS
    razormountconfiguration.ui
)

# FIXME: we probably should either add lxqtmount-config.cmake to 
# lxqt-mount, or merge it with liblxqt again since it's small enough.
set(LIBRARIES lxqtmount)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
