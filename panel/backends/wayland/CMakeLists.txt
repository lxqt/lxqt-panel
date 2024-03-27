add_subdirectory(plasma)
add_subdirectory(wlroots)

add_library(lxqt-panel-backend-wayland STATIC
    lxqttaskbarbackendwayland.h
    lxqttaskbarbackendwayland.cpp
)

target_link_libraries(lxqt-panel-backend-wayland
    Qt6::GuiPrivate
    lxqt-panel-backend-common
    lxqt-panel-backend-plasma
    lxqt-panel-backend-wlroots
)
