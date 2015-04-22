set(PLUGIN "mount")

set(HEADERS
    lxqtmountplugin.h
    button.h
    menudiskitem.h
    popup.h
)

set(SOURCES
    lxqtmountplugin.cpp
    button.cpp
    menudiskitem.cpp
    popup.cpp
)

find_package(KF5Solid REQUIRED)
set(LIBRARIES ${QTXDG_LIBRARIES} KF5::Solid)

BUILD_LXQT_PLUGIN(${PLUGIN})
