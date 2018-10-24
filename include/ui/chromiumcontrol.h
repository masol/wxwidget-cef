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

#ifndef WIDE_UI_WEBVIEWBASE_H
#define WIDE_UI_WEBVIEWBASE_H

#include "ui/cefbase.h"

namespace wide {

extern const wxChar *wxChromiumControlNameStr;
extern const wxChar *wxChromiumControlDefURL;

class wxChromiumControl : public wxWindow {
  typedef wxWindow inherit;
public:
  wxChromiumControl() {}

  wxChromiumControl(wxWindow *parent, wxWindowID id,
                    const wxString &url = wxChromiumControlDefURL,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize, long style = 0,
                    const wxString &name = wxChromiumControlNameStr,
                    const int handlerType = 0) {
    Create(parent, id, url, pos, size, style, name, handlerType);
  }

  ~wxChromiumControl();

  /* @brief : create a control, and initionlize a client handler to it.
  //*/
  bool Create(wxWindow *parent, wxWindowID id,
              const wxString &url = wxChromiumControlDefURL,
              const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxString &name = wxChromiumControlNameStr,
              const int handlerType = 0);

protected:
  CefRefPtr<CefHandlerBase> m_browserHandler;
  void OnSize(wxSizeEvent &event);
  void OnFocus(wxFocusEvent &event);
  void OnKillFocus(wxFocusEvent &event);
private:
  DECLARE_EVENT_TABLE()
};

} // namespace wide

#endif
