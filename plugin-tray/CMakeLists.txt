set(PLUGIN "tray")

include(FindPkgConfig)
include(CheckLibraryExists)

find_package(X11 REQUIRED)
pkg_check_modules(XCOMPOSITE REQUIRED xcomposite)
pkg_check_modules(XDAMAGE REQUIRED xdamage)
pkg_check_modules(XRENDER REQUIRED xrender)

set(HEADERS
    lxqttrayplugin.h
    lxqttray.h
    trayicon.h
)

set(SOURCES
    lxqttrayplugin.cpp
    lxqttray.cpp
    trayicon.cpp
)

set(MOCS
    lxqttrayplugin.h
    lxqttray.h
    trayicon.h
)

set(LIBRARIES
    ${X11_LIBRARIES}
    ${XCOMPOSITE_LIBRARIES}
    ${XDAMAGE_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XCB_LIBRARIES}
    ${XCB_DAMAGE_LIBRARIES}
)

include("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
