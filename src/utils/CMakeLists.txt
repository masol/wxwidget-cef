###########################################################################
#   Copyright (C) masol.li@gmail.com                                      #
#                                                                         #
#  this file is part of WIDE(wware ide)                                   #
###########################################################################

#############################################################################
#
# ui library
#
#############################################################################


set(UTILS_SRCS
    widexception.cpp
    i18n.cpp
    option.cpp
    sysinfo.cpp
    pathext.cpp
   )


add_library(utils STATIC ${UTILS_SRCS})
