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
    ${PROJECT_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.cpp
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

set(UIS
    lxqtmountconfiguration.ui
)

find_package(KF5Solid REQUIRED)

set(LIBRARIES ${QTXDG_LIBRARIES} KF5::Solid)

BUILD_LXQT_PLUGIN(${PLUGIN})
