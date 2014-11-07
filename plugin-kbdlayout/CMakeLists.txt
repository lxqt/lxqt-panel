cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

set(PLUGIN "panelkbdlayout")
add_definitions(-std=c++11)

set(HEADERS
    lxqtkbdlayoutwidget.h
    lxqtkbdlayoutplugin.h
    lxqtkbdlayout.h
    lxqtkbdinfo.h
    lxqtkbdlayoutconf.h
    lxqtkbdkeeper.h
)

set(SOURCES
    lxqtkbdlayoutplugin.cpp
    lxqtkbdlayoutwidget.cpp
    lxqtkbdlayout.cpp
    lxqtkbdinfo.cpp
    lxqtkbdlayoutconf.cpp
    lxqtkbdkeeper.cpp
)

set(MOCS
    lxqtkbdlayoutwidget.h
    lxqtkbdlayoutplugin.h
    lxqtkbdlayout.h
    lxqtkbdlayoutconf.h
    lxqtkbdkeeper.h
)

set(UIS
    lxqtkbdlayoutconf.ui
)


set(LIBRARIES ${LIBRARIES} ${X11_LIBRARIES} ${XCB_LIBRARIES} ${XCB_XCB_LIBRARIES})

#*******************************************
include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
