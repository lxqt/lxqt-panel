# Wayland WM Backend

project(lxqt-panel-backend-wayland)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS WaylandClient Concurrent)

add_library(lxqt-panel-backend-wayland STATIC
    lxqttaskbarbackendwayland.h
    lxqttaskbarplasmawindowmanagment.h
    lxqtplasmavirtualdesktop.h

    lxqttaskbarbackendwayland.cpp
    lxqttaskbarplasmawindowmanagment.cpp
    lxqtplasmavirtualdesktop.cpp
)

qt6_generate_wayland_protocol_client_sources(lxqt-panel-backend-wayland
FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/plasma-window-management.xml
)

qt6_generate_wayland_protocol_client_sources(lxqt-panel-backend-wayland
FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/org-kde-plasma-virtual-desktop.xml
)

target_link_libraries(lxqt-panel-backend-wayland
    Qt6::GuiPrivate
    Qt6::WaylandClient
    Qt6::Concurrent
    lxqt-panel-backend-common
)
