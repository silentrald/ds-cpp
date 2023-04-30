/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-12
 *===============================*/

#ifndef DS_GEOM_VEC3_HPP
#define DS_GEOM_VEC3_HPP

#include "ds/types.hpp"
#include <type_traits>

namespace ds {

template <typename T> struct vec3 {
  static_assert(std::is_arithmetic_v<T>, "vec2 should have an arithmetic type");
  T x;
  T y;
  T z;

  vec3() noexcept = default;
  vec3(const vec3&) noexcept = default;
  vec3(vec3&&) noexcept = default;
  vec3& operator=(const vec3&) noexcept = default;
  vec3& operator=(vec3&&) noexcept = default;
  ~vec3() noexcept = default;

  // === Operators === //

  vec3& operator+=(const vec3& v) noexcept;
  vec3& operator-=(const vec3& v) noexcept;
  template <typename S> vec3& operator*=(S s) noexcept;
  template <typename S> vec3& operator/=(S s) noexcept;

  // === Non-Member === //
  template <typename T_>
  friend bool operator==(const vec3<T_>& lhs, const vec3<T_>& rhs) noexcept;
  template <typename T_>
  friend bool operator!=(const vec3<T_>& lhs, const vec3<T_>& rhs) noexcept;
  template <typename T_>
  friend vec3<T_> operator+(vec3<T_> lhs, const vec3<T_>& rhs) noexcept;
  template <typename T_>
  friend vec3<T_> operator-(vec3<T_> lhs, const vec3<T_>& rhs) noexcept;
  template <typename T_>
  friend vec3<T_> operator+(const vec3<T_>& lhs, const vec3<T_>& rhs) noexcept;
  template <typename T_>
  friend vec3<T_> operator-(const vec3<T_>& lhs, const vec3<T_>& rhs) noexcept;
  template <typename T_, typename S>
  friend vec3<T_> operator*(vec3<T_> lhs, S rhs) noexcept;
  template <typename T_, typename S>
  friend vec3<T_> operator*(S lhs, vec3<T_> rhs) noexcept;
  template <typename T_, typename S>
  friend vec3<T_> operator/(vec3<T_> lhs, S rhs) noexcept;
  template <typename T_, typename S>
  friend vec3<T_> operator/(S lhs, vec3<T_> rhs) noexcept;
};

} // namespace ds

#ifndef DS_GEOM_VEC3_TPP
#include "./vec3.tpp"
#endif

#endif

