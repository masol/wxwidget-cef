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
#include "utils/cef.h"

namespace wide {

void CefHandlerBase::OnGotFocus(CefRefPtr<CefBrowser> browser) {
  std::cout << "OnGotFocus" << std::endl;
  return;
}

bool CefHandlerBase::OnSetFocus(CefRefPtr<CefBrowser> browser,
                                CefFocusHandler::FocusSource source) {
  std::cout << "OnSetFocus source=" << source << std::endl;
  if(m_control && !m_control->HasFocus()){
    m_control->SetFocus();
  }
  //allow set focus.
  return false;
}

void CefHandlerBase::OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) {
  std::cout << "OnTakeFocus" << std::endl;
}

} // namespace wide
