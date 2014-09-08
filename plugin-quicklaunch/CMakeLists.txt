set(PLUGIN "quicklaunch")

# if (QUICKLAUNCH_PLUGIN)
#   set(ENABLED_PLUGINS ${ENABLED_PLUGINS}  "Quick launch")
#   QT4_WRAP_CPP(quicklaunch_moc plugin-quicklaunch/lxqtquicklaunch.h)
#   add_library(lxqtpanel_quicklaunch SHARED ${quicklaunch_moc} plugin-quicklaunch/lxqtquicklaunch.cpp)
#   target_link_libraries(lxqtpanel_quicklaunch ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#   install(TARGETS lxqtpanel_quicklaunch DESTINATION ${PLUGIN_DIR})
# endif (QUICKLAUNCH_PLUGIN)
set(HEADERS
  lxqtquicklaunchplugin.h
  lxqtquicklaunch.h
  quicklaunchbutton.h
  quicklaunchaction.h
  desktopfile.h
)

set(SOURCES
  lxqtquicklaunchplugin.cpp
  lxqtquicklaunch.cpp
  quicklaunchbutton.cpp
  quicklaunchaction.cpp
  desktopfile.cpp
)

set(MOCS
  lxqtquicklaunchplugin.h
  lxqtquicklaunch.h
  quicklaunchbutton.h
  quicklaunchaction.h
)

include_directories (
    ${LXQT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../panel
)

set(LIBRARIES ${QTXDG_LIBRARIES})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
