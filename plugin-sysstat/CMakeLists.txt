cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelsysstat")


set(HEADERS
    lxqtsysstat.h
    lxqtsysstatconfiguration.h
    lxqtsysstatcolours.h
    lxqtsysstatutils.h
)

set(SOURCES
    lxqtsysstat.cpp
    lxqtsysstatconfiguration.cpp
    lxqtsysstatcolours.cpp
    lxqtsysstatutils.cpp
)

set(MOCS
    lxqtsysstat.h
    lxqtsysstatconfiguration.h
    lxqtsysstatcolours.h
)

set(UIS
    lxqtsysstatconfiguration.ui
    lxqtsysstatcolours.ui
)


LINK_LIBRARIES(${PLUGIN} ${SYSSTAT_LIB})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(panelsysstat_LIBRARIES
    sysstat
)
target_link_libraries(${PLUGIN}  ${panelsysstat_LIBRARIES})
