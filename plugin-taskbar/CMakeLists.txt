cmake_minimum_required(VERSION 2.6)

set(PLUGIN "taskbar")


set(HEADERS
    lxqttaskbar.h
    lxqttaskbutton.h
    lxqttaskbarconfiguration.h
    lxqttaskbarplugin.h
)

set(SOURCES
    lxqttaskbar.cpp
    lxqttaskbutton.cpp
    lxqttaskbarconfiguration.cpp
    lxqttaskbarplugin.cpp
)

set(MOCS
    lxqttaskbar.h
    lxqttaskbutton.h
    lxqttaskbarconfiguration.h
    lxqttaskbarplugin.h
)

set(UIS
    lxqttaskbarconfiguration.ui
)

set(LIBRARIES
    ${LXQT_LIBRARIES}
)


#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
