# XCB WM Backend

project(lxqt-panel-backend-xcb)

add_library(lxqt-panel-backend-xcb STATIC
    lxqttaskbarbackend_x11.h
    lxqttaskbarbackend_x11.cpp
)

target_link_libraries(lxqt-panel-backend-xcb
    KF6::WindowSystem
    lxqt-panel-backend-common
)

