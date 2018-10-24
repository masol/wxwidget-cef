###########################################################################
#   Copyright (C) masol.li@gmail.com                                      #
#                                                                         #
###########################################################################

#Message("ASSIMP_LIBRARY_DBG============================${ASSIMP_LIBRARY_DBG}")
MESSAGE(STATUS "CMAKE VERSION DETECTED " ${CMAKE_VERSION})

###########################################################################
#
# Check and configure cmake
#
###########################################################################
# Fresh start
# we use copy internal macro -- this is supported by cmake 2.8.
# we use qt5 internal macro -- this is supported by cmake 3.1.
cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)
#Remove the following when the version check is at least 2.8.4
#SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(wide)

# Dont overconfigure
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limit configs" FORCE)

###########################################################################
#
# Include necessary submodules
#
###########################################################################

set(CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/cmake"
	"${PROJECT_SOURCE_DIR}/cmake/Utils"
	"${PROJECT_SOURCE_DIR}/cmake/Packages"
	"${PROJECT_SOURCE_DIR}/cmake/SpecializedConfig"
)

INCLUDE(Configuration)
INCLUDE(PlatformSpecific)

# Install CMake modules
#add_subdirectory(CMake)


SET(WIDE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
SET(WIDE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

SET(WIDE_INCLUDE_DIR "${WIDE_SOURCE_DIR}/include")
include_directories("${WIDE_INCLUDE_DIR}")
include_directories("${WIDE_BUILD_DIR}/include")

# When using single configuration generators like make
# cmake does need to know which of the possible configurations
# to generate
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "assure config" FORCE) # makes sure type is shown in cmake gui

message(STATUS "Building mode: " ${CMAKE_BUILD_TYPE})

# Find dependencies
include(Dependencies)

if (NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required Boost files - Please check ${BOOST_SEARCH_PATH}")
endif()

if (NOT wxWidgets_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required wxWidgets files - Please check ${wxWidgets_ROOT_DIR}")
endif()

if (NOT CEF_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required libcef files - Please check ${CEF_ROOT}")
endif()

#if (NOT FreeImage_FOUND)
#	MESSAGE(FATAL_ERROR "--> Could not locate required FreeImage files - Please check ${FREEIMAGE_SEARCH_PATH}")
#endif()

#if (NOT GETTEXT_FOUND)
#	MESSAGE(FATAL_ERROR "--> Could not locate required gettext files, Please check path")
#endif()

#TODO disable i18n support when icu not found.
if(Boost_USE_STATIC_LIBS)
	if (NOT ICU_FOUND)
		MESSAGE(FATAL_ERROR "--> Could not locate required ICU files, Please check ${ICU_ROOT}")
	endif()
endif()

set(WIDE_VERSION_MAJOR 2)
set(WIDE_VERSION_MINOR 0)

if(UNIX AND NOT APPLE)
	set(OS_LINUX TRUE)
endif()
if(APPLE)
	set(OS_APPLE TRUE)
endif()
if(WIN32)
	set(OS_WIN TRUE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(DEBUG FALSE)
	set(NDEBUG TRUE)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(DEBUG TRUE)
	set(NDEBUG FALSE)
endif()


# Configure a header file to pass some of the CMake settings
# to the source code
configure_file(
  "${WIDE_SOURCE_DIR}/include/config.h.in"
  "${WIDE_BUILD_DIR}/include/config.h"
  )

#Message(STATUS "wxWidgets_LIBRARY : ${wxWidgets_LIBRARIES}")

IF (NOT WIN32)
    STRING(REGEX MATCH "Release" _build_release ${CMAKE_BUILD_TYPE})
    if(_build_release)
        add_definitions( -DNDEBUG )
    else()
        add_definitions( -D_DEBUG )
    endif()
endif(NOT WIN32)


add_subdirectory(src)
add_subdirectory(src/ui)
add_subdirectory(src/utils)
add_subdirectory(src/main)



###########################################################################
#
# For non win32 we'll have to copy everything to a single dir
#
###########################################################################

INCLUDE(AssembleBinDirs)
