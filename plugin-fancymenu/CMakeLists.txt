set(PLUGIN "fancymenu")

set(HEADERS
    lxqtfancymenu.h
    lxqtfancymenuconfiguration.h
    lxqtfancymenuwindow.h
    lxqtfancymenuappmap.h
    lxqtfancymenuappmodel.h
    lxqtfancymenucategoriesmodel.h
    lxqtfancymenutypes.h
)

set(SOURCES
    lxqtfancymenu.cpp
    lxqtfancymenuconfiguration.cpp
    lxqtfancymenuwindow.cpp
    lxqtfancymenuappmap.cpp
    lxqtfancymenuappmodel.cpp
    lxqtfancymenucategoriesmodel.cpp
)

set(UIS
    lxqtfancymenuconfiguration.ui
)

set(LIBRARIES
    lxqt
    lxqt-globalkeys
    lxqt-globalkeys-ui
)

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

BUILD_LXQT_PLUGIN(${PLUGIN})
