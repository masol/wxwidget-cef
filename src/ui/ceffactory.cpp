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
#include "precomp.h"
#include "ui/cefbase.h"
#include "ui/ceffactory.h"
#include "ui/chromiumcontrol.h"
#include "utils/cef.h"

#if defined(USE_X11)
#include <X11/Xlib.h>
#endif

#if defined(USE_GTK)
extern unsigned int GetXWinID(void *handle);
#endif

namespace wide {

CefHandlerBase *Cefactory::createBrowser(const int type, wxWindow *win,
                                         const wxString &url) {
  CefBrowserSettings browsersettings;
  CefWindowInfo info;
  CefHandlerBase *retPtr = new CefHandlerBase();
  retPtr->SetControl(win);

#if defined(USE_GTK)
  int width, height;
  win->GetClientSize(&width, &height);
  cef_window_handle_t cefParent(GetXWinID(win->GetHandle()));
  std::cout << "cef_window_handle_t=" << cefParent
            << "win Handle = " << win->GetHandle() << std::endl;
  CefRect rect(0, 0, width, height);
  info.SetAsChild(cefParent, rect);

#elif defiend(OS_WIN)
  // Initialize window info to the defaults for a child window
  info.SetAsChild(win->GetHWND(), wxGetClientRect(win->GetHWND()));
#else
#error "NOT IMPLEMENT Window System..."
#endif
  // Creat the new child browser window, we do this async as we use a multi
  // threaded message loop
  retPtr->m_browser = CefBrowserHost::CreateBrowserSync(
      info, static_cast<CefRefPtr<CefClient>>(retPtr), url.ToStdString(),
      browsersettings, NULL);

  return retPtr;
}

} // namespace wide
