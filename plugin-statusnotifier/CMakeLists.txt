set(PLUGIN "statusnotifier")


find_package(dbusmenu-qt5 REQUIRED QUIET)

set(HEADERS
    statusnotifier.h
    dbustypes.h
    statusnotifierbutton.h
    statusnotifieriteminterface.h
    statusnotifierwatcher.h
    statusnotifierwidget.h
    sniasync.h
)

set(SOURCES
    statusnotifier.cpp
    dbustypes.cpp
    statusnotifierbutton.cpp
    statusnotifieriteminterface.cpp
    statusnotifierwatcher.cpp
    statusnotifierwidget.cpp
    sniasync.cpp
)

qt5_add_dbus_adaptor(SOURCES
    org.kde.StatusNotifierItem.xml
    statusnotifieriteminterface.h
    StatusNotifierItemInterface
)

set(LIBRARIES
    lxqt
    dbusmenu-qt5
)

BUILD_LXQT_PLUGIN(${PLUGIN})
