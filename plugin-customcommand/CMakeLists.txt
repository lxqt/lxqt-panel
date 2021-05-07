set(PLUGIN "customcommand")

set(HEADERS
    lxqtcustomcommand.h
    custombutton.h
    lxqtcustomcommandconfiguration.h
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.cpp
    lxqtcustomcommand.cpp
    custombutton.cpp
    lxqtcustomcommandconfiguration.cpp
)

set(UIS
    lxqtcustomcommandconfiguration.ui
)

set(LIBRARIES
    Qt5Xdg
)

BUILD_LXQT_PLUGIN(${PLUGIN})
