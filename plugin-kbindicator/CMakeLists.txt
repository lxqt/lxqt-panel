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

find_package(XCB REQUIRED COMPONENTS xcb xcb-xkb)
find_package(XKBCommon REQUIRED COMPONENTS XKBCommon X11)
find_package(X11 REQUIRED)
find_package(Qt5 ${QT_MINIMUM_VERSION} REQUIRED COMPONENTS X11Extras Xml)

if(NOT X11_Xkbfile_FOUND)
    MESSAGE(FATAL_ERROR "'libxkbfile' not found. Aborting")
endif()

find_package(PkgConfig REQUIRED)
execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=xkb_base xkeyboard-config
    OUTPUT_VARIABLE XKB_RULES_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT EXISTS "${XKB_RULES_DIR}")
    message(FATAL_ERROR "Couldn't find XKB rules location: \"${XKB_RULES_DIR}\".")
endif()

message(STATUS "X11_xkbfile_INCLUDE_PATH: ${X11_xkbfile_INCLUDE_PATH}")
message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}")

include_directories(
    ${XCB_INCLUDE_DIRS}
    ${X11_xkbfile_INCLUDE_PATH}
)

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
    ${XCB_LIBRARIES}
    ${X11_xkbfile_LIB}
    XKBCommon::XKBCommon
    XKBCommon::X11
    Qt5::Xml
)

add_definitions(-DX11_ENABLED -DXKB_RULES_DIR=\"${XKB_RULES_DIR}\")


BUILD_LXQT_PLUGIN(${PLUGIN})
