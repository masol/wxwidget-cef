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

#ifndef WIDE_UTILS_SINGLETON_H
#define WIDE_UTILS_SINGLETON_H

#include <boost/scoped_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/utility.hpp>

namespace wide {
template <class T> class Singleton : private boost::noncopyable {
public:
  static T &instance() {
    boost::call_once(T::init, m_once_flag);
    return *m_instance_ptr;
  }
  static void init() { m_instance_ptr.reset(new T()); }

protected:
  ~Singleton() {}
  Singleton() {}

private:
  static boost::scoped_ptr<T> m_instance_ptr;
  static boost::once_flag m_once_flag;
};
} // namespace wide

template <class T> boost::scoped_ptr<T> wide::Singleton<T>::m_instance_ptr(0);
template <class T>
boost::once_flag wide::Singleton<T>::m_once_flag = BOOST_ONCE_INIT;

#endif // WIDE_UTILS_SINGLETON_H
