set(PROJECT lxqt-panel)

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

# using LXQt namespace in the public headers.
set(lxqt-panel_PUB_H_FILES
    lxqtpanelglobals.h
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

set(RESOURCES "")

set(lxqt-panel_UI_FILES
    config/configpaneldialog.ui
)

qt5_wrap_ui(UI_HEADERS ${lxqt-panel_UI_FILES})

file(GLOB CONFIG_FILES resources/*.conf)

############################################

add_definitions(-DCOMPILE_LXQT_PANEL)

set(PLUGIN_DESKTOPS_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/lxqt/${PROJECT}")
add_definitions(-DPLUGIN_DESKTOPS_DIR=\"${PLUGIN_DESKTOPS_DIR}\")

project(${PROJECT})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LXQT_INCLUDE_DIRS}
    ${LXQT_INCLUDE_DIRS}/LXQt # FIXME: liblxqt seems to be broken, workaround it.
)

qt5_wrap_cpp(MOC_SOURCES ${MOCS})
qt5_wrap_ui(UI_HEADERS ${FORMS})
qt5_add_resources(QRC_SOURCES ${RESOURCES})
set(QTX_LIBRARIES Qt5::Widgets Qt5::Xml Qt5::DBus)

# Translations
lxqt_translate_ts(lxqt-runner_QM_FILES SOURCES
    UPDATE_TRANSLATIONS
        ${UPDATE_TRANSLATIONS}
    SOURCES
        ${lxqt-panel_PUB_H_FILES}
        ${lxqt-panel_PRIV_H_FILES}
        ${lxqt-panel_CPP_FILES}
        ${lxqt-panel_UI_FILES}
    INSTALL_DIR
        "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
)

lxqt_app_translation_loader(QM_LOADER ${PROJECT_NAME})


set(PLUGIN_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/lxqt-panel")
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

add_executable(${PROJECT} ${lxqt-panel_PUB_H_FILES} ${lxqt-panel_PRIV_H_FILES} ${lxqt-panel_CPP_FILES} ${MOC_SOURCES} ${lxqt-runner_QM_FILES} ${QRC_SOURCES} ${UI_HEADERS} ${QM_LOADER})
target_link_libraries(${PROJECT} ${LIBRARIES} ${QTX_LIBRARIES} KF5::WindowSystem)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES ${CONFIG_FILES} DESTINATION ${LXQT_ETC_XDG_DIR}/lxqt)
install(FILES ${lxqt-panel_PUB_H_FILES} DESTINATION include/lxqt)
