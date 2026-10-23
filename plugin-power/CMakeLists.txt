set(PLUGIN "power")

set(HEADERS
    lxqtpowerplugin.h
    lxqtpower.h
    lxqtpowerconfiguration.h
)

set(SOURCES
    lxqtpowerplugin.cpp
    lxqtpower.cpp
    lxqtpowerconfiguration.cpp
)

set(UIS
    lxqtpowerconfiguration.ui
)

BUILD_LXQT_PLUGIN(${PLUGIN})
