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

#ifndef WIDE_UTILS_PATHEXT_H
#define WIDE_UTILS_PATHEXT_H

//#define BOOST_FILESYSTEM_VERSION 3
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

/**
 * @file        pathext.h
 * @brief       扩展了boost::path，增加了link友好的canonicalize方法。
 **/

namespace boost {
namespace filesystem {
namespace wide_ext {

/** @brief
 * 对p执行canonicalize操作，不同于boost::path中的方法，本方法正确处理了符号链接。
 */
boost::filesystem::path canonicalize(const boost::filesystem::path &p);

/** @brief
 * 解析路径p，并将其中含有符号链接的部分替换为实际路径。同时对结果执行canonicalize操作。
 */
boost::filesystem::path resolve_sym(const boost::filesystem::path &p);

/** @brief 读取符号链接result所对应的真实路径。
 *   @param result result保存了请求转化的符号链接，必须其是一个符号链接。
 */
bool read_symlink(boost::filesystem::path &result);

/** @brief 获取指定路径的后缀。
 **/
std::string get_extension(const std::string &path);

/** @brief 在给定文件名的基础上，通过添加数字后缀的方式返回一个尚未创建的文件。
 **/
inline boost::filesystem::path
ensureNonExistFile(const boost::filesystem::path &src_target) {
  if (boost::filesystem::exists(src_target)) { //目标文件已经存在。添加后缀.
    boost::filesystem::path target(src_target);
    std::string extension = target.extension().string();
    const std::string &fullpath = target.string();
    std::string stempath =
        target.string().substr(0, fullpath.length() - extension.length());
    int suffix = 1;
    do {
      target = boost::filesystem::path(
          stempath + boost::lexical_cast<std::string>(suffix++) + extension);
    } while (boost::filesystem::exists(target));
    return target;
  }
  return src_target;
}

inline char getSeparator(void) {
#ifdef WIN32
  return '\\';
#else
  return '/';
#endif
}
inline void replaceSeparator(std::string &srcPath) {
#ifdef WIN32
  boost::replace_all(srcPath, "/", "\\");
#else
  boost::replace_all(srcPath, "\\", "/");
#endif
}

struct fileFetcher {
public:
  typedef boost::function<bool(const std::string &, std::string &)>
      type_fn_getfile;
  inline void setGetFunction(type_fn_getfile fn) { m_fn_getfile = fn; }
  std::string findFile(const std::string &srcpath, const std::string &basepath);

protected:
  type_fn_getfile m_fn_getfile;
};

} // namespace wide_ext
} // namespace filesystem
} // namespace boost

#endif // WIDE_UTILS_PATHEXT_H
