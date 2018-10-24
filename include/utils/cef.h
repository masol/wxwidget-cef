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

#ifndef WIDE_UTILS_CEF_H
#define WIDE_UTILS_CEF_H

#ifdef __VISUALC__
#pragma warning(push)
#pragma warning(disable : 4100)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_string_visitor.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __VISUALC__
#pragma warning(pop)
#endif

#endif
