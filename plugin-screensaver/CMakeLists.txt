set(PLUGIN "screensaver")

set(HEADERS
    panelscreensaver.h
)

set(SOURCES
    panelscreensaver.cpp
)

set(LIBRARIES
    ${LIBRARIES}
    lxqt-globalkeys
)

BUILD_LXQT_PLUGIN(${PLUGIN})
