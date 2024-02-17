set(PLUGIN "taskbar")

set(HEADERS
    lxqttaskbar.h
    lxqttaskbutton.h
    lxqttaskbarconfiguration.h
    lxqttaskbarplugin.h
    lxqttaskgroup.h
    lxqtgrouppopup.h

    ilxqttaskbarabstractbackend.h
    lxqttaskbartypes.h
    lxqttaskbarproxymodel.h

    backends/xcb/lxqttaskbarbackend_x11.h
)

set(SOURCES
    lxqttaskbar.cpp
    lxqttaskbutton.cpp
    lxqttaskbarconfiguration.cpp
    lxqttaskbarplugin.cpp
    lxqttaskgroup.cpp
    lxqtgrouppopup.cpp

    ilxqttaskbarabstractbackend.cpp
    lxqttaskbarproxymodel.cpp

    backends/xcb/lxqttaskbarbackend_x11.cpp
)

set(UIS
    lxqttaskbarconfiguration.ui
)

set(LIBRARIES
    lxqt
    lxqt-globalkeys
    Qt5Xdg
)

BUILD_LXQT_PLUGIN(${PLUGIN})
