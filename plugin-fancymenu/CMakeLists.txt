set(PLUGIN "fancymenu")

set(HEADERS
    lxqtfancymenu.h
    lxqtfancymenuconfiguration.h
    lxqtfancymenuwindow.h
    lxqtfancymenuappmap.h
    lxqtfancymenuappmodel.h
    lxqtfancymenucategoriesmodel.h
    lxqtfancymenushortcututils.h
    lxqtfancymenutypes.h
)

set(SOURCES
    lxqtfancymenu.cpp
    lxqtfancymenuconfiguration.cpp
    lxqtfancymenuwindow.cpp
    lxqtfancymenuappmap.cpp
    lxqtfancymenuappmodel.cpp
    lxqtfancymenucategoriesmodel.cpp
    lxqtfancymenushortcututils.cpp
)

set(UIS
    lxqtfancymenuconfiguration.ui
)


# optionally use libmenu-cache to generate the application menu
if(USE_MENU_CACHE)
    find_package(MenuCache "0.3.3")
endif()

set(LIBRARIES
    lxqt
    lxqt-globalkeys
    lxqt-globalkeys-ui
)

if(MENUCACHE_FOUND)
    list(APPEND SOURCES xdgcachedmenu.cpp)
    list(APPEND MOCS xdgcachedmenu.h)

    include_directories(${MENUCACHE_INCLUDE_DIRS})
    list(APPEND LIBRARIES ${MENUCACHE_LIBRARIES})
    add_definitions(-DHAVE_MENU_CACHE=1)

endif()


set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

BUILD_LXQT_PLUGIN(${PLUGIN})
