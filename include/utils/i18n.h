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

#ifndef WIDE_UTILS_I18N_H
#define WIDE_UTILS_I18N_H

#include "utils/singleton.h"
#include <boost/function.hpp>

/** @brief 国际化支持
 *  wide的国际化支持有如下假设:
 *    - 字符串推荐使用类boost::format语法来进行格式化。
 *    -
 * 在设置语言及编码之后。随后对WIDE_GETTEXT(str)或者__(string)的调用,如果string对应的翻译存在,则返回翻译文本，否则会返回string。
 *    - 只有可能输出给最终用户的信息才需要国际化。
 */

namespace wide {

/**
 * @brief           国际化支持.
 *                  缺省是线程安全的。如果不需要线程安全，可以定义宏WIDE_NOTHREADSAFE并以--disable-threads选项重新编译ICU库。
 * @todo cmake support WIDE_NOTHREADSAFE configuration.
 **/
class I18n : public Singleton<I18n> {
private:
  friend class Singleton<I18n>;
  typedef Singleton<I18n> inherited;

public:
  typedef boost::function<void(void)> type_switch_handler;

  /**
   * @brief           根据locale获取本地的一个翻译字符串
   * @param msg       [IN], 待翻译的字符串
   * @return
   *返回翻译后的结果.翻译后的结果本线程内下一次调用gettext之前是有效的。
   * @exception
   **/
  const char *gettext(const char *msg);

  /**
   * @brief 初始化国际化支持,从模块所处路径下的data/i18n目录中加载resource.
   * @detail
   *无论是否设置WIDE_NOTHREADSAFE,本函数不是线程安全的，只应该在开始时调用一次。中途如果调用，也应该由调用者维护其线程安全性。
   * @param local     [IN], 指定local的字符串,如果给定NULL,则采用系统当前locale.
   * @param encoding  [IN], 指定编码,如果给定NULL,则采用系统当前encoding.(wide
   *                  use utf-8 always,don't change it!)
   * @return          成功返回true, 失败返回false
   * @exception
   **/
  bool setLocale(const std::string &locale, const char *encoding = "UTF-8",
                 bool fireEvent = false);
  /**
   * @brief           返回系统当前设置的local字符串。
   * @return          成功返回字符串, 失败返回NULL。返回的字符串不能删除。
   * @exception
   **/
  inline const std::string &getSystemLocal(void) {
    if (m_current_local.empty())
      initSystemInfo();
    return m_current_local;
  }

  /**
   * @brief           返回系统当前设置的encoding字符串。
   * @return          成功返回字符串, 失败返回NULL。返回的字符串不能删除。
   * @exception
   **/
  const std::string &getSystemEncoding(void) {
    if (m_current_encoding.empty())
      initSystemInfo();
    return m_current_encoding;
  }

  /** @brief 在系统语言设置被切换时回调。 @deprecated.
   **/
  /*
  inline void onSwitch(type_switch_handler handler) {
    switch_Evt.addEventListen(handler);
  }
  //*/

  static void GetSystemLocaleInfo(std::string &locale, std::string &encoding);

  ~I18n();

private:
  I18n();

  std::string m_current_local;
  std::string m_current_encoding;
  std::locale m_translate_locale;
  // SingleEventListen<type_switch_handler> switch_Evt;

  static void assignSystemInfo(const char *lang, std::string &locale,
                               std::string &encoding);
  void initSystemInfo(void);
  void destroy(void);
};

} // namespace wide

static inline const char *WIDE_GETTEXT(const char *str) {
  return wide::I18n::instance().gettext(str);
}

#define __(str) WIDE_GETTEXT(str)

#endif // WIDE_UTILS_I18N_H
