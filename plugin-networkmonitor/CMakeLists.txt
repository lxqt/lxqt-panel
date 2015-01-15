set(PLUGIN "networkmonitor")

set(HEADERS
    lxqtnetworkmonitorplugin.h
    lxqtnetworkmonitor.h
    lxqtnetworkmonitorconfiguration.h
)

set(SOURCES
    lxqtnetworkmonitorplugin.cpp
    lxqtnetworkmonitor.cpp
    lxqtnetworkmonitorconfiguration.cpp
)

set(MOCS
    lxqtnetworkmonitorplugin.h
    lxqtnetworkmonitor.h
    lxqtnetworkmonitorconfiguration.h
)

set(UIS
    lxqtnetworkmonitorconfiguration.ui
)

set(RESOURCES
    resources.qrc
)

set(LIBRARIES ${STATGRAB_LIB})

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
