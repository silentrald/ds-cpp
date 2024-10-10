/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_COMPARE_HPP
#define DS_COMPARE_HPP

#include "./string.hpp"
#include <cstring>

// NOTE: Subtraction can overflow

namespace ds {

template <typename T> class compare {
public:
  [[nodiscard]] isize operator()(const T& t1, const T& t2) const noexcept {
    return t1 - t2;
  }
};

template <> class compare<i32> {
public:
  [[nodiscard]] isize operator()(i32 i1, i32 i2) const noexcept {
#ifdef BIT_64
    return static_cast<isize>(i1) - static_cast<isize>(i2);
#else
    return i1 > i2 ? 1 : (i1 < i2 ? -1 : 0);
#endif
  }
};

template <> class compare<i64> {
public:
  [[nodiscard]] isize operator()(i64 i1, i64 i2) const noexcept {
    return i1 > i2 ? 1 : (i1 < i2 ? -1 : 0);
  }
};

template <> class compare<u32> {
public:
  [[nodiscard]] isize operator()(u32 u1, u32 u2) const noexcept {
#ifdef BIT_64
    return static_cast<isize>(u1) - static_cast<isize>(u2);
#else
    return u1 > u2 ? 1 : (u1 < u2 ? -1 : 0);
#endif
  }
};

template <> class compare<u64> {
public:
  [[nodiscard]] isize operator()(u64 u1, u64 u2) const noexcept {
    return u1 > u2 ? 1 : (u1 < u2 ? -1 : 0);
  }
};

template <> class compare<const char*> {
  [[nodiscard]] isize
  operator()(const char* str1, const char* str2) const noexcept {
    return std::strcmp(str1, str2);
  }
};

template <> class compare<string> {
public:
  [[nodiscard]] isize
  operator()(const string& str1, const string& str2) const noexcept {
    return std::strcmp(str1.c_str(), str2.c_str());
  }

  [[nodiscard]] isize
  operator()(const string& str1, const char* str2) const noexcept {
    return std::strcmp(str1.c_str(), str2);
  }

  [[nodiscard]] isize
  operator()(const char* str1, const string& str2) const noexcept {
    return std::strcmp(str1, str2.c_str());
  }

  [[nodiscard]] isize
  operator()(const char* str1, const char* str2) const noexcept {
    return std::strcmp(str1, str2);
  }
};

} // namespace ds

#endif
