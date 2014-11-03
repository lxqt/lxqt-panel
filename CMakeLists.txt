cmake_minimum_required(VERSION 2.8.11)
project(lxqt-panel)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
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


# *******************************************************************


include(LxQtLibSuffix)

add_definitions ( -Wall )

# Support Qt5
option(USE_QT5 "Build with Qt5." $ENV{USE_QT5})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5LinguistTools REQUIRED QUIET)
find_package(Qt5Xml REQUIRED QUIET)
find_package(Qt5X11Extras REQUIRED QUIET)

find_package(KF5WindowSystem)

find_package(lxqt-qt5 REQUIRED)
find_package(Qt5Xdg REQUIRED QUIET)
find_package(lxqt-globalkeys-qt5 REQUIRED)
find_package(lxqt-globalkeys-ui-qt5 REQUIRED)

include(${LXQT_USE_FILE})
include(${QTXDG_USE_FILE})
include(${LXQT_GLOBALKEYS_USE_FILE})
include(${LXQT_GLOBALKEYS_UI_USE_FILE})

include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${LXQT_INCLUDE_DIRS}
    ${QTXDG_INCLUDE_DIRS}
    src
)

add_subdirectory(panel)

# Plugin system
# You can enable/disable building of the plugin using cmake options.
#    cmake -DCLOCK_PLUGIN=Yes .. # Enable clock plugin
#    cmake -DCLOCK_PLUGIN=No ..  # Disable clock plugin

set(ENABLED_PLUGINS)

set(PLUGIN_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/lxqt-panel/")
add_definitions(-DPLUGIN_DIR=\"${PLUGIN_DIR}\")
message(STATUS "Panel plugins location: ${PLUGIN_DIR}")

setByDefault(CLOCK_PLUGIN Yes)
if(CLOCK_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Clock/Calendar")
    add_subdirectory( plugin-clock )
endif()

setByDefault(DESKTOPSWITCH_PLUGIN Yes)
if(DESKTOPSWITCH_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Desktop switcher")
    add_subdirectory( plugin-desktopswitch )
endif()

setByDefault(QUICKLAUNCH_PLUGIN Yes)
if(QUICKLAUNCH_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Quicklaunch")
    add_subdirectory(plugin-quicklaunch)
endif()

setByDefault(TRAY_PLUGIN Yes)
if(TRAY_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "System Tray")
    add_subdirectory(plugin-tray)
endif()

setByDefault(MAINMENU_PLUGIN Yes)
if(MAINMENU_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Application menu")
    add_subdirectory(plugin-mainmenu)
endif()

setByDefault(MOUNT_PLUGIN Yes)
if(MOUNT_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Mount")

    find_package(lxqtmount-qt5 REQUIRED)
    include(${LXQTMOUNT_USE_FILE})

    add_subdirectory(plugin-mount)
endif(MOUNT_PLUGIN)

setByDefault(TASKBAR_PLUGIN Yes)
if(TASKBAR_PLUGIN)
  set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Taskbar")
  add_subdirectory(plugin-taskbar)
endif()

setByDefault(SCREENSAVER_PLUGIN Yes)
if(SCREENSAVER_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Screensaver")
    add_subdirectory( plugin-screensaver )
endif()

setByDefault(SHOWDESKTOP_PLUGIN Yes)
if(SHOWDESKTOP_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Show Desktop")
    add_subdirectory(plugin-showdesktop)
endif()

setByDefault(COLORPICKER_PLUGIN Yes)
if(COLORPICKER_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Color Picker")
    add_subdirectory(plugin-colorpicker)
endif()

setByDefault(HELLOWORLD_PLUGIN No)
if(HELLOWORLD_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Hello World")
    add_subdirectory(plugin-helloworld)
endif()

setByDefault(CPULOAD_PLUGIN Yes)
if(CPULOAD_PLUGIN)
    find_library(STATGRAB_LIB statgrab)

    if(NOT(${STATGRAB_LIB} MATCHES "NOTFOUND"))
        set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "Cpu Load")
        add_subdirectory(plugin-cpuload)
  else()
        message(STATUS "")
        message(STATUS "panel's CPU load plugin requires libstatgrab")
        message(STATUS "")
    endif()
endif()

setByDefault(NETWORKMONITOR_PLUGIN Yes)
if(NETWORKMONITOR_PLUGIN)
    find_library(STATGRAB_LIB statgrab)

    if(NOT(${STATGRAB_LIB} MATCHES "NOTFOUND"))
        set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "NetworkMonitor")
        add_subdirectory(plugin-networkmonitor)
    else()
        message(STATUS "")
        message(STATUS "Network monitor plugin requires libstatgrab")
        message(STATUS "")
    endif()
endif()

setByDefault(SENSORS_PLUGIN Yes)
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

setByDefault(KBINDICATOR_PLUGIN Yes)
if(KBINDICATOR_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS}   "Keyboard indicator")
    add_subdirectory(plugin-kbindicator)
endif(KBINDICATOR_PLUGIN)


setByDefault(TEATIME_PLUGIN No)
if(TEATIME_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS}   "TeaTime")
    add_subdirectory(plugin-teatime)
endif(TEATIME_PLUGIN)

setByDefault(SYSSTAT_PLUGIN Yes)
if(SYSSTAT_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS}   "System Stats")
    add_subdirectory(plugin-sysstat)
endif(SYSSTAT_PLUGIN)

setByDefault(WORLDCLOCK_PLUGIN Yes)
if(WORLDCLOCK_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS}   "World Clock")
    add_subdirectory(plugin-worldclock)
endif(WORLDCLOCK_PLUGIN)

setByDefault(DOM_PLUGIN Yes)
if(DOM_PLUGIN)
    set(ENABLED_PLUGINS ${ENABLED_PLUGINS} "DOM")
    add_subdirectory(plugin-dom)
endif(DOM_PLUGIN)

message(STATUS  "**************** The following plugins will be built ****************")
foreach (PLUGIN_STR ${ENABLED_PLUGINS})
    message(STATUS "  ${PLUGIN_STR}")
endforeach()
message(STATUS "*********************************************************************")

# building tarball with CPack -------------------------------------------------
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set(CPACK_GENERATOR TBZ2)
set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include(CPack)
