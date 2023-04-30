/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-12
 *===============================*/

#ifndef DS_GEOM_RECT_TPP
#define DS_GEOM_RECT_TPP

#include "./rect.hpp"
#include "ds-geom/vec2.hpp"
#include "ds/optional.hpp"
#include "ds/types.hpp"
#include <algorithm>

namespace ds {

// === Operators === //

template <typename T> void rect<T>::set_position(T x, T y) noexcept {
  this->x = x;
  this->y = y;
}

template <typename T> void rect<T>::set_size(T w, T h) noexcept {
  this->w = w;
  this->h = h;
}

template <typename T> vec2<T>& rect<T>::get_position() const noexcept {
  return *(vec2<T>*)this;
}

template <typename T> vec2<T>& rect<T>::get_size() const noexcept {
  return *(vec2<T>*)(&this->w);
}

template <typename T>
rect<T> rect<T>::intersect(const rect& other) const noexcept {
  rect<T> r{
      .x = std::max(this->x, other.x),
      .y = std::max(this->y, other.y),
      .w = std::min(this->x + this->w, other.x + other.w),
      .h = std::min(this->y + this->h, other.y + other.h)};
  r.w -= r.x;
  r.h -= r.y;
  return r;
}

template <typename T> void rect<T>::shrink(T s) noexcept {
  this->x += s;
  this->y += s;
  this->w -= 2 * s;
  this->h -= 2 * s;
}

template <typename T> void rect<T>::grow(T s) noexcept {
  this->x -= s;
  this->y -= s;
  this->w += 2 * s;
  this->h += 2 * s;
}

} // namespace ds

#endif

