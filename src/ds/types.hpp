/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_TYPES_HPP
#define DS_TYPES_HPP

#include "./expected.hpp"
#include <cstdint>

#if _WIN32 || _WIN64
#if _WIN64
#define BIT_64
#else
#define BIT_64
#endif
#endif

#if __GNUC__
#if __x86_64__ || __ppc64__
#define BIT_64
#else
#define BIT_32
#endif
#endif

#ifdef BIT_64
#define USIZE_FORMAT "%llu"
#else
#define USIZE_FORMAT "%u"
#endif

namespace ds {

using u8 = std::uint8_t;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = std::int8_t;
using i16 = short;
using i32 = int;
using i64 = long long;

using f32 = float;
using f64 = double;

#ifdef BIT_64
using usize = u64;
const usize USIZE_MAX = UINT64_MAX;

using isize = i64;
const isize ISIZE_MIN = INT64_MIN;
const isize ISIZE_MAX = INT64_MAX;
#else
using usize = u32;
const ususize USIZE_MAX = UINT32_MAX;

using isize = i32;
const isize ISIZE_MIN = INT32_MIN;
const isize ISIZE_MAX = INT32_MAX;
#endif

using c8 = char;

enum error_code : u32 {
  OK = 0,
  BAD_ALLOCATION = 1,
  NULL_VALUE = 2, // Accessing null pointer
  INDEX_OUT_OF_BOUNDS = 3,
  CONTAINER_EMPTY = 4,
  CONTAINER_FULL = 5,
  NOT_FOUND = 6,
  INVALID_SIZE = 7, // When capacity is set to a negative value
  NOT_IMPLEMENTED = 8,
  THREAD_ERROR = 9,
};

inline bool is_error(error_code error) noexcept {
  return error != error_code::OK;
}

template <typename T>
inline bool is_error(const expected<T, error_code>& expected) noexcept {
  return !expected;
}

inline unexpected<error_code> to_unexpected(error_code error) noexcept {
  return unexpected{error};
}

} // namespace ds

// NOLINTNEXTLINE
#define GET_MACRO(_1, _2, NAME, ...) NAME

// NOLINTNEXTLINE
#define TRY(...) GET_MACRO(__VA_ARGS__, TRY2, TRY1)(__VA_ARGS__)

// NOLINTNEXTLINE
#define TRY1(expression)                                                       \
  {                                                                            \
    auto error = expression;                                                   \
    if (ds::is_error(error)) {                                                 \
      return std::move(error);                                                 \
    }                                                                          \
  }

// NOLINTNEXTLINE
#define TRY2(expression, converter)                                            \
  {                                                                            \
    auto error = expression;                                                   \
    if (ds::is_error(error)) {                                                 \
      return std::move(converter(error));                                      \
    }                                                                          \
  }

#endif
