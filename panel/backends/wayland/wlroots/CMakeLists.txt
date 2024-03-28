# Wayland WM Backend

project(lxqt-panel-backend-wlroots)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS WaylandClient Concurrent)

add_library(lxqt-panel-backend-wlroots STATIC
    lxqttaskbarbackendwlr.h
    lxqttaskbarwlrwindowmanagment.h
    lxqtwlrvirtualdesktop.h

    lxqttaskbarbackendwlr.cpp
    lxqttaskbarwlrwindowmanagment.cpp
    lxqtwlrvirtualdesktop.cpp
)

qt6_generate_wayland_protocol_client_sources(lxqt-panel-backend-wlroots
FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/wlr-foreign-toplevel-management-unstable-v1.xml
)

target_link_libraries(lxqt-panel-backend-wlroots
    Qt6::GuiPrivate
    Qt6::WaylandClient
    Qt6::Concurrent
    Qt6Xdg
    lxqt-panel-backend-common
)
