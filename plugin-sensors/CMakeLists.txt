cmake_minimum_required(VERSION 2.6)

set(PLUGIN "sensors")


set(HEADERS
    lxqtsensorsplugin.h
    chip.h
    feature.h
    lxqtsensors.h
    lxqtsensorsconfiguration.h
    sensors.h
)

set(SOURCES
    lxqtsensorsplugin.cpp
    chip.cpp
    feature.cpp
    lxqtsensors.cpp
    lxqtsensorsconfiguration.cpp
    sensors.cpp
)

set(MOCS
    lxqtsensorsplugin.h
    lxqtsensors.h
    lxqtsensorsconfiguration.h
)

set(UIS
  lxqtsensorsconfiguration.ui
)

LINK_LIBRARIES(${PLUGIN} ${SENSORS_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
