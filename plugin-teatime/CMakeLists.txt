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

include("../cmake/BuildPlugin.cmake")
BUILD_LXQT_PLUGIN(${PLUGIN})
