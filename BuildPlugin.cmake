MACRO (BUILD_LXQT_PLUGIN NAME)
    set(PROGRAM "lxqt-panel")
    project(${PROGRAM}_${NAME})

    set(PROG_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/lxqt/${PROGRAM})
    set(PLUGIN_SHARE_DIR ${PROG_SHARE_DIR}/${NAME})

    # Translations **********************************
    include(LxQtTranslate)
    lxqt_translate_ts(${PROJECT_NAME}_QM_FILES 
        SOURCES
            ${HEADERS} 
            ${SOURCES} 
            ${MOCS} 
            ${UIS}
        TS_SRC_FILE
            translations/${NAME}.ts.src
        INSTALLATION_DIR
            ${CMAKE_INSTALL_PREFIX}/share/lxqt/${PROGRAM}/${NAME}
    )
    

    #lxqt_translate_to(QM_FILES ${CMAKE_INSTALL_PREFIX}/share/lxqt/${PROGRAM}/${PROJECT_NAME})
    file (GLOB ${PROJECT_NAME}_DESKTOP_FILES_IN resources/*.desktop.in)
    lxqt_translate_desktop(DESKTOP_FILES 
        SOURCES
            ${${PROJECT_NAME}_DESKTOP_FILES_IN}
    )
    #************************************************

    file (GLOB CONFIG_FILES     resources/*.conf    )

    include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}/panel
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    if (NOT DEFINED PLUGIN_DIR)
        set (PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/${PROGRAM}/)
    endif (NOT DEFINED PLUGIN_DIR)

    find_package(Qt4 REQUIRED)
    include(${QT_USE_FILE})
    qt4_wrap_cpp(MOC_SOURCES ${MOCS})
    qt4_add_resources(QRC_SOURCES ${RESOURCES})
    qt4_wrap_ui(UI_SOURCES ${UIS})

    add_library(${NAME} MODULE ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${${PROJECT_NAME}_QM_FILES} ${QRC_SOURCES} ${UIS} ${DESKTOP_FILES})
    target_link_libraries(${NAME} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${LXQT_LIBRARIES} ${LIBRARIES})

    install(TARGETS ${NAME} DESTINATION ${PLUGIN_DIR})
    install(FILES ${CONFIG_FILES}  DESTINATION ${PLUGIN_SHARE_DIR})
    install(FILES ${DESKTOP_FILES} DESTINATION ${PROG_SHARE_DIR})

ENDMACRO(BUILD_LXQT_PLUGIN)



