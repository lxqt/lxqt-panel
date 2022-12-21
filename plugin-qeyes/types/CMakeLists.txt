install(DIRECTORY
    .
    DESTINATION "${LXQT_SHARE_DIR}/panel/qeyes-types"
    COMPONENT Runtime
    PATTERN "CMake*"
    	EXCLUDE
    PATTERN "cmake*"
    	EXCLUDE
    PATTERN "Makefile"
    	EXCLUDE
)
