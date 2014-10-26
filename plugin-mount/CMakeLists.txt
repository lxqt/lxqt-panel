set(PLUGIN "mount")

set(HEADERS
    lxqtmountplugin.h
    lxqtmountconfiguration.h
    mountbutton.h
    menudiskitem.h
    popup.h
    actions/deviceaction.h
    actions/deviceaction_info.h
    actions/deviceaction_menu.h
    actions/deviceaction_nothing.h
)

set(SOURCES
    lxqtmountplugin.cpp
    lxqtmountconfiguration.cpp
    mountbutton.cpp
    menudiskitem.cpp
    popup.cpp
    actions/deviceaction.cpp
    actions/deviceaction_info.cpp
    actions/deviceaction_menu.cpp
    actions/deviceaction_nothing.cpp
)

set(MOCS
    lxqtmountplugin.h
    lxqtmountconfiguration.h
    mountbutton.h
    menudiskitem.h
    popup.h
    actions/deviceaction.h
    actions/deviceaction_info.h
    actions/deviceaction_menu.h
    actions/deviceaction_nothing.h
)

set(UIS
    lxqtmountconfiguration.ui
)

include_directories(${LXQTMOUNT_INCLUDE_DIRS})
set(LIBRARIES ${QTXDG_LIBRARIES} ${LXQTMOUNT_LIBRARIES})

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
