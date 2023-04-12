/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-12
 *===============================*/

#ifndef DS_GEOM_VEC2_TPP
#define DS_GEOM_VEC2_TPP

#include "./vec2.hpp"
#include "ds/error.hpp"
#include "ds/optional.hpp"
#include <type_traits>

namespace ds {

template <typename T> opt_err vec2<T>::copy(const vec2& other) noexcept {
  this->x = other.x;
  this->y = other.y;
  return null;
}

// === Operators === //

template <typename T> bool vec2<T>::in_rect(const rect<T>& r) const noexcept {
  return (
      this->x >= r.x && this->x <= r.x + r.w && // X-axis
      this->y >= r.y && this->y <= r.y + r.h    // Y-axis
  );
}

template <typename T> bool vec2<T>::in_rect(vec2 v) const noexcept {
  return (
      this->x >= 0 && this->x <= v.x && // X-axis
      this->y >= 0 && this->y <= v.y    // Y-axis
  );
}

/* template <typename T> exp_err<string> vec2<T>::to_string() noexcept { */
/* } */

template <typename T> vec2<T>& vec2<T>::operator+=(vec2 v) noexcept {
  this->x += v.x;
  this->y += v.y;
  return *this;
}

template <typename T> vec2<T>& vec2<T>::operator-=(vec2 v) noexcept {
  this->x -= v.x;
  this->y -= v.y;
  return *this;
}

template <typename T>
template <typename S>
vec2<T>& vec2<T>::operator*=(S s) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec2 mul must use an arithmetic type"
  );
  this->x *= s;
  this->y *= s;
  return *this;
}

template <typename T>
template <typename S>
vec2<T>& vec2<T>::operator/=(S s) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec2 mul must use an arithmetic type"
  );
  this->x /= s;
  this->y /= s;
  return *this;
}

template <typename T> bool operator==(vec2<T> lhs, vec2<T> rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T> bool operator!=(vec2<T> lhs, vec2<T> rhs) noexcept {
  return lhs.x != rhs.x || lhs.y != rhs.y;
}

template <typename T> vec2<T> operator+(vec2<T> lhs, vec2<T> rhs) noexcept {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

template <typename T> vec2<T> operator-(vec2<T> lhs, vec2<T> rhs) noexcept {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

template <typename T, typename S>
vec2<T> operator*(vec2<T> lhs, S rhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec2 mul must use an arithmetic type"
  );
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}

template <typename T, typename S>
vec2<T> operator*(S lhs, vec2<T> rhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec2 mul must use an arithmetic type"
  );
  rhs.x *= lhs;
  rhs.y *= lhs;
  return rhs;
}

template <typename T, typename S>
vec2<T> operator/(vec2<T> lhs, S rhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec2 div must use an arithmetic type"
  );
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

template <typename T, typename S>
vec2<T> operator/(S lhs, vec2<T> rhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec2 div must use an arithmetic type"
  );
  rhs.x /= lhs;
  rhs.y /= lhs;
  return rhs;
}

} // namespace ds

#endif

