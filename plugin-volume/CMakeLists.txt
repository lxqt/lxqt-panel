cmake_minimum_required(VERSION 2.6)

set(PLUGIN "panelvolume")


set(HEADERS
    lxqtvolume.h
    volumebutton.h
    volumepopup.h
    audiodevice.h
    lxqtvolumeconfiguration.h
    audioengine.h
)

set(SOURCES
    lxqtvolume.cpp
    volumebutton.cpp
    volumepopup.cpp
    audiodevice.cpp
    lxqtvolumeconfiguration.cpp
    audioengine.cpp
)

set(MOCS
    lxqtvolume.h
    volumebutton.h
    volumepopup.h
    audiodevice.h
    lxqtvolumeconfiguration.h
    audioengine.h
)

set(UIS
    lxqtvolumeconfiguration.ui
)

if (PULSEAUDIO_FOUND)
    add_definitions(-DUSE_PULSEAUDIO)
    include_directories(${PULSEAUDIO_INCLUDE_DIR})
    set(HEADERS ${HEADERS} pulseaudioengine.h)
    set(SOURCES ${SOURCES} pulseaudioengine.cpp)
    set(MOCS ${MOCS} pulseaudioengine.h)
    LINK_LIBRARIES(${PLUGIN} ${PULSEAUDIO_LIBRARY})
endif (PULSEAUDIO_FOUND)

if (ALSA_FOUND)
    add_definitions(-DUSE_ALSA)
    set(HEADERS ${HEADERS} alsaengine.h alsadevice.h)
    set(SOURCES ${SOURCES} alsaengine.cpp alsadevice.cpp)
    set(MOCS ${MOCS} alsaengine.h alsadevice.h)
    LINK_LIBRARIES(${PLUGIN} ${ALSA_LIBRARIES})
endif (ALSA_FOUND)

set(LIBRARIES ${LIBRARIES} ${LXQT_GLOBALKEYS_LIBRARIES} ${QTXDG_LIBRARIES})

#*******************************************
include ("../BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
