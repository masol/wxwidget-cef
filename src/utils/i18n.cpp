//////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2013 by sanpolo CO.LTD                                    //
//                                                                          //
//  This file is part of WIDE                                               //
//                                                                          //
//  You should have received a copy of the MIT License along with this     //
//  program.  If not, see <http://www.wware.org/wide/license.html>.         //
//                                                                          //
//  WIDE website: http://www.wware.org/                                     //
//////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "utils/i18n.h"
#include "utils/modulepath.h"
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/thread/tss.hpp>

#if WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace wide {

struct Buffer_Info {
protected:
  /*一个通用缓冲区的大小*/
  static const size_t BUFFER_UNIT = 1024;
  char *buffer;
  size_t buffer_len;

public:
  Buffer_Info() : buffer(new char[BUFFER_UNIT]), buffer_len(BUFFER_UNIT) {}
  ~Buffer_Info() {
    if (buffer) {
      delete[] buffer;
      buffer = NULL;
    }
    buffer_len = 0;
  }

public:
  inline size_t getBufferLen(void) const { return buffer_len; }
  inline char *getBuffer(size_t size = 0) {
    if (size < buffer_len) {
      return buffer;
    }
    size_t nbuf_len = BUFFER_UNIT * (size / BUFFER_UNIT + 1);
    delete[] buffer;
    buffer = new char[nbuf_len];
    buffer_len = nbuf_len;
    return buffer;
  }
};

#ifdef WIDE_NOTHREAD_SAFE
static inline Buffer_Info &getBufferInfo(void) {
  static Buffer_Info lv_info;
  return lv_info;
}
#else
static inline Buffer_Info &getBufferInfo(void) {
  static boost::thread_specific_ptr<Buffer_Info> lv_infoStorage;
  if (!lv_infoStorage.get())
    lv_infoStorage.reset(new Buffer_Info());
  return *(lv_infoStorage.get());
}
#endif

const char *I18n::gettext(const char *msg) {
  std::string ctx = boost::locale::gettext(msg, m_translate_locale);
  if (ctx.empty())
    return msg;
  char *line = getBufferInfo().getBuffer();
  if (ctx.length() >= getBufferInfo().getBufferLen()) {
    line = getBufferInfo().getBuffer(ctx.length() + 1);
  }
  strcpy(line, ctx.c_str());
  return line;
}

bool I18n::setLocale(const std::string &locale, const char *encoding,
                     bool fireEvent) {
  bool rt = false;
  //我们输出的目标一定是utf-8.然后由wx转义为系统接受的格式。
  boost::filesystem::path i18npath = ModulePath::instance().modulePath();
  i18npath /= "i18n";
  boost::filesystem::path targetI18n = i18npath / locale;
  if (boost::filesystem::is_directory(i18npath) &&
      boost::filesystem::is_directory(
          targetI18n)) { //只有在目标路径存在时，我们才允许切换翻译用locale.
    boost::locale::generator gen;
    gen.add_messages_path(i18npath.string());
    gen.add_messages_domain("wide");
    //我们输出的目标一定是utf-8.然后由wx转义为系统接受的格式。
    m_translate_locale = gen(locale + '.' + encoding);
    // if (fireEvent) {
    //  switch_Evt.fire();
    //}
    m_current_local = locale;
    m_current_encoding = encoding;
    rt = true;
  }
  return rt;
}

void I18n::destroy(void) {}

void I18n::assignSystemInfo(const char *lang, std::string &locale,
                            std::string &encoding) {
  std::vector<std::string> localArray;
  boost::split(localArray, lang, boost::is_any_of("."),
               boost::token_compress_on);
  if (localArray.size() == 2) {
    locale = localArray[0];
    encoding = localArray[1];
  } else {
    // default value.
    locale = "en_US";
    encoding = "UTF-8";
  }
}

void I18n::GetSystemLocaleInfo(std::string &locale, std::string &encoding) {
  const char *lang = NULL;
  if (!lang)
    lang = std::getenv("LC_CTYPE");
  if (!lang)
    lang = std::getenv("LC_ALL");
  if (!lang)
    lang = std::getenv("LANG");
#ifndef WIN32
  if (!lang)
    lang = "C";
  assignSystemInfo(lang, locale, encoding);
  return;
#else
  if (lang) {
    assignSystemInfo(lang, locale, encoding);
    return;
  }
  char buf[10];
  if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, buf,
                     sizeof(buf)) == 0) {
    assignSystemInfo("C", locale, encoding);
    return;
  }
  std::string lc_name = buf;
  if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, buf,
                     sizeof(buf)) != 0) {
    lc_name += "_";
    lc_name += buf;
  }
  if (GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, buf,
                     sizeof(buf)) != 0) {
    if (atoi(buf) == 0)
      lc_name += ".UTF-8";
    else {
      lc_name += ".windows-";
      lc_name += buf;
    }
  } else {
    lc_name += "UTF-8";
  }
  assignSystemInfo(lc_name.c_str(), locale, encoding);
#endif
}

void I18n::initSystemInfo(void) {
  GetSystemLocaleInfo(this->m_current_local, this->m_current_encoding);
}

I18n::I18n() { setLocale(this->getSystemLocal(), "UTF-8"); }

I18n::~I18n() { destroy(); }

} // namespace wide
