set(PLUGIN "spacer")

set(HEADERS
    spacer.h
    spacerconfiguration.h
)

set(SOURCES
    spacer.cpp
    spacerconfiguration.cpp
)

set(MOCS
    spacer.h
    spacerconfiguration.h
)

set(UIS
    spacerconfiguration.ui
)

BUILD_LXQT_PLUGIN(${PLUGIN})
