set(PLUGIN "mount")

set(HEADERS
    lxqtmountplugin.h
    configuration.h
    button.h
    menudiskitem.h
    popup.h
    actions/deviceaction.h
    actions/deviceaction_info.h
    actions/deviceaction_menu.h
    actions/deviceaction_nothing.h
    actions/ejectaction.h
    actions/ejectaction_optical.h
    actions/ejectaction_nothing.h
)

set(SOURCES
    lxqtmountplugin.cpp
    configuration.cpp
    button.cpp
    menudiskitem.cpp
    popup.cpp
    actions/deviceaction.cpp
    actions/deviceaction_info.cpp
    actions/deviceaction_menu.cpp
    actions/deviceaction_nothing.cpp
    actions/ejectaction.cpp
    actions/ejectaction_optical.cpp
    actions/ejectaction_nothing.cpp
)

set(UIS
    configuration.ui
)

find_package(KF6Solid ${QT_MINIMUM_VERSION} REQUIRED)
set(LIBRARIES Qt6Xdg lxqt-globalkeys KF6::Solid)

BUILD_LXQT_PLUGIN(${PLUGIN})
