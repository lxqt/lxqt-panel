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

if(ICU_I18N_FOUND)
set(UIS
    lxqtworldclockconfiguration_icu.ui
    lxqtworldclockconfigurationtimezones_icu.ui
)
else()
set(UIS
    lxqtworldclockconfiguration.ui
    lxqtworldclockconfigurationtimezones.ui
)
endif()


if(ICU_I18N_FOUND)
  include_directories(${ICU_INCLUDE_DIRS})
  LINK_LIBRARIES(${PLUGIN} ${ICU_LIBRARIES})
endif()

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
