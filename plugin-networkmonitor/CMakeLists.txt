cmake_minimum_required(VERSION 2.6)

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

LINK_LIBRARIES(${PLUGIN} ${STATGRAB_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
