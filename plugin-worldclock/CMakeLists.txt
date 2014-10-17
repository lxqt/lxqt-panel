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

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
