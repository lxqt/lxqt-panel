cmake_minimum_required(VERSION 2.6)

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

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
