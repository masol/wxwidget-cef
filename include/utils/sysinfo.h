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

#ifndef WIDE_UTILS_SYSINFO_H
#define WIDE_UTILS_SYSINFO_H

#include <string>

/**
 * @file        sysinfo.h
 * @brief       获取信息信息的工具类。
 **/
namespace wide {

struct SysInfo {
  /** @brief 获取系统idle的时间(没有任何用户输入的时间).
   **/
  static int GetSystemIdleMicroSecond(void);
};

std::string SysWideToNativeMB(const std::wstring &wide);
std::wstring SysNativeMBToWide(const std::string &native_mb);

} // namespace wide

#endif // WIDE_UTILS_SYSINFO_H
