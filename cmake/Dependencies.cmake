###########################################################################
#   Copyright (C) 2013 by masol.li@gmail.com                              #
#                                                                         #
###########################################################################
#
# Binary samples directory
#
###########################################################################

include(FindPkgMacros)
getenv_path(WIDE_DEP_DIRS)

#######################################################################
# Core dependencies
#######################################################################

# Find threading library
FIND_PACKAGE(Threads REQUIRED)

# Find Boost
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT                  "${BOOST_SEARCH_PATH}")
#set(Boost_DEBUG                 ON)
set(Boost_MINIMUM_VERSION       "1.50.0")

set(Boost_ADDITIONAL_VERSIONS "1.53.0" "1.52.0" "1.52.1" "1.51" "1.51.0" "1.50" "1.50.0")

set(BOOST_BOOST_COMPONENTS serialization locale thread filesystem system program_options regex)
find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${BOOST_BOOST_COMPONENTS})
if (NOT Boost_FOUND)
        # Try again with the other type of libs
        if(Boost_USE_STATIC_LIBS)
                set(Boost_USE_STATIC_LIBS)
        else()
                set(Boost_USE_STATIC_LIBS OFF)
        endif()
        find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${BOOST_BOOST_COMPONENTS})
endif()

if (Boost_FOUND)
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	# Don't use old boost versions interfaces
	ADD_DEFINITIONS(-DBOOST_FILESYSTEM_NO_DEPRECATED)
endif ()


#[[
##---------------------------------------------------
## set your Qt configuration here
##---------------------------------------------------
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
]]

# Use the package PkgConfig to detect GTK+ headers/library files
##---------------------------------------------------
## Found GTK2 here.
##---------------------------------------------------
if(UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
  #Message(STATUS "GTK2_FOUND : ${GTK2_FOUND}")
  if (GTK2_FOUND)
    INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS})
    LINK_DIRECTORIES(${GTK2_LIBRARY_DIRS})
    ADD_DEFINITIONS(${GTK2_CFLAGS_OTHER})
    set(USE_GTK     TRUE)
  endif(GTK2_FOUND)
endif(UNIX AND NOT APPLE)


##---------------------------------------------------
## Found X11 here.
##---------------------------------------------------
if(NOT WIN32)
  find_package(X11)
  if(X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    link_directories(${X11_LIBRARIES})
    set(USE_X11     TRUE)
  endif(X11_FOUND)
endif(NOT WIN32)


##---------------------------------------------------
## set your wxWidgets configuration here
##---------------------------------------------------
set(wxWidgets_USE_STATIC 1)

# Here you can define what libraries of wxWidgets you need for your
# application. You can figure out what libraries you need here;
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
# We need the Find package for wxWidgets to work
# NOTE: if you're using aui, include aui in this required components list.

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS propgrid gl aui stc html adv core base REQUIRED)

if (wxWidgets_FOUND)
    set(wxUSE_UNICODE_DEFINE "wxUSE_UNICODE=1")
#    set(wxUSE_STATIC_DEFINE "wxUSE_STATIC=1")
 	IF(MSVC)
 		include_directories( ${wxWidgets_ROOT_DIR}/include/msvc )
 	ENDIF(MSVC)
   	include_directories(SYSTEM ${wxWidgets_INCLUDE_DIRS})

	if(WIN32)
		set(wxWidgets_LIBRARY_DIRS "${wxWidgets_ROOT_DIR}/lib/vc_lib")
	endif()
	link_directories(${wxWidgets_LIBRARY_DIRS})

	STRING(REGEX REPLACE ";" ";-D" wxWidgets_DEFINITIONS "${wxWidgets_DEFINITIONS}")
	SET(wxWidgets_DEFINITIONS "-D${wxWidgets_DEFINITIONS}")
#    Message(STATUS "wxWidgets_DEFINITIONS : ${wxWidgets_DEFINITIONS}")
#    Message(STATUS "wxWidgets_LIBRARY : ${wxWidgets_LIBRARIES}")
#    mark_as_advanced(wxWidgets_LIBRARIES)
	# Don't use old boost versions interfaces
	ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})
	ADD_DEFINITIONS(-D${wxUSE_UNICODE_DEFINE})
#	ADD_DEFINITIONS(-D${wxUSE_STATIC_DEFINE})
endif ()


# Find FreeImage
# find_package(FreeImage)

#if (FreeImage_FOUND)
#	include_directories(SYSTEM ${FreeImage_INCLUDE_DIRS})
#endif ()

# Find CEF
find_package(CEF REQUIRED)
if (CEF_FOUND)
  # Include the libcef_dll_wrapper target (executes libcef_dll/CMakeLists.txt).
  #add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
  # Allow includes relative to the current source directory.
  include_directories(${CEF_ROOT})
  #Message("CEF_DBG============================${CEF_LIBRARY}-----")
  #Message("CEF_DBG============================${CEFWRAPPER_LIBRARY}-----")
  #PRINT_CEF_CONFIG()
endif()

##---------------------------------------------------
## set your icu configuration here, if we dynamic link to boost, do not need icu any more.
##---------------------------------------------------
if(Boost_USE_STATIC_LIBS)
 find_package(ICU 4.2 REQUIRED)
 if (ICU_FOUND)
    include_directories(${ICU_INCLUDE_DIRS})
    link_directories(${ICU_LIBRARY_DIRS})
 endif()
endif()
