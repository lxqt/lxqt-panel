set(PLUGIN "dom")

set(HEADERS
    domplugin.h
    treewindow.h
    domtreeitem.h
)

set(SOURCES
    domplugin.cpp
    treewindow.cpp
    domtreeitem.cpp
)

set(MOCS
    domplugin.h
    treewindow.h
    domtreeitem.h
)

set(UIS
    treewindow.ui
)

set(RESOURCES
    resources.qrc
)

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
