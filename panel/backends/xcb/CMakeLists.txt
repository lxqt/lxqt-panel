set(NAME xcb_backend)
set(PROGRAM "lxqt-panel")
set(BACKEND "backend")
project(${PROGRAM}_${BACKEND}_${NAME})

set(PROG_SHARE_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/lxqt/${PROGRAM}/${BACKEND})
set(PLUGIN_SHARE_DIR ${PROG_SHARE_DIR}/${BACKEND}/${NAME})
#************************************************

if (NOT DEFINED PLUGIN_DIR)
    set (PLUGIN_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/${PROGRAM})
endif (NOT DEFINED PLUGIN_DIR)

set(QTX_LIBRARIES Qt6::Gui)

set(SRC lxqtwmbackend_x11.h lxqtwmbackend_x11.cpp)

add_library(${NAME} MODULE ${SRC}) # build dynamically loadable modules
install(TARGETS ${NAME} DESTINATION ${PLUGIN_DIR}/${BACKEND}) # install the *.so file

target_link_libraries(${NAME} ${QTX_LIBRARIES} KF6::WindowSystem)
