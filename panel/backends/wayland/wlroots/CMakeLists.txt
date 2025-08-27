set(PLATFORM_NAME wlroots)

set(PREFIX_NAME wmbackend)
set(PROGRAM "lxqt-panel")
set(BACKEND "backend")
set(NAME ${PREFIX_NAME}_${PLATFORM_NAME})
project(${PROGRAM}_${BACKEND}_${NAME})

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS WaylandClient Concurrent)
find_package(Qt6Xdg)

set(PROG_SHARE_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/lxqt/${PROGRAM}/${BACKEND})
set(PLUGIN_SHARE_DIR ${PROG_SHARE_DIR}/${BACKEND}/${NAME})

#************************************************

if (NOT DEFINED PLUGIN_DIR)
    set (PLUGIN_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/${PROGRAM})
endif (NOT DEFINED PLUGIN_DIR)

set(QTX_LIBRARIES Qt6::Gui Qt6::GuiPrivate)

set(
    SRC
    lxqtwmbackend_wlr.cpp lxqtwmbackend_wlr.h
    lxqttaskbarwlrwm.cpp  lxqttaskbarwlrwm.h
    workspace.cpp         workspace.hpp
)

add_library(${NAME} MODULE ${SRC}) # build dynamically loadable modules
install(TARGETS ${NAME} DESTINATION ${PLUGIN_DIR}/${BACKEND}) # install the *.so file

target_link_libraries(${NAME} ${QTX_LIBRARIES} Qt6::Concurrent Qt6::WaylandClient Qt6Xdg)

qt6_generate_wayland_protocol_client_sources(${NAME} FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/wlr-foreign-toplevel-management-unstable-v1.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/ext-workspace-v1.xml
)
