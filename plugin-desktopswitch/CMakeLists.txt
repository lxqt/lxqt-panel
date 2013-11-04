cmake_minimum_required(VERSION 2.6)

set(PLUGIN "desktopswitch")


set(HEADERS
  desktopswitch.h
  desktopswitchbutton.h
)

set(SOURCES
  desktopswitch.cpp
  desktopswitchbutton.cpp
)

set(MOCS
  desktopswitch.h
  desktopswitchbutton.h
)

set(LIBRARIES ${LIBRARIES} ${LXQT_LIBRARIES} ${LXQT_GLOBALKEYS_LIBRARIES} )

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
