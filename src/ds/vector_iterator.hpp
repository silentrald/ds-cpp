/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_VECTOR_ITERATOR_HPP
#define DS_VECTOR_ITERATOR_HPP

#include "./types.hpp"

namespace ds {

template <typename Vector> class vector_iterator {
public:
  using value = typename Vector::value;
  using ptr = value*;
  using ref = value&;
  using cref = const value&;
  using cptr = const value*;

private:
  ptr pointer = nullptr;

public:
  explicit vector_iterator(ptr pointer) noexcept : pointer(pointer) {}

  ref operator*() noexcept {
    return *this->pointer;
  }

  cref operator*() const noexcept {
    return *this->pointer;
  }

  ptr operator->() noexcept {
    return this->pointer;
  }

  cptr operator->() const noexcept {
    return this->pointer;
  }

  // Prefix
  vector_iterator& operator++() noexcept {
    ++this->pointer;
    return *this;
  }

  vector_iterator& operator--() noexcept {
    --this->pointer;
    return *this;
  }

  // Postfix
  const vector_iterator operator++(i32) noexcept {
    vector_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  const vector_iterator operator--(i32) noexcept {
    vector_iterator tmp = *this;
    --(*this);
    return tmp;
  }

  // Non-member operators
  friend bool
  operator==(const vector_iterator& lhs, const vector_iterator& rhs) noexcept {
    return lhs.pointer == rhs.pointer;
  }

  friend bool
  operator!=(const vector_iterator& lhs, const vector_iterator& rhs) noexcept {
    return lhs.pointer != rhs.pointer;
  }

  friend vector_iterator operator+(vector_iterator it, i32 offset) noexcept {
    it.pointer += offset;
    return it;
  }

  friend i32
  operator-(const vector_iterator& lhs, const vector_iterator& rhs) noexcept {
    return lhs.pointer - rhs.pointer;
  }
};

} // namespace ds

#endif

