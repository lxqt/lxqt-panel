set(PLUGIN "notes")

set(HEADERS
    notes.h
    stickynote.h
    notesconfiguration.h
)

set(SOURCES
    notes.cpp
    stickynote.cpp
    notesconfiguration.cpp
)

set(UIS 
	stickynote.ui
	notesconfiguration.ui
)

set(RESOURCES
    resources.qrc
)

set(LIBRARIES
    lxqt
)

BUILD_LXQT_PLUGIN(${PLUGIN})
