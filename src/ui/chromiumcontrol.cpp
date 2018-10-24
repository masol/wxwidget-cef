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
#include "ui/ceffactory.h"
#include "ui/chromiumcontrol.h"
#include "utils/cef.h"

#if defined(USE_X11)
#include "X11/Xlib.h"
#endif

namespace wide {

const wxChar *wxChromiumControlNameStr = wxT("chromiumView");
const wxChar *wxChromiumControlDefURL = wxT("about:blank");

BEGIN_EVENT_TABLE(wxChromiumControl, inherit)
EVT_SIZE(wxChromiumControl::OnSize)
EVT_SET_FOCUS(wxChromiumControl::OnFocus)
EVT_KILL_FOCUS(wxChromiumControl::OnKillFocus)
END_EVENT_TABLE()

bool wxChromiumControl::Create(wxWindow *parent, wxWindowID id,
                               const wxString &url, const wxPoint &pos,
                               const wxSize &size, long style,
                               const wxString &name, const int handlerType) {

  std::cout << "create base Control..." << std::endl;
  if (!inherit::Create(parent, id, pos, size, style, name)) {
    return false;
  }

  std::cout << "create Browser..." << std::endl;
  m_browserHandler =
      Cefactory::instance().createBrowser(Cefactory::Default, this, url);

  // we use BEGIN_EVENT_TABLE. not need this.
  // this->Bind(wxEVT_SIZE, &wxChromiumControl::OnSize, this);

  SetFocus();
  return true;
}

wxChromiumControl::~wxChromiumControl() {
  CefRefPtr<CefBrowser> browser = m_browserHandler->GetBrowser();
  if (browser.get()) {
    // Let the browser window know we are about to destroy it.
    browser->GetHost()->CloseBrowser(true);
  }
}

void wxChromiumControl::OnSize(wxSizeEvent &event) {
  wxSize size = GetClientSize();
#if defined(OS_WIN32)
  if (m_browserHandler && m_browserHandler->GetBrowser() &&
      m_browserHandler->GetBrowser()->GetHost()) {
    HWND handle = m_browserHandler->GetBrowser()->GetHost()->GetWindowHandle();

    if (handle) {
      HDWP hdwp = BeginDeferWindowPos(1);
      hdwp = DeferWindowPos(hdwp, handle, NULL, 0, 0, size.GetWidth(),
                            size.GetHeight(), SWP_NOZORDER);
      EndDeferWindowPos(hdwp);
    }
  }
#elif defined(USE_X11)
  if (m_browserHandler && m_browserHandler->GetBrowser() &&
      m_browserHandler->GetBrowser()->GetHost()) {
    Window handle =
        m_browserHandler->GetBrowser()->GetHost()->GetWindowHandle();
    // std::cout << "has handle=" << handle << std::endl;

    if (handle) {
      auto display = cef_get_xdisplay();
      XMoveResizeWindow(display, handle, 0, 0, size.GetWidth(),
                        size.GetHeight());
      SetFocus();
    }
  }
#else
#error "NOT IMPLEMENT Window system support: to move cef with parent widget..."
#endif
  event.Skip();
}

void wxChromiumControl::OnFocus(wxFocusEvent &event) {

  std::cout << "gather focus" << std::endl;
  if (m_browserHandler && m_browserHandler->GetBrowser() &&
      m_browserHandler->GetBrowser()->GetHost()) {
    CefRefPtr<CefBrowserHost> host = m_browserHandler->GetBrowser()->GetHost();
    std::cout << "set focus true" << std::endl;
    host->SetFocus(true);
  }
  // event.Skip();
}

void wxChromiumControl::OnKillFocus(wxFocusEvent &event) {
  std::cout << "kill focus" << std::endl;
  if (m_browserHandler && m_browserHandler->GetBrowser() &&
      m_browserHandler->GetBrowser()->GetHost()) {
    CefRefPtr<CefBrowserHost> host = m_browserHandler->GetBrowser()->GetHost();
    std::cout << "set focus false" << std::endl;
    host->SetFocus(false);
  }
}

} // namespace wide

/*


void wxWebViewChromium::SetPageSource(const wxString &pageSource) {
m_pageSource = pageSource;
}

void wxWebViewChromium::SetPageText(const wxString &pageText) {
m_pageText = pageText;
}

void *wxWebViewChromium::GetNativeBackend() const {
return m_clientHandler->GetBrowser();
}

bool wxWebViewChromium::CanGoForward() const {
if (m_historyEnabled)
  return m_historyPosition != static_cast<int>(m_historyList.size()) - 1;
else
  return false;
}

bool wxWebViewChromium::CanGoBack() const {
if (m_historyEnabled)
  return m_historyPosition > 0;
else
  return false;
}

void wxWebViewChromium::LoadHistoryItem(
  wxSharedPtr<wxWebViewHistoryItem> item) {
int pos = -1;
for (unsigned int i = 0; i < m_historyList.size(); i++) {
  // We compare the actual pointers to find the correct item
  if (m_historyList[i].get() == item.get())
    pos = i;
}
wxASSERT_MSG(pos != static_cast<int>(m_historyList.size()),
             "invalid history item");
m_historyLoadingFromList = true;
LoadURL(item->GetUrl());
m_historyPosition = pos;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem>>
wxWebViewChromium::GetBackwardHistory() {
wxVector<wxSharedPtr<wxWebViewHistoryItem>> backhist;
// As we don't have std::copy or an iterator constructor in the wxwidgets
// native vector we construct it by hand
for (int i = 0; i < m_historyPosition; i++) {
  backhist.push_back(m_historyList[i]);
}
return backhist;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem>>
wxWebViewChromium::GetForwardHistory() {
wxVector<wxSharedPtr<wxWebViewHistoryItem>> forwardhist;
// As we don't have std::copy or an iterator constructor in the wxwidgets
// native vector we construct it by hand
for (int i = m_historyPosition + 1;
     i < static_cast<int>(m_historyList.size()); i++) {
  forwardhist.push_back(m_historyList[i]);
}
return forwardhist;
}

void wxWebViewChromium::GoBack() {
LoadHistoryItem(m_historyList[m_historyPosition - 1]);
}

void wxWebViewChromium::GoForward() {
LoadHistoryItem(m_historyList[m_historyPosition + 1]);
}

void wxWebViewChromium::LoadURL(const wxString &url) {
m_clientHandler->GetBrowser()->GetMainFrame()->LoadURL(url.ToStdString());
}

void wxWebViewChromium::ClearHistory() {
m_historyList.clear();
m_historyPosition = -1;
}

void wxWebViewChromium::EnableHistory(bool enable) {
m_historyEnabled = enable;
}

void wxWebViewChromium::Stop() { m_clientHandler->GetBrowser()->StopLoad(); }

void wxWebViewChromium::Reload(wxWebViewReloadFlags flags) {
if (flags == wxWEBVIEW_RELOAD_NO_CACHE) {
  m_clientHandler->GetBrowser()->ReloadIgnoreCache();
} else {
  m_clientHandler->GetBrowser()->Reload();
}
}

wxString wxWebViewChromium::GetPageSource() const { return m_pageSource; }

wxString wxWebViewChromium::GetPageText() const { return m_pageText; }

wxString wxWebViewChromium::GetCurrentURL() const {
return m_clientHandler->GetBrowser()->GetMainFrame()->GetURL().ToString();
}

wxString wxWebViewChromium::GetCurrentTitle() const { return m_title; }

void wxWebViewChromium::Print() {
// m_browser->GetMainFrame()->Print();
}

void wxWebViewChromium::Cut() {
m_clientHandler->GetBrowser()->GetMainFrame()->Cut();
}

void wxWebViewChromium::Copy() {
m_clientHandler->GetBrowser()->GetMainFrame()->Copy();
}

void wxWebViewChromium::Paste() {
m_clientHandler->GetBrowser()->GetMainFrame()->Paste();
}

void wxWebViewChromium::Undo() {
m_clientHandler->GetBrowser()->GetMainFrame()->Undo();
}

void wxWebViewChromium::Redo() {
m_clientHandler->GetBrowser()->GetMainFrame()->Redo();
}

void wxWebViewChromium::SelectAll() {
m_clientHandler->GetBrowser()->GetMainFrame()->SelectAll();
}

void wxWebViewChromium::DeleteSelection() {
wxString jsdelete = "if (window.getSelection) { if "
                    "(window.getSelection().deleteFromDocument) { "
                    "window.getSelection().deleteFromDocument(); } }";
RunScript(jsdelete);
}

void wxWebViewChromium::ClearSelection() {
wxString jsclear =
    "if (window.getSelection) { if (window.getSelection().empty) { "
    "window.getSelection().empty(); } }";
RunScript(jsclear);
}

void wxWebViewChromium::RunScript(const wxString &javascript) {
m_clientHandler->GetBrowser()->GetMainFrame()->ExecuteJavaScript(
    javascript.ToStdString(), "", 0);
}

bool wxWebViewChromium::IsBusy() const {
if (m_clientHandler->GetBrowser())
  return m_clientHandler->GetBrowser()->IsLoading();
else
  return false;
}

void wxWebViewChromium::SetEditable(bool enable) {
wxString mode = enable ? "\"on\"" : "\"off\"";
RunScript("document.designMode = " + mode);
}

void wxWebViewChromium::DoSetPage(const wxString &html,
                                const wxString &baseUrl) {
m_clientHandler->GetBrowser()->GetMainFrame()->LoadString(
    html.ToStdString(), baseUrl.ToStdString());
}

wxWebViewZoom wxWebViewChromium::GetZoom() const { return m_zoomLevel; }

void wxWebViewChromium::SetZoom(wxWebViewZoom zoom) {
m_zoomLevel = zoom;

double mapzoom;
// arbitrary way to map our common zoom enum to float zoom
switch (zoom) {
case wxWEBVIEW_ZOOM_TINY:
  mapzoom = -1.0;
  break;

case wxWEBVIEW_ZOOM_SMALL:
  mapzoom = -0.5;
  break;

case wxWEBVIEW_ZOOM_MEDIUM:
  mapzoom = 0.0;
  break;

case wxWEBVIEW_ZOOM_LARGE:
  mapzoom = 0.5;
  break;

case wxWEBVIEW_ZOOM_LARGEST:
  mapzoom = 1.0;
  break;

default:
  wxASSERT(false);
}
m_clientHandler->GetBrowser()->GetHost()->SetZoomLevel(mapzoom);
}

void wxWebViewChromium::SetZoomType(wxWebViewZoomType type) {
// there is only one supported zoom type at the moment so this setter
// does nothing beyond checking sanity
wxASSERT(type == wxWEBVIEW_ZOOM_TYPE_LAYOUT);
}

wxWebViewZoomType wxWebViewChromium::GetZoomType() const {
return wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

bool wxWebViewChromium::CanSetZoomType(wxWebViewZoomType type) const {
return type == wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

void wxWebViewChromium::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) {
// We currently don't support custom scheme handlers
}

bool wxWebViewChromium::StartUp(int &code, const wxString &path) {
CefMainArgs args(wxGetInstance());

// If there is no subprocess then we need to execute on this process
if (path == "") {
  code = CefExecuteProcess(args, NULL);
  if (code >= 0)
    return false;
}

CefSettings settings;
// We use a multithreaded message loop so we don't have to integrate
// with the wx message loop
settings.multi_threaded_message_loop = true;
CefString(&settings.browser_subprocess_path) = path.ToStdString();

return CefInitialize(args, settings, NULL);
}

int wxWebViewChromium::StartUpSubprocess() {
CefMainArgs args(wxGetInstance());

return CefExecuteProcess(args, NULL);
}

void wxWebViewChromium::Shutdown() { CefShutdown(); }

// CefDisplayHandler methods
void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                       bool isLoading, bool canGoBack,
                                       bool canGoForward) {}

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  const CefString &url) {}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                const CefString &title) {
m_webview->m_title = title.ToWString();
wxString target = browser->GetMainFrame()->GetName().ToString();

wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_TITLE_CHANGED, m_webview->GetId(),
                     "", target);
event.SetString(title.ToWString());
event.SetEventObject(m_webview);

m_webview->HandleWindowEvent(event);
}

bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                   const CefString &message,
                                   const CefString &source, int line) {
return false;
}

// CefContextMenuHandler methods
void ClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params,
                                      CefRefPtr<CefMenuModel> model) {
if (!m_webview->IsContextMenuEnabled())
  model->Clear();
}

bool ClientHandler::OnContextMenuCommand(
  CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
  CefRefPtr<CefContextMenuParams> params, int command_id,
  CefContextMenuHandler::EventFlags event_flags) {
return false;
}

void ClientHandler::OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame) {}

// CefLifeSpanHandler methods
bool ClientHandler::OnBeforePopup(
  CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
  const CefString &target_url, const CefString &target_frame_name,
  const CefPopupFeatures &popupFeatures, CefWindowInfo &windowInfo,
  CefRefPtr<CefClient> &client, CefBrowserSettings &settings,
  bool *no_javascript_access) {
wxWebViewEvent *event =
    new wxWebViewEvent(wxEVT_WEBVIEW_NEWWINDOW, m_webview->GetId(),
                       target_url.ToString(), target_frame_name.ToString());
event->SetEventObject(m_webview);
// We use queue event as this function is called on the render thread
m_webview->GetEventHandler()->QueueEvent(event);

return true;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
if (!m_browser.get()) {
  m_browser = browser;
  m_browserId = browser->GetIdentifier();
}
}
bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser) { return false; }

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
if (browser->GetIdentifier() == m_browserId) {
  m_browser = NULL;
}
}

// CefLoadHandler methods
void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame) {
wxString url = frame->GetURL().ToString();
wxString target = frame->GetName().ToString();

wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_NAVIGATING, m_webview->GetId(),
                     url, target);
event.SetEventObject(m_webview);

m_webview->HandleWindowEvent(event);

if (!event.IsAllowed()) {
  // We do not yet have support for vetoing pages
}
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame, int httpStatusCode) {
wxString url = frame->GetURL().ToString();
wxString target = frame->GetName().ToString();

wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_NAVIGATED, m_webview->GetId(), url,
                     target);
event.SetEventObject(m_webview);

m_webview->HandleWindowEvent(event);

if (frame->IsMain()) {
  // Get source code when main frame loads ended.
  CefRefPtr<CefStringVisitor> source_visitor =
      new wxStringVisitor(m_webview, wxStringVisitor::PAGE_SOURCE);
  frame->GetSource(source_visitor);

  // Get page text when main frame loads ended.
  CefRefPtr<CefStringVisitor> text_visitor =
      new wxStringVisitor(m_webview, wxStringVisitor::PAGE_TEXT);
  frame->GetText(text_visitor);

  // As we are complete we also add to the history list, but not if the
  // page is not the main page, ie it is a subframe
  if (m_webview->m_historyEnabled && !m_webview->m_historyLoadingFromList) {
    // If we are not at the end of the list, then erase everything
    // between us and the end before adding the new page
    if (m_webview->m_historyPosition !=
        static_cast<int>(m_webview->m_historyList.size()) - 1) {
      m_webview->m_historyList.erase(m_webview->m_historyList.begin() +
                                         m_webview->m_historyPosition + 1,
                                     m_webview->m_historyList.end());
    }
    wxSharedPtr<wxWebViewHistoryItem> item(
        new wxWebViewHistoryItem(url, m_webview->GetCurrentTitle()));
    m_webview->m_historyList.push_back(item);
    m_webview->m_historyPosition++;
  }
  // Reset as we are done now
  m_webview->m_historyLoadingFromList = false;

  wxWebViewEvent levent(wxEVT_COMMAND_WEBVIEW_LOADED, m_webview->GetId(), url,
                        target);
  levent.SetEventObject(m_webview);

  m_webview->HandleWindowEvent(levent);
}
}

void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame, ErrorCode errorCode,
                              const CefString &errorText,
                              const CefString &failedUrl) {
// We define a macro for convenience
#define ERROR_TYPE_CASE(error, wxtype)                                         \
case (error):                                                                \
  type = wxtype;                                                             \
  break

wxWebViewNavigationError type = wxWEBVIEW_NAV_ERR_OTHER;
switch (errorCode) {
  ERROR_TYPE_CASE(ERR_FAILED, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_ABORTED, wxWEBVIEW_NAV_ERR_USER_CANCELLED);
  ERROR_TYPE_CASE(ERR_INVALID_ARGUMENT, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_INVALID_HANDLE, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_FILE_NOT_FOUND, wxWEBVIEW_NAV_ERR_NOT_FOUND);
  ERROR_TYPE_CASE(ERR_TIMED_OUT, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_FILE_TOO_BIG, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_UNEXPECTED, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_ACCESS_DENIED, wxWEBVIEW_NAV_ERR_AUTH);
  ERROR_TYPE_CASE(ERR_NOT_IMPLEMENTED, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_CONNECTION_CLOSED, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_CONNECTION_RESET, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_CONNECTION_REFUSED, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_CONNECTION_ABORTED, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_CONNECTION_FAILED, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_NAME_NOT_RESOLVED, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_INTERNET_DISCONNECTED, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_SSL_PROTOCOL_ERROR, wxWEBVIEW_NAV_ERR_SECURITY);
  ERROR_TYPE_CASE(ERR_ADDRESS_INVALID, wxWEBVIEW_NAV_ERR_REQUEST);
  ERROR_TYPE_CASE(ERR_ADDRESS_UNREACHABLE, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED, wxWEBVIEW_NAV_ERR_AUTH);
  ERROR_TYPE_CASE(ERR_TUNNEL_CONNECTION_FAILED, wxWEBVIEW_NAV_ERR_CONNECTION);
  ERROR_TYPE_CASE(ERR_NO_SSL_VERSIONS_ENABLED, wxWEBVIEW_NAV_ERR_SECURITY);
  ERROR_TYPE_CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH,
                  wxWEBVIEW_NAV_ERR_SECURITY);
  ERROR_TYPE_CASE(ERR_SSL_RENEGOTIATION_REQUESTED, wxWEBVIEW_NAV_ERR_REQUEST);
  ERROR_TYPE_CASE(ERR_CERT_COMMON_NAME_INVALID,
                  wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_DATE_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_AUTHORITY_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_CONTAINS_ERRORS, wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_NO_REVOCATION_MECHANISM,
                  wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION,
                  wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_REVOKED, wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_CERT_END, wxWEBVIEW_NAV_ERR_CERTIFICATE);
  ERROR_TYPE_CASE(ERR_INVALID_URL, wxWEBVIEW_NAV_ERR_REQUEST);
  ERROR_TYPE_CASE(ERR_DISALLOWED_URL_SCHEME, wxWEBVIEW_NAV_ERR_REQUEST);
  ERROR_TYPE_CASE(ERR_UNKNOWN_URL_SCHEME, wxWEBVIEW_NAV_ERR_REQUEST);
  ERROR_TYPE_CASE(ERR_TOO_MANY_REDIRECTS, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_UNSAFE_REDIRECT, wxWEBVIEW_NAV_ERR_SECURITY);
  ERROR_TYPE_CASE(ERR_UNSAFE_PORT, wxWEBVIEW_NAV_ERR_SECURITY);
  ERROR_TYPE_CASE(ERR_INVALID_RESPONSE, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_INVALID_CHUNKED_ENCODING, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_METHOD_NOT_SUPPORTED, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_UNEXPECTED_PROXY_AUTH, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_EMPTY_RESPONSE, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_RESPONSE_HEADERS_TOO_BIG, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_CACHE_MISS, wxWEBVIEW_NAV_ERR_OTHER);
  ERROR_TYPE_CASE(ERR_INSECURE_RESPONSE, wxWEBVIEW_NAV_ERR_SECURITY);
}

wxString url = failedUrl.ToString();
wxString target = frame->GetName().ToString();
wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_ERROR, m_webview->GetId(), url,
                     target);
event.SetEventObject(m_webview);
event.SetInt(type);
event.SetString(errorText.ToString());

m_webview->HandleWindowEvent(event);
}

//*/
