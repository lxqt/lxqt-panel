set(PLUGIN "directorymenu")

set(HEADERS
    directorymenu.h
)

set(SOURCES
    directorymenu.cpp
)

set(MOCS
    directorymenu.h
)

set(LIBRARIES
    ${LIBRARIES}
    ${QTXDG_LIBRARIES}
)

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
