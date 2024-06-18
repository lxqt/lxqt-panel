set(AUTOSTART_DESKTOP_FILES_IN lxqt-panel.desktop.in)

# Translations **********************************
lxqt_translate_desktop(DESKTOP_FILES
    SOURCES
        ${AUTOSTART_DESKTOP_FILES_IN}
    USE_YAML
)
add_custom_target(lxqt_panel_autostart_desktop_files ALL DEPENDS ${DESKTOP_FILES})
#************************************************

install(FILES
    ${DESKTOP_FILES}
    DESTINATION "${LXQT_ETC_XDG_DIR}/autostart"
    COMPONENT Runtime
)

configure_file(lxqt-panel_wayland.desktop.in lxqt-panel_wayland.desktop @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/lxqt-panel_wayland.desktop"
    DESTINATION "/usr/share/applications"
    RENAME "lxqt-panel.desktop"
    COMPONENT Runtime
)
