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
    ${CMAKE_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.cpp
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
    ${CMAKE_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.h
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

include_directories(${LXQTMOUNT_INCLUDE_DIRS})
set(LIBRARIES ${QTXDG_LIBRARIES} ${LXQTMOUNT_LIBRARIES})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
