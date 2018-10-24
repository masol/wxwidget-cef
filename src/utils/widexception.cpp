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
#include "utils/widexception.h"

#ifdef WIN32
#include "win32/Win32Exception.cpp"
#else
#endif

namespace wide {

void exception::installTranslator() {
#ifdef WIN32
  _set_se_translator(Win32Exception::translate);
#endif
}

void exception::removeTranslator() {
#ifdef WIN32
  _set_se_translator(NULL);
#endif
}

} // namespace wide
