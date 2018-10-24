###########################################################################
#   Copyright (C) 2013 by masol.li@gmail.com                              #
#                                                                         #
###########################################################################

###########################################################################
#
# Configuration
#
# Use cmake "-DWIDE_CUSTOM_CONFIG=YouFileCName" To define your personal settings
# where YouFileCName.cname must exist in one of the cmake include directories
# best to use cmake/SpecializedConfig/
#
# To not load defaults before loading custom values define
# -DWIDE_NO_DEFAULT_CONFIG=true
#
# WARNING: These variables will be cached like any other
#
###########################################################################

IF (NOT WIDE_NO_DEFAULT_CONFIG)

  # Disable Boost automatic linking
  # ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

  IF (WIN32)

    MESSAGE(STATUS "Using default WIN32 Configuration settings")

    IF(MSVC)

      STRING(REGEX MATCH "(Win64)" _carch_x64 ${CMAKE_GENERATOR})
      IF(_carch_x64)
        SET(WINDOWS_ARCH "x64")
      ELSE(_carch_x64)
        SET(WINDOWS_ARCH "x86")
      ENDIF(_carch_x64)
      MESSAGE(STATUS "Building for target ${WINDOWS_ARCH}")

      SET(MSVC_RT_DEBUG_FIX 1)

    ELSE(MSVC)

    ENDIF(MSVC)

    IF(DEFINED ENV{WIDE_DEP_DIRS})
        SET(WIDE_DEP_DIRS $ENV{WIDE_DEP_DIRS})
        MESSAGE(STATUS "wide build environment variables found")
        SET(BOOST_SEARCH_PATH       "${WIDE_DEP_DIRS}/boost_1_65_0")
        SET(FreeImage_INC_SEARCH_PATH 		"${WIDE_DEP_DIRS}/FreeImage/Dist")
        SET(FreeImage_LIB_SEARCH_PATH 		"${WIDE_DEP_DIRS}/FreeImage/Dist")
    ENDIF(DEFINED ENV{WIDE_DEP_DIRS})

  ELSE(WIN32)
    IF(DEFINED ENV{WIDE_DEP_DIRS})
        SET(WIDE_DEP_DIRS $ENV{WIDE_DEP_DIRS})
        MESSAGE(STATUS "wide build environment variables found")
    ENDIF(DEFINED ENV{WIDE_DEP_DIRS})
  ENDIF(WIN32)

ELSE(NOT WIDE_NO_DEFAULT_CONFIG)

  MESSAGE(STATUS "WIDE_NO_DEFAULT_CONFIG defined - not using default configuration values.")

ENDIF(NOT WIDE_NO_DEFAULT_CONFIG)

# Setup libraries output directory
SET (LIBRARY_OUTPUT_PATH
   ${PROJECT_BINARY_DIR}/lib
   CACHE PATH
   "Single Directory for all Libraries"
   )

# Setup binaries output directory
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single Directory for all binaries"
	)

#
# Overwrite defaults with Custom Settings
#

IF (WIDE_CUSTOM_CONFIG)
	MESSAGE(STATUS "Using custom build config: ${WIDE_CUSTOM_CONFIG}")
	INCLUDE(${WIDE_CUSTOM_CONFIG})
ENDIF (WIDE_CUSTOM_CONFIG)
