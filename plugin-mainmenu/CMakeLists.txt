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
)

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

BUILD_LXQT_PLUGIN(${PLUGIN})
