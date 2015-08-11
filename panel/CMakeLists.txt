set(PROJECT lxqt-panel)

set(PRIV_HEADERS
    panelpluginsmodel.h
    lxqtpanel.h
    lxqtpanelapplication.h
    lxqtpanellayout.h
    plugin.h
    lxqtpanellimits.h
    popupmenu.h
    pluginmoveprocessor.h
    lxqtpanelpluginconfigdialog.h
    config/configpaneldialog.h
    config/configpanelwidget.h
    config/configpluginswidget.h
    config/addplugindialog.h
)

# using LXQt namespace in the public headers.
set(PUB_HEADERS
    lxqtpanelglobals.h
    ilxqtpanelplugin.h
    ilxqtpanel.h
)

set(SOURCES
    main.cpp
    panelpluginsmodel.cpp
    lxqtpanel.cpp
    lxqtpanelapplication.cpp
    lxqtpanellayout.cpp
    plugin.cpp
    popupmenu.cpp
    pluginmoveprocessor.cpp
    lxqtpanelpluginconfigdialog.cpp
    config/configpaneldialog.cpp
    config/configpanelwidget.cpp
    config/configpluginswidget.cpp
    config/addplugindialog.cpp
)

set(UI
    config/configpanelwidget.ui
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

project(${PROJECT})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(QTX_LIBRARIES Qt5::Widgets Qt5::Xml Qt5::DBus)

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
    KF5::WindowSystem
    ${STATIC_PLUGINS}
)

install(TARGETS ${PROJECT} RUNTIME DESTINATION bin)
install(FILES ${CONFIG_FILES} DESTINATION ${LXQT_ETC_XDG_DIR}/lxqt)
install(FILES ${PUB_HEADERS} DESTINATION include/lxqt)
