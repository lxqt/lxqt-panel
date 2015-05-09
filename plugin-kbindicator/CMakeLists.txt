set(PLUGIN "kbindicator")

find_package(KF5GuiAddons REQUIRED)

set(HEADERS
    lxqtkbindicator.h
    lxqtkbindicatorconfiguration.h
)

set(SOURCES
    lxqtkbindicator.cpp
    lxqtkbindicatorconfiguration.cpp
)

set(UIS
    lxqtkbindicatorconfiguration.ui
)

set(LIBRARIES
    ${LIBRARIES}
    KF5::GuiAddons
)

BUILD_LXQT_PLUGIN(${PLUGIN})
