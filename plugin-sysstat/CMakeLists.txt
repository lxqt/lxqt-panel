set(PLUGIN "sysstat")

find_package(SysStat-Qt6 REQUIRED)

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

set(UIS
    lxqtsysstatconfiguration.ui
    lxqtsysstatcolours.ui
)

set(LIBRARIES sysstat-qt6)

BUILD_LXQT_PLUGIN(${PLUGIN})
