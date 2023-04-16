/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_COMPARE_HPP
#define DS_COMPARE_HPP

#include "ds-error/types.hpp"
#include "ds/string.hpp"
#include <cstring>

namespace ds {

template <typename T> class compare {
public:
  [[nodiscard]] i32 operator()(const T& t1, const T& t2) const noexcept {
    return t1 - t2;
  }
};

template <> class compare<string> {
public:
  [[nodiscard]] i32
  operator()(const string& str1, const string& str2) const noexcept {
    return std::strcmp(str1.c_str(), str2.c_str());
  }

  [[nodiscard]] i32
  operator()(const string& str1, const char* str2) const noexcept {
    return std::strcmp(str1.c_str(), str2);
  }

  [[nodiscard]] i32
  operator()(const char* str1, const string& str2) const noexcept {
    return std::strcmp(str1, str2.c_str());
  }

  [[nodiscard]] i32
  operator()(const char* str1, const char* str2) const noexcept {
    return std::strcmp(str1, str2);
  }
};

} // namespace ds

#endif

