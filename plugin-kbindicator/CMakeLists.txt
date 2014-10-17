set(PLUGIN "panelkbindicator")

set(HEADERS
    lxqtkbindicator.h
    lxqtkbindicatorconfiguration.h
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

set(LIBRARIES
    ${LIBRARIES}
    ${X11_LIBRARIES}
    ${XCB_LIBRARIES}
    ${XCB_XCB_LIBRARIES}
)

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
