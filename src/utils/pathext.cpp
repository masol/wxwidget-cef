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
#include "utils/pathext.h"
#include "utils/sysinfo.h"
#include <boost/locale.hpp>

#if (BOOST_FILESYSTEM_VERSION != 3)
#ifndef WIN32
#include <unistd.h>
#endif // WIN32
#endif //(BOOST_FILESYSTEM_VERSION != 3)

namespace boost {
namespace filesystem {
namespace wide_ext {

bool read_symlink(boost::filesystem::path &result) {
#if (BOOST_FILESYSTEM_VERSION == 3)
  result = boost::filesystem::read_symlink(result);
  return true;
#elif !defined(WIN32)
  char buf[512];
  if (readlink(result.string().c_str(), buf, 512) > 0) {
    result = boost::filesystem::path(buf);
    return true;
  }
  return false;
#else
  return false;
#endif // WIN32
}

boost::filesystem::path canonicalize(const boost::filesystem::path &p) {
  boost::filesystem::path result;
  for (boost::filesystem::path::iterator it = p.begin(); it != p.end(); ++it) {
    if (*it == "..") {
      // /a/b/.. is not necessarily /a if b is a symbolic link
      if (boost::filesystem::is_symlink(result))
        result /= *it;
      // /a/b/../.. is not /a/b/.. under most circumstances
      // We can end up with ..s in our result because of symbolic links
      else if (result.filename() == "..")
        result /= *it;
      // Otherwise it should be safe to resolve the parent
      else
        result = result.parent_path();
    } else if (*it == ".") {
      // Ignore
    } else {
      // Just cat other path entries
      result /= *it;
    }
  }
  return result;
}

boost::filesystem::path resolve_sym(const boost::filesystem::path &p) {
  boost::filesystem::path result;
  for (boost::filesystem::path::iterator it = p.begin(); it != p.end(); ++it) {
    if (*it == "..") {
      // /a/b/.. is not necessarily /a if b is a symbolic link
      if (boost::filesystem::is_symlink(result)) {
        read_symlink(result);
        result /= *it;
      }
      // /a/b/../.. is not /a/b/.. under most circumstances
      // We can end up with ..s in our result because of symbolic links
      else if (result.filename() == "..")
        result /= *it;
      // Otherwise it should be safe to resolve the parent
      else
        result = result.parent_path();
    } else if (*it == ".") {
      // Ignore
    } else {
      // Just cat other path entries
      result /= *it;
    }
    if (boost::filesystem::is_symlink(result)) {
      read_symlink(result);
    }
  }
  return result;
}

std::string get_extension(const std::string &path) {
  boost::filesystem::path tmp(path);
  return tmp.extension().generic_string();
}

std::string fileFetcher::findFile(const std::string &srcpath,
                                  const std::string &basepath) {
  std::string result;
  boost::filesystem::path path;
  try {
    path = boost::filesystem::absolute(srcpath, basepath);
  } catch (const boost::system::system_error
               &e) { // we assume e.code().category().name() is 'codecvt'.
    (void)e;
  }
  // if any exception, leave it to caller.
  // try{
  if (path.empty()) { // srcpath保存了非utf-8编码。try convert it to utf-8.
    std::string new_srcpath =
        boost::locale::conv::utf_to_utf<char>(wide::SysNativeMBToWide(srcpath));
    path = boost::filesystem::absolute(new_srcpath, basepath);
  }
  if (boost::filesystem::exists(path)) {
    result = path.string();
  } else if (this->m_fn_getfile) {
    if (!m_fn_getfile(path.string(), result)) {
      result.clear();
    }
  }
  //}catch(std::exception &e)
  //{//
  //}
  return result;
}

} // namespace wide_ext
} // namespace filesystem
} // namespace boost
