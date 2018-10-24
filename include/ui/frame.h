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

#ifndef WIDE_FRAME_H
#define WIDE_FRAME_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <wx/aui/aui.h>
#include <wx/config.h>
#include <wx/filehistory.h>
#include <wx/wx.h>

namespace wide {
class Frame : public wxFrame, public boost::enable_shared_from_this<Frame> {
  typedef wxFrame inherited;

public:
  Frame(wxWindow *parent);
  virtual ~Frame();
  void onCefInited();

protected:
  void onClose(wxCloseEvent &event);

public:
  ///@todo meory leak detected by valgrind, caused by wxAuiManager loading
  /// paninfo using expat xmlparser.
  wxAuiManager m_mgr;
  DECLARE_EVENT_TABLE()
};
} // namespace wide

#endif // WIDE_FRAME_H
