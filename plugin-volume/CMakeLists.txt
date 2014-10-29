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
    ${PROJECT_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.cpp
    lxqtvolume.cpp
    volumebutton.cpp
    volumepopup.cpp
    audiodevice.cpp
    lxqtvolumeconfiguration.cpp
    audioengine.cpp
    ossengine.cpp
)

set(MOCS
    ${PROJECT_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.h
    lxqtvolume.h
    volumebutton.h
    volumepopup.h
    audiodevice.h
    lxqtvolumeconfiguration.h
    audioengine.h
    ossengine.h
)

set(UIS
    lxqtvolumeconfiguration.ui
)

set(LIBRARIES
    ${LIBRARIES}
    ${LXQT_GLOBALKEYS_LIBRARIES}
    ${QTXDG_LIBRARIES}
)

if(PULSEAUDIO_FOUND)
    add_definitions(-DUSE_PULSEAUDIO)
    include_directories(${PULSEAUDIO_INCLUDE_DIR})
    set(HEADERS ${HEADERS} pulseaudioengine.h)
    set(SOURCES ${SOURCES} pulseaudioengine.cpp)
    set(MOCS ${MOCS} pulseaudioengine.h)
    link_libraries(${PLUGIN} ${PULSEAUDIO_LIBRARY})
endif()

if(ALSA_FOUND)
    add_definitions(-DUSE_ALSA)
    set(HEADERS ${HEADERS} alsaengine.h alsadevice.h)
    set(SOURCES ${SOURCES} alsaengine.cpp alsadevice.cpp)
    set(MOCS ${MOCS} alsaengine.h alsadevice.h)
    link_libraries(${PLUGIN} ${ALSA_LIBRARIES})
endif()

include ("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
