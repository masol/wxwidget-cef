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

#ifndef WIDE_PRECOMP_H
#define WIDE_PRECOMP_H

//precompiler header.


#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/control.h>
#include <wx/sharedptr.h>
#include <wx/timer.h>
#include <wx/vector.h>
#include <wx/clipbrd.h>

#include <wx/nativewin.h>

#include <boost/log/trivial.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include "nlohmann/json.hpp"
// for convenience
using json = nlohmann::json;


#endif
