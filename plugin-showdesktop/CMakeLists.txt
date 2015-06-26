set(PLUGIN "showdesktop")

# Because of a bug in KWindowSystem, we still rely on a XCB function in this plugin
# Later when KWindowSystem is fixed, we can drop xcb dependency
find_package(PkgConfig)
pkg_check_modules(XCB REQUIRED xcb)

set(HEADERS
    showdesktop.h
)

set(SOURCES
    showdesktop.cpp
)

set(LIBRARIES
    ${LIBRARIES}
    lxqt-globalkeys
    Qt5Xdg
    ${XCB_LIBRARIES}
)

BUILD_LXQT_PLUGIN(${PLUGIN})
