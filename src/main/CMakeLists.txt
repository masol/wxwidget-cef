###########################################################################
#   Copyright (C) masol.li@gmail.com                                      #
#                                                                         #
#  this file is part of WIDE(wware ide)                                   #
###########################################################################

#############################################################################
#
# app executable
#
#############################################################################

#############################################################################
#
# ICU Resource Bundle used by wide.
#
#############################################################################
set(WIDE_RESBUNDLE_INPUT_DIR ${WIDE_SOURCE_DIR}/i18n)

IF (WIN32)
        SET(WIDE_RESBUNDLE_OUTPUT_BASEDIR ${WIDE_BUILD_DIR}/bin/$(ConfigurationName)/i18n)
        SET(WIDE_RESBUNDLE_OUTPUT_DIR ${WIDE_RESBUNDLE_OUTPUT_BASEDIR}/zh_CN/LC_MESSAGES)
		STRING(REGEX REPLACE "/" "\\\\" WIDE_RESBUNDLE_OUTPUT_DIR ${WIDE_RESBUNDLE_OUTPUT_DIR})
        #STRING(REGEX REPLACE "/" "\\\\" WIDE_RESBUNDLE_OUTPUT ${WIDE_RESBUNDLE_OUTPUT})
		STRING(REGEX REPLACE "/" "\\\\" WIDE_RESBUNDLE_INPUT_DIR ${WIDE_RESBUNDLE_INPUT_DIR})
		#SET(RESBUNDLE_CREATE_OUTPUT_DIR if exist "${WIDE_RESBUNDLE_OUTPUT_DIR}" echo "dir exist" else mkdir -p "${WIDE_RESBUNDLE_OUTPUT_DIR}")
ELSE()
        SET(WIDE_RESBUNDLE_OUTPUT_BASEDIR ${WIDE_BUILD_DIR}/bin/i18n)
        SET(WIDE_RESBUNDLE_OUTPUT_DIR ${WIDE_RESBUNDLE_OUTPUT_BASEDIR}/zh_CN/LC_MESSAGES)
ENDIF(WIN32)

        SET(WIDE_RESBUNDLE_OUTPUT ${WIDE_RESBUNDLE_OUTPUT_DIR}/wide.mo)

add_custom_command(
		OUTPUT ${WIDE_RESBUNDLE_OUTPUT}
		# Creating target directory
		COMMAND ${CMAKE_COMMAND} -E make_directory ${WIDE_RESBUNDLE_OUTPUT_DIR}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${WIDE_RESBUNDLE_OUTPUT_BASEDIR}/en_US/LC_MESSAGES
		#COMMAND "${RESBUNDLE_CREATE_OUTPUT_DIR}"
		COMMAND msgfmt -c -o ${WIDE_RESBUNDLE_OUTPUT_BASEDIR}/zh_CN/LC_MESSAGES/wide.mo ${WIDE_RESBUNDLE_INPUT_DIR}/zh_CN.po
		COMMAND msgfmt -c -o ${WIDE_RESBUNDLE_OUTPUT_BASEDIR}/en_US/LC_MESSAGES/wide.mo ${WIDE_RESBUNDLE_INPUT_DIR}/en_US.po
		WORKING_DIRECTORY ${WIDE_BUILD_DIR}
	)

add_custom_target(wide_res DEPENDS ${WIDE_RESBUNDLE_OUTPUT})

#############################################################################
#
# WIDE binary
#
#############################################################################
set(wxWidgets_LIBRARIES_Cache ${wxWidgets_LIBRARIES})
set(WIDE_SRCS
	main.cpp
 )

set(SUB_SRCS
 	submain.cpp
 )

if(wxWidgets_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
    Message(STATUS "wxWidgets_CXX_FLAGS=${wxWidgets_CXX_FLAGS}")
endif(wxWidgets_CXX_FLAGS)

add_executable(wide ${WIDE_SRCS})
add_executable(subproc ${SUB_SRCS})

IF (WIN32)
TARGET_LINK_LIBRARIES(wide
 wideui
 utils
 precomp
 #${ICU_LIBRARIES}
 ${wxWidgets_LIBRARIES_Cache}
 ${Boost_LIBRARIES}
 ${CEF_LIBRARIES}
 #${ASSIMP_LIBRARIES}
 #${FreeImage_LIBRARIES}
)

set_target_properties(wide PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

TARGET_LINK_LIBRARIES(subproc
 wideui
 utils
 #${ICU_LIBRARIES}
 ${wxWidgets_LIBRARIES_Cache}
 ${Boost_LIBRARIES}
 ${CEF_LIBRARIES}
 #${ASSIMP_LIBRARIES}
 #${FreeImage_LIBRARIES}
)

set_target_properties(subproc PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

ELSE(WIN32)
#Message(STATUS "OPENCL_LIBRARIES : ${OPENCL_INCLUDE_DIR}")

set_target_properties(wide PROPERTIES LINK_FLAGS "-Wl,-rpath,.")
TARGET_LINK_LIBRARIES(wide
  wideui
  utils
  precomp
  #${ICU_LIBRARIES}
  #${ASSIMP_LIBRARIES}
  ${wxWidgets_LIBRARIES_Cache}
  #${FreeImage_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CEF_LIBRARIES}
  ${GTK2_LIBRARIES}
  ${X11_LIBRARIES}
)

set_target_properties(subproc PROPERTIES LINK_FLAGS "-Wl,-rpath,.")
TARGET_LINK_LIBRARIES(subproc
  #wideui
  #utils
  #${ICU_LIBRARIES}
  #${ASSIMP_LIBRARIES}
  #${wxWidgets_LIBRARIES_Cache}
  #${FreeImage_LIBRARIES}
  #${Boost_LIBRARIES}
  ${CEF_LIBRARIES}
  ${CEF_LIBRARY}
)

ENDIF(WIN32)
