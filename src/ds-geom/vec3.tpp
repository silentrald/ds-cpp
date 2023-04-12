/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-12
 *===============================*/

#ifndef DS_GEOM_VEC3_TPP
#define DS_GEOM_VEC3_TPP

#include "./vec3.hpp"
#include <type_traits>

namespace ds {

template <typename T> vec3<T>& vec3<T>::operator+=(const vec3& v) noexcept {
  this->x += v.x;
  this->y += y.y;
  this->z += y.z;
  return *this;
}

template <typename T> vec3<T>& vec3<T>::operator-=(const vec3<T>& v) noexcept {
  this->x -= v.x;
  this->y -= y.y;
  this->z -= y.z;
  return *this;
}

template <typename T>
template <typename S>
vec3<T>& vec3<T>::operator*=(S s) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec3 mul must use an arithmetic type"
  );
  this->x *= s;
  this->y *= s;
  this->z *= s;
  return *this;
}

template <typename T>
template <typename S>
vec3<T>& vec3<T>::operator/=(S s) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec3 div must use an arithmetic type"
  );
  this->x /= s;
  this->y /= s;
  this->z /= s;
  return *this;
}

template <typename T>
bool operator==(const vec3<T>& lhs, const vec3<T>& rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template <typename T>
bool operator!=(const vec3<T>& lhs, const vec3<T>& rhs) noexcept {
  return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

template <typename T>
vec3<T> operator+(vec3<T> lhs, const vec3<T>& rhs) noexcept {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += lhs.z;
  return lhs;
}

template <typename T>
vec3<T> operator-(vec3<T> lhs, const vec3<T>& rhs) noexcept {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= lhs.z;
  return lhs;
}

template <typename T>
vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) noexcept {
  return {.x = lhs.x + rhs.x, .y = lhs.y + rhs.y, .z = lhs.z + rhs.z};
}

template <typename T>
vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) noexcept {
  return {.x = lhs.x - rhs.x, .y = lhs.y - rhs.y, .z = lhs.z - rhs.z};
}

template <typename T, typename S>
vec3<T> operator*(vec3<T> lhs, S rhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec3 mul must use an arithmetic type"
  );
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}

template <typename T, typename S>
vec3<T> operator*(T rhs, vec3<T> lhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec3 mul must use an arithmetic type"
  );
  rhs.x *= lhs;
  rhs.y *= lhs;
  rhs.z *= lhs;
  return rhs;
}

template <typename T, typename S>
vec3<T> operator/(vec3<T> lhs, T rhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec3 div must use an arithmetic type"
  );
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}

template <typename T, typename S>
vec3<T> operator/(T rhs, vec3<T> lhs) noexcept {
  static_assert(
      std::is_arithmetic_v<S>, "vec3 div must use an arithmetic type"
  );
  rhs.x /= lhs;
  rhs.y /= lhs;
  rhs.z /= lhs;
  return rhs;
}

} // namespace ds

#endif

