# Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# This file is the CEF CMake configuration entry point and should be loaded
# using `find_package(CEF REQUIRED)`. See the top-level CMakeLists.txt file
# included with the CEF binary distribution for usage information.
#


include(FindPackageHandleStandardArgs)

SET(CEF_ROOT_DIR "" CACHE PATH "Path to a CEF distributed build")

if(CEF_ROOT AND IS_DIRECTORY "${CEF_ROOT}")
  set(CEF_ROOT_DIR "${CEF_ROOT}")
else()
  set(_ENV_CEF_ROOT "")
  if(DEFINED ENV{CEF_ROOT})
    file(TO_CMAKE_PATH "$ENV{CEF_ROOT}" _ENV_CEF_ROOT)
  endif()
  if(_ENV_CEF_ROOT AND IS_DIRECTORY "${_ENV_CEF_ROOT}")
    set(CEF_ROOT_DIR "${_ENV_CEF_ROOT}")
  endif()
  set(CEF_ROOT ${CEF_ROOT_DIR})
  unset(_ENV_CEF_ROOT)
endif()

message(STATUS "Looking for Chromium Embedded Framework in ${CEF_ROOT_DIR}")

# Execute additional cmake files from the CEF binary distribution.
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT_DIR}/cmake")
#include("cef_variables")
#include("cef_macros")

find_path(CEF_INCLUDE_DIR "include/cef_version.h"
	HINTS ${CEF_ROOT_DIR})

if(APPLE)
	find_library(CEF_LIBRARY
		NAMES cef libcef cef.lib libcef.o "Chromium Embedded Framework"
		NO_DEFAULT_PATH
		PATHS ${CEF_ROOT_DIR} ${CEF_ROOT_DIR}/Release)
	find_library(CEFWRAPPER_LIBRARY
		NAMES cef_dll_wrapper libcef_dll_wrapper
		NO_DEFAULT_PATH
		PATHS ${CEF_ROOT_DIR}/build/libcef_dll/Release
			${CEF_ROOT_DIR}/build/libcef_dll_wrapper/Release
			${CEF_ROOT_DIR}/build/libcef_dll
			${CEF_ROOT_DIR}/build/libcef_dll_wrapper)
else()
	find_library(CEF_LIBRARY
		NAMES cef libcef cef.lib libcef.o "Chromium Embedded Framework"
		PATHS ${CEF_ROOT_DIR} ${CEF_ROOT_DIR}/Release)
	find_library(CEFWRAPPER_LIBRARY
		NAMES cef_dll_wrapper libcef_dll_wrapper
		PATHS ${CEF_ROOT_DIR}/build/libcef_dll/Release
			${CEF_ROOT_DIR}/build/libcef_dll_wrapper/Release
			${CEF_ROOT_DIR}/build/libcef_dll
			${CEF_ROOT_DIR}/build/libcef_dll_wrapper)
	if(WIN32)
		find_library(CEFWRAPPER_LIBRARY_DEBUG
			NAMES cef_dll_wrapper libcef_dll_wrapper
			PATHS ${CEF_ROOT_DIR}/build/libcef_dll/Debug ${CEF_ROOT_DIR}/build/libcef_dll_wrapper/Debug)
	endif()
endif()

if(NOT CEF_LIBRARY)
	message(WARNING "Could not find the CEF shared library" )
	set(CEF_FOUND FALSE)
	return()
endif()

if(NOT CEFWRAPPER_LIBRARY)
	message(WARNING "Could not find the CEF wrapper library" )
	set(CEF_FOUND FALSE)
	return()
endif()

if(WIN32)
	set(CEF_LIBRARIES
			${CEF_LIBRARY}
			optimized ${CEFWRAPPER_LIBRARY})
	if (CEFWRAPPER_LIBRARY_DEBUG)
		list(APPEND CEF_LIBRARIES
				debug ${CEFWRAPPER_LIBRARY_DEBUG})
	endif()
else()
	set(CEF_LIBRARIES
			${CEF_LIBRARY}
			${CEFWRAPPER_LIBRARY})
endif()

find_package_handle_standard_args(CEF DEFAULT_MSG CEF_LIBRARY
	CEFWRAPPER_LIBRARY CEF_INCLUDE_DIR)
mark_as_advanced(CEF_LIBRARY CEF_WRAPPER_LIBRARY CEF_LIBRARIES
CEF_INCLUDE_DIR)

if(NOT CEF_FOUND)
  message(FATAL_ERROR "Must specify a CEF_ROOT value via CMake or environment variable.")
endif()
