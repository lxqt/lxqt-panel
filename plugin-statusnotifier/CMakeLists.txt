set(PLUGIN "statusnotifier")

include(CheckLibraryExists)

find_package(PkgConfig)
pkg_check_modules(dbusmenu-qt5 REQUIRED dbusmenu-qt5)
include_directories(${dbusmenu-qt5_INCLUDE_DIRS})

set(QT_USE_QTDBUS 1)
set(QT_USE_QTXML 1)

set(HEADERS
    statusnotifier.h
    dbustypes.h
    statusnotifierbutton.h
    statusnotifieriteminterface.h
    statusnotifierwatcher.h
    statusnotifierwidget.h
)

set(SOURCES
    statusnotifier.cpp
    dbustypes.cpp
    statusnotifierbutton.cpp
    statusnotifieriteminterface.cpp
    statusnotifierwatcher.cpp
    statusnotifierwidget.cpp
)

set(MOCS
    statusnotifier.h
    dbustypes.h
    statusnotifierbutton.h
    statusnotifieriteminterface.h
    statusnotifierwatcher.h
    statusnotifierwidget.h
)

qt5_add_dbus_adaptor(SOURCES
    org.kde.StatusNotifierItem.xml
    statusnotifieriteminterface.h
    StatusNotifierItemInterface
)

set(LIBRARIES
    ${LXQT_LIBRARIES}
    ${QTXDG_LIBRARIES}
    ${dbusmenu-qt5_LIBRARIES}
)

BUILD_LXQT_PLUGIN(${PLUGIN})
