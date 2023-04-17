/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-12
 *===============================*/

#ifndef DS_GEOM_RECT_HPP
#define DS_GEOM_RECT_HPP

#include "ds/types.hpp"
#include <type_traits>

namespace ds {

template <typename T> struct vec2;

template <typename T> struct rect {
  static_assert(std::is_arithmetic_v<T>, "rect should have an arithmetic type");
  T x;
  T y;
  T w;
  T h;

  err_code copy(const rect& other) noexcept;

  void set_position(T x, T y) noexcept;
  void set_size(T w, T h) noexcept;

  [[nodiscard]] vec2<T>& get_position() const noexcept;
  [[nodiscard]] vec2<T>& get_size() const noexcept;

  // === Collision === //
  [[nodiscard]] rect intersect(const rect& other) const noexcept;
  void shrink(T s) noexcept;
  void grow(T s) noexcept;
};

} // namespace ds

#ifndef DS_GEOM_RECT_TPP
#include "./rect.tpp"
#endif

#endif

