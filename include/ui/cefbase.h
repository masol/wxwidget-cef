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

#ifndef WIDE_UI_CEFBASE_H
#define WIDE_UI_CEFBASE_H

#include "utils/cef.h"

class wxWindow;
namespace wide {

class wxChromiumControl;

// CefHandlerBase implementation.
class CefHandlerBase : public CefClient,
                       public CefContextMenuHandler,
                       public CefDisplayHandler,
                       public CefFocusHandler,
                       public CefLifeSpanHandler,
                       public CefLoadHandler {
public:
  CefHandlerBase() {}
  virtual ~CefHandlerBase() {}

  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() { return this; }
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() { return this; }
  virtual CefRefPtr< CefFocusHandler > 	GetFocusHandler() {return this;}
  // CefDisplayHandler methods
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                    bool isLoading, bool canGoBack,
                                    bool canGoForward) {}
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString &url) {}
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString &title) {}
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                const CefString &message,
                                const CefString &source, int line) {
    return false;
  }

  // CefContextMenuHandler methods
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) {}
  virtual bool
  OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                       CefRefPtr<CefContextMenuParams> params, int command_id,
                       CefContextMenuHandler::EventFlags event_flags) {
    return false;
  }
  virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame) {}

  // CefLifeSpanHandler methods
  virtual bool
  OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                const CefString &target_url, const CefString &target_frame_name,
                const CefPopupFeatures &popupFeatures,
                CefWindowInfo &windowInfo, CefRefPtr<CefClient> &client,
                CefBrowserSettings &settings, bool *no_javascript_access) {
    return false;
  }
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) {}
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) { return false; }
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) {}

  // CefLoadHandler methods
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame) {}
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame, int httpStatusCode) {}
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame, ErrorCode errorCode,
                           const CefString &errorText,
                           const CefString &failedUrl) {}

  CefRefPtr<CefBrowser> GetBrowser() { return m_browser; }
  void SetControl(wxWindow *control) { m_control.reset(control); }

  virtual void OnGotFocus( CefRefPtr< CefBrowser > browser );
  virtual bool OnSetFocus( CefRefPtr< CefBrowser > browser, CefFocusHandler::FocusSource source );
  virtual void OnTakeFocus( CefRefPtr< CefBrowser > browser, bool next );

private:
  friend class Cefactory;
  CefRefPtr<CefBrowser> m_browser;
  boost::shared_ptr<wxWindow> m_control;
  int m_browserId;

  IMPLEMENT_REFCOUNTING(CefHandlerBase)
};

} // namespace wide

#endif
