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

#ifndef WIDE_UI_CEFFACTORY_H
#define WIDE_UI_CEFFACTORY_H

#include "utils/singleton.h"
class wxWindow;

namespace wide {

class CefHandlerBase;
class Cefactory : public Singleton<Cefactory> {
protected:
  friend class Singleton<Cefactory>;
  typedef Singleton<Cefactory> inherited;

public:
  enum Handler_Type { Default = 0, WYSIWYG_Editor, Html_Editor };
  CefHandlerBase *createBrowser(const int type,wxWindow *win,const wxString& url);
  ~Cefactory(void) {}
protected:
  Cefactory(void) {}
};

} // namespace wide

#endif
