set(PLUGIN "panelworldclock")

set(HEADERS
    lxqtworldclock.h
    lxqtworldclockconfiguration.h
    lxqtworldclockconfigurationtimezones.h
    lxqtworldclockconfigurationmanualformat.h
)

set(SOURCES
    lxqtworldclock.cpp
    lxqtworldclockconfiguration.cpp
    lxqtworldclockconfigurationtimezones.cpp
    lxqtworldclockconfigurationmanualformat.cpp
)

set(MOCS
    lxqtworldclock.h
    lxqtworldclockconfiguration.h
    lxqtworldclockconfigurationtimezones.h
    lxqtworldclockconfigurationmanualformat.h
)

set(UIS
    lxqtworldclockconfiguration.ui
    lxqtworldclockconfigurationtimezones.ui
    lxqtworldclockconfigurationmanualformat.ui
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
