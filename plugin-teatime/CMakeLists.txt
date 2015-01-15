set(PLUGIN "teatime")

set(HEADERS
    teatimeplugin.h
)

set(SOURCES
    teatimeplugin.cpp
)

set(MOCS
    teatimeplugin.h
)

BUILD_LXQT_PLUGIN(${PLUGIN})
