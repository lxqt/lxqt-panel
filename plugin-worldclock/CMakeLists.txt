cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelworldclock")


set(HEADERS
    lxqtworldclock.h
    lxqtworldclockconfiguration.h
    lxqtworldclockconfigurationtimezones.h
)

set(SOURCES
    lxqtworldclock.cpp
    lxqtworldclockconfiguration.cpp
    lxqtworldclockconfigurationtimezones.cpp
)

set(MOCS
    lxqtworldclock.h
    lxqtworldclockconfiguration.h
    lxqtworldclockconfigurationtimezones.h
)

set(UIS
    lxqtworldclockconfiguration.ui
    lxqtworldclockconfigurationtimezones.ui
)


include_directories(${ICU_INCLUDE_DIRS})
LINK_LIBRARIES(${PLUGIN} ${ICU_LIBRARIES})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
