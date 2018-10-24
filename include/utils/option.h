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

#ifndef WIDE_UTILS_OPTION_H
#define WIDE_UTILS_OPTION_H

#include "utils/singleton.h"
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace wide {

/**说明:
 *     一个不可以有多个同名的属性(虽然property允许)，否则get等函数
 *     将不确定返回哪个值,通过类Option进行操作，不会产生多个同名属
 *     性的情况
 **/

/**
 * @brief           Option类，负责维护一些全局的信息
 * @see							OptionTest
 **/
class Option : public Singleton<Option> {
protected:
  friend class Singleton<Option>;
  typedef Singleton<Option> inherited;

public:
  /**
   * @brief           属性被操作的行为类型
   **/
  enum Act_Type { ACT_NULL = 0, ACT_PUT, ACT_ERASE };
  typedef enum Act_Type Act_Type;

  /**
   * @brief           slottype属性的监视函数,供回调
   * @param arg1      [IN], 属性名
   * @param arg2      [IN|OUT], 属性值，根据第三个参数，可能是一个可忽略的值
   * @param arg3      [IN], 属性发生的事件
   * @return          返回true，操作将继续执行，返回false, 操作不执行
   **/
  typedef boost::function<bool(const std::string &, boost::any &, Act_Type)>
      slottype;

  typedef boost::property_tree::basic_ptree<std::string, boost::any> ptree_any;
  typedef boost::property_tree::basic_ptree<std::string, slottype> ptree_slot;

protected:
  ptree_any m_ptree;  /**<属性树 */
  ptree_slot m_slots; /**<slottype表 */

#ifndef WIDE_NOTHREAD_SAFE
  /**
   *	@brief 允许在多线程之间访问Option对象。
   *	@details
   *我们只需要一个锁，这增加了潜在的锁定等待时间。但是option应该不会成为hotspot.因此，一个锁的策略其粒度应该足够了。
   **/
  boost::shared_mutex m_mutex; /**<mutex for m_ptree */
#endif                         // WIDE_NOTHREAD_SAFE

  Option(void) {}

public:
  /**
   * @brief
   *从命令行以及配置文件初始化option对象。本方法一并初始化了module路径信息.
   * @param argc [IN],命令行参数个数。
   * @param argv [IN],命令参数。
   * @return
   *是否不需要退出，返回true需要继续执行。返回false发生了参数错误或者执行了帮助显示。
   * @exception  NULL.
   **/
  bool initFromArgs(int argc, char **argv);


  /**
   * @brief
   * load json configurationFile from personal directory.
   * @exception  NULL.
   **/
  void loadConfigurationFile(void);

  /**
   * @brief           获取属性的值。
   * @param path      [IN], 属性的路径
   * @return          返回属性的值。
   * @exception       如果所指路径不存在，throw ptree_bad_path,
   *如果类型转化失败， throw boost::bad_any_cast
   **/
  template <typename Type> Type get(const std::string &path) {
#ifndef WIDE_NOTHREAD_SAFE
    boost::shared_lock<boost::shared_mutex> l(m_mutex);
#endif
    return boost::any_cast<Type>(m_ptree.get<boost::any>(path));
  }

  /**
   * @brief           获取属性的值。
   * @param path      [IN], 属性的路径
   * @return          指定路径的属性值。
   * @exception       不抛异常
   **/
  template <typename Type>
  boost::optional<Type> get_optional(const std::string &path) {
#ifndef WIDE_NOTHREAD_SAFE
    boost::shared_lock<boost::shared_mutex> l(m_mutex);
#endif
    boost::optional<boost::any> value = m_ptree.get_optional<boost::any>(path);
    if (value.is_initialized()) {
      try {
        Type t = boost::any_cast<Type>(*value);
        return boost::optional<Type>(t);
      } catch (...) {
        return boost::optional<Type>();
      }
    }
    return boost::optional<Type>();
  }

  template <typename Type>
  inline Type get(const std::string &path, Type defaultValue) {
    boost::optional<Type> opt_value = get_optional<Type>(path);
    if (opt_value.is_initialized())
      return *opt_value;
    return defaultValue;
  }

  /**
   * @brief
   *给一个属性添加一个监视functor，当该属性的值发生改变的时候，functor会被调用
   * @param path      [IN], 要监视的属性
   * @param s         [IN], 回调functor
   * @return          返回是否监视成功，如果成功，返回旧的监视函数
   * @exception       如果属性名不存在，throw ptree_bad_path
   **/
  boost::optional<slottype> connect(const std::string &path, slottype s) {
#ifndef WIDE_NOTHREAD_SAFE
    boost::unique_lock<boost::shared_mutex> l(m_mutex);
#endif

    /*如果属性名不存在的话，直接让抛异常 */
    // m_ptree.get<boost::any>(path);

    /*检查该属性是否有监视函数, 如果有，返回 */
    boost::optional<slottype> old = m_slots.get_optional<slottype>(path);
    m_slots.put(path, s);
    return old;
  }

  /**
   * @brief           放弃监测指定路径下的属性变化
   * @param path      [IN], 被监视的属性
   * @return          true or false.
   **/
  void disconnect(const std::string &path) {
#ifndef WIDE_NOTHREAD_SAFE
    boost::unique_lock<boost::shared_mutex> l(m_mutex);
#endif
    m_slots.erase(path);
  }

  /**
   * @brief           设置一个属性值
   * @param path      [IN],属性的路径
   * @param value     [IN],属性的值
   * @exception       put失败，throw ptree_error
   **/
  template <typename Type>
  void put(const std::string &path, const Type &value) {
    boost::optional<slottype> slot;
    {
#ifndef WIDE_NOTHREAD_SAFE
      boost::unique_lock<boost::shared_mutex> l(m_mutex);
#endif
      slot = m_slots.get_optional<slottype>(path);
    }

    bool bPutValue = true;
    boost::any value_any = value;
    /*如果有监视函数，则调用监视函数 */
    if (slot.is_initialized()) {
      //调用slot,返回值决定了是否更新，value_any有可能在回调中被修改。
      bPutValue = (*slot)(path, value_any, ACT_PUT);
    }

    /*put属性值 */
    if (bPutValue) {
#ifndef WIDE_NOTHREAD_SAFE
      boost::unique_lock<boost::shared_mutex> l(m_mutex);
#endif
      m_ptree.put(path, value_any);
    }
  }

  /**
   * @brief           判断一个属性值是否存在
   * @param path      [IN], 属性的路径
   * @return          存在返回true, 否则返回false
   **/
  bool is_existed(const std::string &path) {
#ifndef WIDE_NOTHREAD_SAFE
    boost::shared_lock<boost::shared_mutex> ls(m_mutex);
#endif
    boost::optional<boost::any> value = m_ptree.get_optional<boost::any>(path);
    return value.is_initialized();
    /**其他异常往上抛 */
  }

  /**
   * @brief           删除一个属性，并删除该属性对应的监视函数
   **/
  void erase(const std::string &path) {

    boost::optional<slottype> slot;
    {
#ifndef WIDE_NOTHREAD_SAFE
      boost::unique_lock<boost::shared_mutex> l(m_mutex);
#endif
      slot = m_slots.get_optional<slottype>(path);
    }

    bool bErase = true;
    boost::any value_any = 0;
    /*如果有监视函数，则调用监视函数 */
    if (slot.is_initialized()) {
      //调用slot,返回值决定了是否更新，value_any有可能在回调中被修改。
      bErase = (*slot)(path, value_any, ACT_ERASE);
    }

    if (bErase) {
#ifndef WIDE_NOTHREAD_SAFE
      boost::unique_lock<boost::shared_mutex> l(m_mutex);
#endif
      /* 删除属性 */
      m_ptree.erase(path);
      /* 删除监视函数 */
      m_slots.erase(path);
    }
  }

  ~Option(void) {}
};

/**
 * @brief           get函数对boost::any的特化
 **/
template <> inline boost::any Option::get<boost::any>(const std::string &path) {
#ifndef WIDE_NOTHREAD_SAFE
  boost::shared_lock<boost::shared_mutex> l(m_mutex);
#endif
  return m_ptree.get<boost::any>(path);
}

/**
 * @brief           get_optional对boost::any类型的特化
 **/
template <>
inline boost::optional<boost::any>
Option::get_optional<boost::any>(const std::string &path) {
#ifndef WIDE_NOTHREAD_SAFE
  boost::shared_lock<boost::shared_mutex> l(m_mutex);
#endif
  return m_ptree.get_optional<boost::any>(path);
}

} // namespace wide

#endif // WIDE_UTILS_OPTION_H
