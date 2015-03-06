set(PLUGIN "kbindicator")

find_package(KF5GuiAddons REQUIRED)
find_package(PkgConfig REQUIRED QUIET)

set(HEADERS
    lxqtkbindicator.h
    lxqtkbindicatorconfiguration.h
    kbdlayout/lxqtkbdlayout.h
)

set(SOURCES
    lxqtkbindicator.cpp
    lxqtkbindicatorconfiguration.cpp
)

set(MOCS
    lxqtkbindicator.h
    lxqtkbindicatorconfiguration.h
)

set(UIS
    lxqtkbindicatorconfiguration.ui
)

pkg_check_modules(XCB_XCB xcb-xkb)
find_package(X11)

if(XCB_XCB_FOUND AND X11_FOUND)
    message(STATUS "Keyboard layout control will be built")

    set(HEADERS ${HEADERS}
        kbdlayout/lxqtkbdinfo.h
        kbdlayout/lxqtkbdkeeper.h
    )

    set(SOURCES ${SOURCES}
        kbdlayout/lxqtkbdinfo.cpp
        kbdlayout/lxqtkbdlayout.cpp
        kbdlayout/lxqtkbdkeeper.cpp
    )

    set(MOCS ${MOCS}
        kbdlayout/lxqtkbdkeeper.h
    )

    set(LIBRARIES ${LIBRARIES}
        ${XCB_XCB_LIBRARIES}
        ${X11_LIBRARIES}
    )

    add_definitions(-DENABLE_KBDLAYOUT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(LIBRARIES
    ${LIBRARIES}
    KF5::GuiAddons
)

BUILD_LXQT_PLUGIN(${PLUGIN})
