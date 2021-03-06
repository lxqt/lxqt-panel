set(PLUGIN "custom")

set(HEADERS
    lxqtcustom.h
    custombutton.h
    lxqtcustomconfiguration.h
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.cpp
    lxqtcustom.cpp
    custombutton.cpp
    lxqtcustomconfiguration.cpp
)

set(UIS
    lxqtcustomconfiguration.ui
)

set(LIBRARIES
    Qt5Xdg
)

BUILD_LXQT_PLUGIN(${PLUGIN})
