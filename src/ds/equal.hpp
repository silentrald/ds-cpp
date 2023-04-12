/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-29
 *===============================*/

#ifndef DS_EQUAL_HPP
#define DS_EQUAL_HPP

#include "./string.hpp"

namespace ds {

template <typename T> class equal {
public:
  [[nodiscard]] bool operator()(const T& t1, const T& t2) const noexcept {
    return t1 == t2;
  }
};

template <> class equal<string> {
public:
  [[nodiscard]] bool
  operator()(const string& str1, const string& str2) const noexcept {
    return str1 == str2;
  }

  [[nodiscard]] bool
  operator()(const char* str1, const string& str2) const noexcept {
    return str1 == str2;
  }

  [[nodiscard]] bool
  operator()(const string& str1, const char* str2) const noexcept {
    return str1 == str2;
  }

  [[nodiscard]] bool
  operator()(const char* str1, const char* str2) const noexcept {
    return str1 == str2;
  }
};

} // namespace ds

#endif

