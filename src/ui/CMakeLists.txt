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


set(UI_SRCS
    frame.cpp
    chromiumcontrol.cpp
    cefbase.cpp
    ceffactory.cpp
    platform/gtk.cpp
   )


add_library(wideui STATIC ${UI_SRCS})
