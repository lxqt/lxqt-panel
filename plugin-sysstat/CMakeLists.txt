set(PLUGIN "sysstat")

find_package(SysStat-Qt5 REQUIRED)

set(HEADERS
    lxqtsysstat.h
    lxqtsysstatconfiguration.h
    lxqtsysstatcolors.h
    lxqtsysstatutils.h
)

set(SOURCES
    lxqtsysstat.cpp
    lxqtsysstatconfiguration.cpp
    lxqtsysstatcolors.cpp
    lxqtsysstatutils.cpp
)

set(UIS
    lxqtsysstatconfiguration.ui
    lxqtsysstatcolors.ui
)

set(LIBRARIES sysstat-qt5)

BUILD_LXQT_PLUGIN(${PLUGIN})
