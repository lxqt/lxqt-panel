set(PLUGIN "desktopswitch")

set(HEADERS
   desktopswitch.h
    desktopswitchbutton.h
)

set(SOURCES
    desktopswitch.cpp
    desktopswitchbutton.cpp
)

set(MOCS
    desktopswitch.h
    desktopswitchbutton.h
)

set(LIBRARIES ${LIBRARIES} ${LXQT_LIBRARIES} ${LXQT_GLOBALKEYS_LIBRARIES} )

BUILD_LXQT_PLUGIN(${PLUGIN})
