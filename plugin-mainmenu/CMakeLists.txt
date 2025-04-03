set(PLUGIN "mainmenu")

set(HEADERS
    actionview.h
    lxqtmainmenu.h
    menustyle.h
    lxqtmainmenuconfiguration.h
)

set(SOURCES
    actionview.cpp
    lxqtmainmenu.cpp
    menustyle.cpp
    lxqtmainmenuconfiguration.cpp
)

set(UIS
    lxqtmainmenuconfiguration.ui
)

set(LIBRARIES
    lxqt
    lxqt-globalkeys
    lxqt-globalkeys-ui
    Qt6::Xml
    Qt6::DBus
)

BUILD_LXQT_PLUGIN(${PLUGIN})
