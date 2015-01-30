set(PLUGIN "colorpicker")

set(HEADERS
    colorpicker.h
)

set(SOURCES
    colorpicker.cpp
)

set(MOCS
    colorpicker.h
)

set(UIS "")

set(LIBRARIES
    ${LXQT_LIBRARIES}
    ${QTXDG_LIBRARIES}
)

BUILD_LXQT_PLUGIN(${PLUGIN})
