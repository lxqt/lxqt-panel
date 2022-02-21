set(PLUGIN "tray")

include(CheckLibraryExists)

find_package(XCB
    REQUIRED COMPONENTS
        xcb
        xcb-xfixes
        xcb-damage
        xcb-composite
        xcb-randr
        xcb-shm
        xcb-util
        xcb-image
        xcb-shape
)
find_package(PkgConfig)
pkg_check_modules(xtst REQUIRED xtst)

set(HEADERS
    xtestsender.h
    xcbutils.h
    sniproxy.h
    snidbus.h
    fdoselectionmanager.h
    lxqttrayplugin.h
)

set(SOURCES
    xtestsender.cpp
    sniproxy.cpp
    snidbus.cpp
    fdoselectionmanager.cpp
    lxqttrayplugin.cpp
)

qt_add_dbus_adaptor(SOURCES org.kde.StatusNotifierItem.xml sniproxy.h SNIProxy)
qt_add_dbus_interface(SOURCES org.kde.StatusNotifierWatcher.xml statusnotifierwatcher_interface)

set(LIBRARIES
    ${XCB_LIBRARIES}
    ${xtst_LDFLAGS}
)


BUILD_LXQT_PLUGIN(${PLUGIN})
