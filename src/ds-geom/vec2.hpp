/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-12
 *===============================*/

#ifndef DS_GEOM_VEC2_HPP
#define DS_GEOM_VEC2_HPP

#include "ds/error.hpp"
#include "ds/string.hpp"

namespace ds {

template <typename T> class rect;

template <typename T> struct vec2 {
  static_assert(std::is_arithmetic_v<T>, "vec2 should have an arithmetic type");
  T x;
  T y;

  opt_err copy(const vec2& other) noexcept;

  /**
   * Check if the vec2 is within the bounds of the rect. If the point touches
   *   the rect, it is also considered within bounds.
   **/
  [[nodiscard]] bool in_rect(const rect<T>& r) const noexcept;

  /**
   * Check if the vec2 is within the bounds of another vec2. This is the same as
   *   in_rect with rect (0, 0, v.x, v.y)
   **/
  [[nodiscard]] bool in_rect(vec2 v) const noexcept;

  /**
   * Format: (x, y)
   **/
  /* [[nodiscard]] exp_err<string> to_string() noexcept; */

  // === Operators === //

  vec2& operator+=(vec2 v) noexcept;
  vec2& operator-=(vec2 v) noexcept;
  template <typename S>
  vec2& operator*=(S s) noexcept;
  template <typename S>
  vec2& operator/=(S s) noexcept;

  // === Non-Member === //

  template <typename T_>
  friend bool operator==(vec2<T_> lhs, vec2<T_> rhs) noexcept;
  template <typename T_>
  friend bool operator!=(vec2<T_> lhs, vec2<T_> rhs) noexcept;
  template <typename T_>
  friend vec2<T_> operator+(vec2<T_> lhs, vec2<T_> rhs) noexcept;
  template <typename T_>
  friend vec2<T_> operator-(vec2<T_> lhs, vec2<T_> rhs) noexcept;
  template <typename T_, typename S>
  friend vec2<T_> operator*(vec2<T_> lhs, S rhs) noexcept;
  template <typename T_, typename S>
  friend vec2<T_> operator*(S lhs, vec2<T_> rhs) noexcept;
  template <typename T_, typename S>
  friend vec2<T_> operator/(vec2<T_> lhs, T_ rhs) noexcept;
  template <typename T_, typename S>
  friend vec2<T_> operator/(T_ lhs, vec2<T_> rhs) noexcept;
};

} // namespace ds

#ifndef DS_GEOM_VEC2_TPP
#include "./vec2.tpp"
#endif
#endif

