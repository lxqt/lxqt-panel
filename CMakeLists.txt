cmake_minimum_required(VERSION 2.8.11)
project(lxqt-panel)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. C++11 support is required")
endif()

macro(setByDefault VAR_NAME VAR_VALUE)
  if(NOT DEFINED ${VAR_NAME})
    set (${VAR_NAME} ${VAR_VALUE})
  endif(NOT DEFINED ${VAR_NAME})
endmacro()

# use gcc visibility feature to decrease unnecessary exported symbols
if (CMAKE_COMPILER_IS_GNUCXX)
  # set visibility to hidden to hide symbols, unlesss they're exporeted manually in the code
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -fno-exceptions")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-no-undefined")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#########################################################################

add_definitions (-Wall)
include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

# when cmake >= 3.0.2:
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(KF5WindowSystem REQUIRED)

find_package(lxqt REQUIRED)
find_package(Qt5Xdg REQUIRED QUIET)
find_package(lxqt-globalkeys REQUIRED)
find_package(lxqt-globalkeys-ui REQUIRED)

include(${LXQT_USE_FILE})
include(${QTXDG_USE_FILE})
include(${LXQT_GLOBALKEYS_USE_FILE})
include(${LXQT_GLOBALKEYS_UI_USE_FILE})

include(LXQtTranslate)

include_directories(
    ${LXQT_INCLUDE_DIRS}
    ${QTXDG_INCLUDE_DIRS}
    ${Qt5X11Extras_INCLUDE_DIRS}
)

# Warning: This must be before add_subdirectory(panel). Move with caution.
set(PLUGIN_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/lxqt-panel")
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

#########################################################################

# Plugin system
# You can enable/disable building of the plugin using cmake options.
#    cmake -DCLOCK_PLUGIN=Yes .. # Enable clock plugin
#    cmake -DCLOCK_PLUGIN=No ..  # Disable clock plugin

include("cmake/BuildPlugin.cmake")

set(ENABLED_PLUGINS) # list of enabled plugins
set(STATIC_PLUGINS) # list of statically linked plugins

setByDefault(CLOCK_PLUGIN Yes)
if(CLOCK_PLUGIN)
    list(APPEND STATIC_PLUGINS "clock")
    add_definitions(-DWITH_CLOCK_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Clock")
    add_subdirectory(plugin-clock)
endif()

setByDefault(COLORPICKER_PLUGIN No)
if(COLORPICKER_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Color Picker")
    add_subdirectory(plugin-colorpicker)
endif()

setByDefault(CPULOAD_PLUGIN No)
if(CPULOAD_PLUGIN)
    find_library(STATGRAB_LIB statgrab)

    if(NOT(${STATGRAB_LIB} MATCHES "NOTFOUND"))
        set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Cpu Load")
        add_subdirectory(plugin-cpuload)
  else()
        message(STATUS "")
        message(STATUS "CPU Load plugin requires libstatgrab")
        message(STATUS "")
    endif()
endif()

setByDefault(DIRECTORYMENU_PLUGIN No)
if(DIRECTORYMENU_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Directory menu")
    add_subdirectory(plugin-directorymenu)
endif()

setByDefault(DOM_PLUGIN No)
if(DOM_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "DOM")
    add_subdirectory(plugin-dom)
endif(DOM_PLUGIN)

setByDefault(DESKTOPSWITCH_PLUGIN Yes)
if(DESKTOPSWITCH_PLUGIN)
    list(APPEND STATIC_PLUGINS "desktopswitch")
    add_definitions(-DWITH_DESKTOPSWITCH_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Desktop Switcher")
    add_subdirectory(plugin-desktopswitch)
endif()

setByDefault(KBINDICATOR_PLUGIN No)
if(KBINDICATOR_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Keyboard Indicator")
    add_subdirectory(plugin-kbindicator)
endif(KBINDICATOR_PLUGIN)

setByDefault(MAINMENU_PLUGIN Yes)
if(MAINMENU_PLUGIN)
    list(APPEND STATIC_PLUGINS "mainmenu")
    add_definitions(-DWITH_MAINMENU_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Application menu")
    add_subdirectory(plugin-mainmenu)
endif()

setByDefault(MOUNT_PLUGIN No)
if(MOUNT_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Mount")
    add_subdirectory(plugin-mount)
endif(MOUNT_PLUGIN)

setByDefault(QUICKLAUNCH_PLUGIN Yes)
if(QUICKLAUNCH_PLUGIN)
    list(APPEND STATIC_PLUGINS "quicklaunch")
    add_definitions(-DWITH_QUICKLAUNCH_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Quicklaunch")
    add_subdirectory(plugin-quicklaunch)
endif()

setByDefault(SCREENSAVER_PLUGIN No)
if(SCREENSAVER_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Screensaver")
    add_subdirectory(plugin-screensaver)
endif()

setByDefault(SENSORS_PLUGIN No)
if(SENSORS_PLUGIN)
    find_library(SENSORS_LIB sensors)

    if(NOT(${SENSORS_LIB} MATCHES "NOTFOUND"))
        set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Sensors")
        add_subdirectory(plugin-sensors)
    else()
        message(STATUS "")
        message(STATUS "Sensors plugin requires lm_sensors")
        message(STATUS "")
    endif()
endif()

setByDefault(SHOWDESKTOP_PLUGIN Yes)
if(SHOWDESKTOP_PLUGIN)
    list(APPEND STATIC_PLUGINS "showdesktop")
    add_definitions(-DWITH_SHOWDESKTOP_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Show Desktop")
    add_subdirectory(plugin-showdesktop)
endif()

setByDefault(NETWORKMONITOR_PLUGIN No)
if(NETWORKMONITOR_PLUGIN)
    find_library(STATGRAB_LIB statgrab)

    if(NOT(${STATGRAB_LIB} MATCHES "NOTFOUND"))
        set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Network Monitor")
        add_subdirectory(plugin-networkmonitor)
    else()
        message(STATUS "")
        message(STATUS "Network Monitor plugin requires libstatgrab")
        message(STATUS "")
    endif()
endif()

setByDefault(SYSSTAT_PLUGIN No)
if(SYSSTAT_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "System Stats")
    add_subdirectory(plugin-sysstat)
endif(SYSSTAT_PLUGIN)

setByDefault(TASKBAR_PLUGIN Yes)
if(TASKBAR_PLUGIN)
  list(APPEND STATIC_PLUGINS "taskbar")
  add_definitions(-DWITH_TASKBAR_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Taskbar")
  add_subdirectory(plugin-taskbar)
endif()

setByDefault(STATUSNOTIFIER_PLUGIN Yes)
if(STATUSNOTIFIER_PLUGIN)
    list(APPEND STATIC_PLUGINS "statusnotifier")
    add_definitions(-DWITH_STATUSNOTIFIER_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Status Notifier")
    add_subdirectory(plugin-statusnotifier)
endif()

setByDefault(TRAY_PLUGIN Yes)
if(TRAY_PLUGIN)
    list(APPEND STATIC_PLUGINS "tray")
    add_definitions(-DWITH_TRAY_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "System Tray")
    add_subdirectory(plugin-tray)
endif()

setByDefault(VOLUME_PLUGIN Yes)
setByDefault(VOLUME_USE_PULSEAUDIO Yes)
setByDefault(VOLUME_USE_ALSA Yes)
if(VOLUME_PLUGIN)
    if (VOLUME_USE_PULSEAUDIO)
        find_package(PulseAudio)
    endif(VOLUME_USE_PULSEAUDIO)

    if(VOLUME_USE_ALSA)
        find_package(ALSA)
    endif()

    if(PULSEAUDIO_FOUND OR ALSA_FOUND)
        set(ENABLED_PLUGINS ${ENABLED_PLUGINS}   "Volume")
        message(STATUS "")
        message(STATUS "Volume plugin will be built")
        message(STATUS "    ALSA: ${ALSA_FOUND}")
        message(STATUS "    PulseAudio: ${PULSEAUDIO_FOUND}")
        message(STATUS "")
        add_subdirectory(plugin-volume)
    else()
        message(STATUS "")
        message(STATUS "Volume plugin requires pulseaudio or alsa")
        message(STATUS "    ALSA: ${ALSA_FOUND}")
        message(STATUS "    PulseAudio: ${PULSEAUDIO_FOUND}")
        message(STATUS "")
    endif()
endif()

setByDefault(WORLDCLOCK_PLUGIN No)
if(WORLDCLOCK_PLUGIN)
    list(APPEND STATIC_PLUGINS "worldclock")
    add_definitions(-DWITH_WORLDCLOCK_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "World Clock")
    add_subdirectory(plugin-worldclock)
endif(WORLDCLOCK_PLUGIN)

setByDefault(SPACER_PLUGIN No)
if(SPACER_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Spacer")
    add_subdirectory(plugin-spacer)
endif()

#########################################################################

message(STATUS  "**************** The following plugins will be built ****************")
foreach (PLUGIN_STR ${ENABLED_PLUGINS})
    message(STATUS "  ${PLUGIN_STR}")
endforeach()
message(STATUS "*********************************************************************")

add_subdirectory(panel)

# building tarball with CPack -------------------------------------------------
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set(CPACK_GENERATOR TBZ2)
set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include(CPack)
