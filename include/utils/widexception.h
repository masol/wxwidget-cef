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

#ifndef WIDE_UTILS_WIDEXCEPTION_H
#define WIDE_UTILS_WIDEXCEPTION_H

namespace wide {

///@brief 安装平台相关的Exception Handler.
struct exception {
  /** @brief 安装异常翻译。这将把se转化为ex抛出。
   **/
  static void installTranslator(void);
  /** @brief 移除异常转化。
   **/
  static void removeTranslator(void);
};

} // namespace wide

#endif // WIDE_UTILS_WIDEXCEPTION_H
