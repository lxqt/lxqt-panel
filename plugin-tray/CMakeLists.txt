set(PLUGIN "tray")

include(CheckLibraryExists)

find_package(PkgConfig)
pkg_check_modules(XCB REQUIRED xcb)
pkg_check_modules(XCB_UTIL REQUIRED xcb-util)
pkg_check_modules(XCB_DAMAGE REQUIRED xcb-damage)

find_package(X11 REQUIRED)
pkg_check_modules(XCOMPOSITE REQUIRED xcomposite)
pkg_check_modules(XDAMAGE REQUIRED xdamage)
pkg_check_modules(XRENDER REQUIRED xrender)

set(HEADERS
    lxqttrayplugin.h
    lxqttray.h
    lxqttrayconfiguration.h
    trayicon.h
    xfitman.h
)

set(SOURCES
    lxqttrayplugin.cpp
    lxqttray.cpp
    lxqttrayconfiguration.cpp
    trayicon.cpp
    xfitman.cpp
)

set(LIBRARIES
    ${X11_LIBRARIES}
    ${XCOMPOSITE_LIBRARIES}
    ${XDAMAGE_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XCB_LIBRARIES}
    ${XCB_DAMAGE_LIBRARIES}
)

set(UIS
    lxqttrayconfiguration.ui
)

BUILD_LXQT_PLUGIN(${PLUGIN})
