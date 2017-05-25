set(PLUGIN "clock")

set(HEADERS
    lxqtclock.h
    lxqtclockconfiguration.h
    lxqtclockconfigurationtimezones.h
    lxqtclockconfigurationmanualformat.h
)

set(SOURCES
    lxqtclock.cpp
    lxqtclockconfiguration.cpp
    lxqtclockconfigurationtimezones.cpp
    lxqtclockconfigurationmanualformat.cpp
)

set(UIS
    lxqtclockconfiguration.ui
    lxqtclockconfigurationtimezones.ui
    lxqtclockconfigurationmanualformat.ui
)

BUILD_LXQT_PLUGIN(${PLUGIN})
