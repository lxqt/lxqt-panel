# Wayland WM Backend

project(lxqt-panel-backend-plasma)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS WaylandClient Concurrent)

add_library(lxqt-panel-backend-plasma STATIC
    lxqttaskbarbackendplasma.h
    lxqttaskbarplasmawindowmanagment.h
    lxqtplasmavirtualdesktop.h

    lxqttaskbarbackendplasma.cpp
    lxqttaskbarplasmawindowmanagment.cpp
    lxqtplasmavirtualdesktop.cpp
)

qt6_generate_wayland_protocol_client_sources(lxqt-panel-backend-plasma
FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/plasma-window-management.xml
)

qt6_generate_wayland_protocol_client_sources(lxqt-panel-backend-plasma
FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/org-kde-plasma-virtual-desktop.xml
)

target_link_libraries(lxqt-panel-backend-plasma
    Qt6::GuiPrivate
    Qt6::WaylandClient
    Qt6::Concurrent
    lxqt-panel-backend-common
)
