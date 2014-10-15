cmake_minimum_required(VERSION 2.6)

set(PLUGIN "clock")

set(HEADERS
    lxqtclock.h
    lxqtclockconfiguration.h
    calendarpopup.h
)

set(SOURCES
    lxqtclock.cpp
    lxqtclockconfiguration.cpp
    calendarpopup.cpp
)

set(MOCS
    lxqtclock.h
    lxqtclockconfiguration.h
    calendarpopup.h
)

set(UIS
    lxqtclockconfiguration.ui
)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
