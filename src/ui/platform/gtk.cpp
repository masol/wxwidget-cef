//////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2013 by sanpolo CO.LTD                                    //
//                                                                          //
//  This file is part of WIDE                                               //
//                                                                          //
//  You should have received a copy of the MIT LIcense along with this     //
//  program.  If not, see <http://www.wware.org/wide/license.html>.         //
//                                                                          //
//  WIDE website: http://www.wware.org/                                     //
//////////////////////////////////////////////////////////////////////////////
#include "config.h"

#if defined(USE_GTK)

#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <iostream>

unsigned int GetXWinID(void *handle) {
  GtkWidget *widget = reinterpret_cast<GtkWidget *>(handle);
  gtk_widget_realize(widget);
  int ret = GDK_WINDOW_XWINDOW(widget->window);
  //std::cout << "widget=" << widget << " ret=" << ret << std::endl;
  return ret;
  // return
  // GDK_WINDOW_XID(gtk_widget_get_window(reinterpret_cast<GtkWidget*>(handle)));
}

#endif
