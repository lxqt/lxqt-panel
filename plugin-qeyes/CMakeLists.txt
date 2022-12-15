set(PLUGIN "qeyesplugin")

set(HEADERS
    qeyesimagewidget.h
    qeyesvectorwidget.h
    qeyeswidget.h
)

set(SOURCES
    main.cpp
    qeyesimagewidget.cpp
    qeyesvectorwidget.cpp
    qeyeswidget.cpp
)

set(LIBRARIES
    ${LIBRARIES}
)

BUILD_LXQT_PLUGIN(${PLUGIN})
