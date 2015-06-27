set(PLUGIN "kbindicator")

set(HEADERS
    src/kbdstate.h
    src/settings.h
    src/content.h
    src/kbdlayout.h
    src/kbdinfo.h
    src/kbdkeeper.h
    src/kbdwatcher.h
    src/controls.h
    src/kbdstateconfig.h
)

set(SOURCES
    kbindicator-plugin.cpp
    src/kbdstate.cpp
    src/settings.cpp
    src/content.cpp
    src/kbdkeeper.cpp
    src/kbdwatcher.cpp
    src/kbdstateconfig.cpp
)

set(UIS
    src/kbdstateconfig.ui
)

set(LIBRARIES
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(XKB_COMMON REQUIRED xkbcommon)
pkg_check_modules(XKB_COMMON_X11 QUIET xkbcommon-x11)

if(XKB_COMMON_X11_FOUND)
    message(STATUS "XkbCommon X11 was found")
    find_package(Qt5 COMPONENTS X11Extras Xml)
    pkg_check_modules(XCB_XCB xcb-xkb)

    set(HEADERS
        ${HEADERS}
        src/x11/kbdlayout.h
    )

    set(SOURCES
        ${SOURCES}
        src/x11/kbdlayout.cpp
    )

    set(LIBRARIES
        ${LIBRARIES}
        ${XKB_COMMON_X11_LIBRARIES}
        ${XCB_XCB_LIBRARIES}
        Qt5::Xml
    )

    add_definitions(-DX11_ENABLED)
else()
    message(FATAL_ERROR "No XkbCommon backend(X11) found!")
endif()

BUILD_LXQT_PLUGIN(${PLUGIN})

