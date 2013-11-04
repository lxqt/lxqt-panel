cmake_minimum_required( VERSION 2.6 )

set(PROJECT lxqt-panel )

set(QT_USE_QTXML 1)
set(QT_USE_QTDBUS 1)


set(lxqt-panel_PRIV_H_FILES
    lxqtpanel.h
    lxqtpanelapplication.h
    lxqtpanellayout.h
    lxqtpanelpluginconfigdialog.h
    config/configpaneldialog.h
    plugin.h
    lxqtpanellimits.h
    popupmenu.h
    pluginmoveprocessor.h
)


# FIXME: we need to rename public headers to lxqt* and
# using LxQt namespace in the public headers.
set(lxqt-panel_PUB_H_FILES
    ilxqtpanelplugin.h
    ilxqtpanel.h
)

set(lxqt-panel_CPP_FILES
    main.cpp
    lxqtpanel.cpp
    lxqtpanelapplication.cpp
    lxqtpanellayout.cpp
    lxqtpanelpluginconfigdialog.cpp
    config/configpaneldialog.cpp
    plugin.cpp
    popupmenu.cpp
    pluginmoveprocessor.cpp
)

set(MOCS
    lxqtpanel.h
    lxqtpanelapplication.h
    lxqtpanellayout.h
    lxqtpanelpluginconfigdialog.h
    config/configpaneldialog.h
    plugin.h
    pluginmoveprocessor.h
)

set(LIBRARIES
    ${LXQT_LIBRARIES}
    ${QTXDG_LIBRARIES}
)

# QRC files .................
set(RESOURCES

)

set (lxqt-panel_UI_FILES
    config/configpaneldialog.ui
)

QT4_WRAP_UI(UI_HEADERS ${lxqt-panel_UI_FILES})

add_definitions(-DDEFULT_PANEL_PLUGINS=\"MainMenu,Quicklaunch,TaskBar,Tray,Clock\")

include(LxQtTranslate)

file (GLOB CONFIG_FILES     resources/*.conf)

############################################

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions (-Wall)
#    add_definitions (-Wall -pedantic)
endif()


set(PLUGIN_DESKTOPS_DIR
    ${CMAKE_INSTALL_PREFIX}/share/lxqt/${PROJECT}
)

add_definitions(-DPLUGIN_DESKTOPS_DIR=\"${PLUGIN_DESKTOPS_DIR}\")


#*******************************************
project(${PROJECT})

find_package(Qt4 REQUIRED)
#find_package ( X11 REQUIRED )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

include(${QT_USE_FILE})
qt4_wrap_cpp(MOC_SOURCES ${MOCS})
qt4_wrap_ui(UI_HEADERS ${FORMS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})

# Translations **********************************
include(LxQtTranslate)
lxqt_translate_ts(lxqt-runner_QM_FILES 
    SOURCES
        ${lxqt-panel_PUB_H_FILES}
        ${lxqt-panel_PRIV_H_FILES}
        ${lxqt-panel_CPP_FILES} 
        ${lxqt-panel_UI_FILES}
)
#************************************************


set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/lxqt-panel/)
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

add_executable(${PROJECT} ${lxqt-panel_PUB_H_FILES} ${lxqt-panel_PRIV_H_FILES} ${lxqt-panel_CPP_FILES} ${MOC_SOURCES} ${lxqt-runner_QM_FILES} ${QRC_SOURCES} ${UI_HEADERS})
target_link_libraries(${PROJECT} ${LIBRARIES} ${QT_LIBRARIES})

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES   ${CONFIG_FILES}    DESTINATION ${LXQT_ETC_XDG_DIR}/lxqt)
install(FILES  ${lxqt-panel_PUB_H_FILES} DESTINATION include/lxqt)
