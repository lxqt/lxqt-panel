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
    Qt6::Xml
    Qt6::DBus
)

BUILD_LXQT_PLUGIN(${PLUGIN})
