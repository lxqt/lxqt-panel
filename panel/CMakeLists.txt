set(PROJECT lxqt-panel)

set(PRIV_HEADERS
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
set(PUB_HEADERS
    lxqtpanelglobals.h
    ilxqtpanelplugin.h
    ilxqtpanel.h
)

set(SOURCES
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

set(QRC
    ""
)
qt5_add_resources(RESOURCES ${QRC})

set(UI
    config/configpaneldialog.ui
)
qt5_wrap_ui(UI_HEADERS ${UI})

set(LIBRARIES
    ${LXQT_LIBRARIES}
    ${QTXDG_LIBRARIES}
)

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

set(QTX_LIBRARIES Qt5::Widgets Qt5::Xml Qt5::DBus)

# Translations
lxqt_translate_ts(lxqt-runner_QM_FILES SOURCES
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

lxqt_app_translation_loader(QM_LOADER ${PROJECT_NAME})

add_executable(${PROJECT}
    ${PUB_HEADERS}
    ${PRIV_HEADERS}
    ${SOURCES}
    ${RESOURCES}
    ${UI_HEADERS}
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
