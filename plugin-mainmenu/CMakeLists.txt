cmake_minimum_required(VERSION 2.6)

# optionally use libmenu-cache from lxde to generate the application menu
find_package(PkgConfig)
pkg_check_modules(MENU_CACHE
    libmenu-cache>=0.3.3
)

set(PLUGIN "mainmenu")


set(HEADERS
    razormainmenu.h
    menustyle.h
    razormainmenuconfiguration.h
)

set(SOURCES
    razormainmenu.cpp
    menustyle.cpp
    razormainmenuconfiguration.cpp
)

set(MOCS
    razormainmenu.h
    menustyle.h
    razormainmenuconfiguration.h
)

set(UIS
    razormainmenuconfiguration.ui
)

# if libmenu-cache is available
if (MENU_CACHE_FOUND)
  list(APPEND SOURCES xdgcachedmenu.cpp)
  list(APPEND MOCS xdgcachedmenu.h)
  
  include_directories(${MENU_CACHE_INCLUDE_DIRS})
  add_definitions(-DHAVE_MENU_CACHE=1)

endif (MENU_CACHE_FOUND)

set(LIBRARIES
  ${LXQT_LIBRARIES}
  ${LXQT_GLOBALKEYS_LIBRARIES}
  ${LXQT_GLOBALKEYS_UI_LIBRARIES}
  ${MENU_CACHE_LIBRARIES}
)

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_RAZOR_PLUGIN(${PLUGIN})
