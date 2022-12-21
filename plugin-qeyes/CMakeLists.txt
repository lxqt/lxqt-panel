set(PLUGIN "qeyes")

set(HEADERS
    qeyes.h
    qeyesimagewidget.h
    qeyesvectorwidget.h
    qeyeswidget.h
    qeyesconfigdialog.h
)

set(SOURCES
    qeyes.cpp
    qeyesimagewidget.cpp
    qeyesvectorwidget.cpp
    qeyeswidget.cpp
    qeyesconfigdialog.cpp
)

set(LIBRARIES
    ${LIBRARIES}
)

add_subdirectory(types)

BUILD_LXQT_PLUGIN(${PLUGIN})
