set(PLUGIN "screensaver")

set(HEADERS
    panelscreensaver.h
)

set(SOURCES
    panelscreensaver.cpp
)

set(LIBRARIES
    ${LIBRARIES}
    ${LXQT_GLOBALKEYS_LIBRARIES}
)

BUILD_LXQT_PLUGIN(${PLUGIN})
