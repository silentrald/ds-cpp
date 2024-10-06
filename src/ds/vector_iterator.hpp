/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_VECTOR_ITERATOR_HPP
#define DS_VECTOR_ITERATOR_HPP

#include "types.hpp"

namespace ds {

template <typename Vector> class vector_iterator {
public:
  using value_type = typename Vector::value_type;

private:
  using T = typename Vector::value_type;

  T* pointer = nullptr;

public:
  explicit vector_iterator(T* pointer) noexcept : pointer(pointer) {}

  T& operator*() noexcept {
    return *this->pointer;
  }

  const T& operator*() const noexcept {
    return *this->pointer;
  }

  T* operator->() noexcept {
    return this->pointer;
  }

  const T* operator->() const noexcept {
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

  friend vector_iterator operator+(vector_iterator it, usize offset) noexcept {
    it.pointer += offset;
    return it;
  }

  friend usize
  operator-(const vector_iterator& lhs, const vector_iterator& rhs) noexcept {
    return lhs.pointer - rhs.pointer;
  }
};

} // namespace ds

#endif
