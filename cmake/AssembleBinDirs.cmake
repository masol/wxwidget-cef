###########################################################################
#   Copyright (C) 2013 by masol.li@gmail.com                              #
#                                                                         #
###########################################################################
#
# Binary samples directory
#
###########################################################################

IF (WIN32)

  # For MSVC moving exe files gets done automatically
  # If there is someone compiling on windows and
  # not using msvc (express is free) - feel free to implement

ELSE (WIN32)
	set(CMAKE_INSTALL_PREFIX .)

ENDIF(WIN32)
