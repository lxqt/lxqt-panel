set(PLUGIN "cpuload")

set(HEADERS
    lxqtcpuloadplugin.h
    lxqtcpuload.h
    lxqtcpuloadconfiguration.h
)

set(SOURCES
    lxqtcpuloadplugin.cpp
    lxqtcpuload.cpp
    lxqtcpuloadconfiguration.cpp
)

set(MOCS
    lxqtcpuloadplugin.h
    lxqtcpuload.h
    lxqtcpuloadconfiguration.h
)

set(UIS
    lxqtcpuloadconfiguration.ui
)

LINK_LIBRARIES(${PLUGIN} ${STATGRAB_LIB})

include("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
