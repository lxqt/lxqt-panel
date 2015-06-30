set(PLUGIN "showdesktop")

set(HEADERS
    showdesktop.h
)

set(SOURCES
    showdesktop.cpp
)

set(LIBRARIES
    ${LIBRARIES}
    ${LXQT_GLOBALKEYS_LIBRARIES}
    ${QTXDG_LIBRARIES}
)

BUILD_LXQT_PLUGIN(${PLUGIN})
