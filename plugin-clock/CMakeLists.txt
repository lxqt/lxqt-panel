cmake_minimum_required(VERSION 2.6)

set(PLUGIN "clock")

include(ConfigureChecks.cmake)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(HEADERS
    lxqtclock.h
    lxqtclockconfiguration.h
    calendar_utils.h
)

set(SOURCES
    lxqtclock.cpp
    lxqtclockconfiguration.cpp
    calendar_utils.cpp
)

set(MOCS
    lxqtclock.h
    lxqtclockconfiguration.h
)

set(UIS
    lxqtclockconfiguration.ui
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
