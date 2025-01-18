set(PLUGIN "customcommand")

set(HEADERS
    lxqtcustomcommand.h
    custombutton.h
    lxqtcustomcommandconfiguration.h
    colorLabel.h
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/panel/lxqtpanelpluginconfigdialog.cpp
    lxqtcustomcommand.cpp
    custombutton.cpp
    lxqtcustomcommandconfiguration.cpp
    colorLabel.cpp
)

set(UIS
    lxqtcustomcommandconfiguration.ui
)

set(LIBRARIES
    Qt6Xdg
)

BUILD_LXQT_PLUGIN(${PLUGIN})
