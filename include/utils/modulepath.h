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

/**
 * @file        modulepath.h
 * @brief
 *负责跟踪主模块信息。包括进程启动时的当前路径、模块所在路径以及主模块名等信息。
 **/

#ifndef WIDE_UTILS_MODULEPATH_H
#define WIDE_UTILS_MODULEPATH_H

#include "utils/pathext.h"
#include "utils/singleton.h"
#include <string>

namespace wide {

class ModulePath : public Singleton<ModulePath> {
private:
  friend class Singleton<ModulePath>;
  typedef Singleton<ModulePath> inherited;
  ModulePath()
      : m_initPath(boost::filesystem::wide_ext::resolve_sym(
            boost::filesystem::system_complete(
                boost::filesystem::initial_path<boost::filesystem::path>()))) {}

public:
  /**
   * @brief           	初始化信息，本函数只能在主程序启动时调用一次。
   * @param modulePath    [IN], 由argv[0]所保存的modulePath&moduleName.
   **/
  inline void initialize(const char *modulePath) {
    m_modulePath = boost::filesystem::path(modulePath);
    m_moduleName = m_modulePath.generic_string();
    m_modulePath = boost::filesystem::wide_ext::resolve_sym(m_modulePath);
    m_modulePath = boost::filesystem::absolute(m_modulePath.parent_path());
  }

  /**
   * @brief       是否已经被初始化完毕。
   * @return		true表示已经被初始化，false表示未初始化。
   **/
  inline bool is_initialized(void) const { return m_moduleName.length() != 0; }

  /**
   * @brief       返回模块所处路径。。
   * @return		模块所处路径。
   **/
  inline const boost::filesystem::path &modulePath(void) const {
    return m_modulePath;
  }

  /**
   * @brief       返回进程启动时的路径。
   * @return		进程启动时的路径。
   **/
  inline const boost::filesystem::path &initPath(void) const {
    return m_initPath;
  }

  /**
   * @brief       返回模块文件名。
   * @return		模块文件名.
   **/
  inline const std::string &moduleName(void) const { return m_moduleName; }
  /**
   * @brief 获取当前用户的家路径。
   * @return 如果成功获取到home,返回true.
   * @exception 无。
   **/
  static bool getHomepath(std::string &path);
  /**
   * @brief 获取制定用户的家路径。
   * @param username : 用户名，如果传入空(0l)，则获取当前用户的home路径。
   * @return 如果成功获取到home,返回true.
   * @exception 无。
   * NOT IMPLEMENT
   **/
  // static	bool	getHomepath(std::string& path,const char* username =
  // 0l);
  /**
   * @brief
   *获取系统级的用户配置路径。在posix下返回/etc，在windows下返回Windows路径。
   * @return 如果成功获取到路径,返回true.
   * @exception 无。
   * NOT IMPLEMENT
   **/
  // static	bool	getSysConfigDirectory(std::string &path);
protected:
  std::string m_moduleName;
  boost::filesystem::path m_modulePath;
  boost::filesystem::path m_initPath;
};

} // namespace wide

#endif // WIDE_UTILS_MODULEPATH_H
