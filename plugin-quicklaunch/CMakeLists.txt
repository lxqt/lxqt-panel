set(PLUGIN "quicklaunch")

set(HEADERS
    lxqtquicklaunchplugin.h
    lxqtquicklaunch.h
    quicklaunchbutton.h
    quicklaunchaction.h
)

set(SOURCES
    lxqtquicklaunchplugin.cpp
    lxqtquicklaunch.cpp
    quicklaunchbutton.cpp
    quicklaunchaction.cpp
)

set(MOCS
    lxqtquicklaunchplugin.h
    lxqtquicklaunch.h
    quicklaunchbutton.h
    quicklaunchaction.h
)

set(LIBRARIES
    ${QTXDG_LIBRARIES}
)

include_directories(
    ${LXQT_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../panel"
)

BUILD_LXQT_PLUGIN(${PLUGIN})
