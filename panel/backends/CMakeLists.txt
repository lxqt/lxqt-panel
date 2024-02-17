# Common interface for Window Manager abstraction backend
# This also contains dummy backend

add_library(lxqt-panel-backend-common STATIC

    lxqttaskbartypes.h
    ilxqtabstractwmiface.h
    ilxqtabstractwmiface.cpp

    lxqtdummywmbackend.h
    lxqtdummywmbackend.cpp
)

target_link_libraries(lxqt-panel-backend-common
    Qt6::Gui
)

add_subdirectory(wayland)
add_subdirectory(xcb)
