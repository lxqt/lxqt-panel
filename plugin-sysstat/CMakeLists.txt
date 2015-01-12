set(PLUGIN "sysstat")

find_package(SysStat-Qt5 REQUIRED)

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

link_libraries(${PLUGIN} ${SYSSTAT_LIBRARIES})
include_directories("${SYSSTAT_INCLUDE_DIRS}")

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
