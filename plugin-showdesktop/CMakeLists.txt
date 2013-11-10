set(PLUGIN "showdesktop")

set(HEADERS
  showdesktop.h
)

set(SOURCES
  showdesktop.cpp
)

set(MOCS
  showdesktop.h
)

set(LIBRARIES ${LIBRARIES} ${LXQT_GLOBALKEYS_LIBRARIES} ${QTXDG_LIBRARIES})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
