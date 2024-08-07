set(PROJECT lxqt-panel)

# Window Manager abstraction backend
add_subdirectory(backends)

# TODO: allow compile time selection via CMake variables
set(PANEL_BACKENDS
    lxqt-panel-backend-wlroots
    lxqt-panel-backend-wayland
    lxqt-panel-backend-xcb
)


set(PRIV_HEADERS
    panelpluginsmodel.h
    windownotifier.h
    lxqtpanel.h
    lxqtpanelapplication.h
    lxqtpanelapplication_p.h
    lxqtpanellayout.h
    plugin.h
    pluginsettings_p.h
    lxqtpanellimits.h
    popupmenu.h
    pluginmoveprocessor.h
    lxqtpanelpluginconfigdialog.h
    config/configpaneldialog.h
    config/configplacement.h
    config/configstyling.h
    config/configpluginswidget.h
    config/addplugindialog.h
)

# using LXQt namespace in the public headers.
set(PUB_HEADERS
    lxqtpanelglobals.h
    pluginsettings.h
    ilxqtpanelplugin.h
    ilxqtpanel.h
)

set(SOURCES
    main.cpp
    panelpluginsmodel.cpp
    windownotifier.cpp
    lxqtpanel.cpp
    lxqtpanelapplication.cpp
    lxqtpanellayout.cpp
    plugin.cpp
    pluginsettings.cpp
    popupmenu.cpp
    pluginmoveprocessor.cpp
    lxqtpanelpluginconfigdialog.cpp
    config/configpaneldialog.cpp
    config/configplacement.cpp
    config/configstyling.cpp
    config/configpluginswidget.cpp
    config/addplugindialog.cpp
)

set(UI
    config/configplacement.ui
    config/configstyling.ui
    config/configpluginswidget.ui
    config/addplugindialog.ui
)

set(LIBRARIES
    lxqt
)

file(GLOB CONFIG_FILES resources/*.conf)

############################################

add_definitions(-DCOMPILE_LXQT_PANEL)

set(PLUGIN_DESKTOPS_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/lxqt/${PROJECT}")
add_definitions(-DPLUGIN_DESKTOPS_DIR=\"${PLUGIN_DESKTOPS_DIR}\")

if (WITH_SCREENSAVER_FALLBACK)
    message(STATUS "Building with conversion of deprecated 'screensaver' plugin")
    add_definitions(-DWITH_SCREENSAVER_FALLBACK "-DLXQT_LOCK_DESKTOP=\"${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications/lxqt-lockscreen.desktop\"")
endif ()

project(${PROJECT})

set(QTX_LIBRARIES Qt6::Widgets Qt6::Xml Qt6::DBus)

# Translations
lxqt_translate_ts(QM_FILES SOURCES
    UPDATE_TRANSLATIONS
        ${UPDATE_TRANSLATIONS}
    SOURCES
        ${PUB_HEADERS}
        ${PRIV_HEADERS}
        ${SOURCES}
        ${UI}
    INSTALL_DIR
        "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
)

lxqt_app_translation_loader(SOURCES ${PROJECT_NAME})

add_executable(${PROJECT}
    ${PUB_HEADERS}
    ${PRIV_HEADERS}
    ${QM_FILES}
    ${SOURCES}
    ${UI}
)

target_link_libraries(${PROJECT}
    ${LIBRARIES}
    ${QTX_LIBRARIES}
    KF6::WindowSystem
    ${PANEL_BACKENDS}
    LayerShellQt::Interface
    ${STATIC_PLUGINS}
)

set_property(TARGET ${PROJECT} PROPERTY ENABLE_EXPORTS TRUE)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES ${CONFIG_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/lxqt)
install(FILES ${PUB_HEADERS} DESTINATION include/lxqt)
install(FILES
    man/lxqt-panel.1
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
    COMPONENT Runtime
)
