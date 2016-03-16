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

# optionally use libmenu-cache to generate the application menu
find_package(PkgConfig)
if(NOT WITHOUT_MENU_CACHE)
    pkg_check_modules(MENU_CACHE
        libmenu-cache>=0.3.3
    )
endif()

if(MENU_CACHE_FOUND)
    list(APPEND SOURCES xdgcachedmenu.cpp)
    list(APPEND MOCS xdgcachedmenu.h)

    include_directories(${MENU_CACHE_INCLUDE_DIRS})
    add_definitions(-DHAVE_MENU_CACHE=1)
endif()

set(LIBRARIES
    lxqt
    lxqt-globalkeys
    lxqt-globalkeys-ui
    ${MENU_CACHE_LIBRARIES}
)

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

BUILD_LXQT_PLUGIN(${PLUGIN})
