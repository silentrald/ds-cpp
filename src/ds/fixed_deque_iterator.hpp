/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_DEQUE_FIXED_ITERATOR_HPP
#define DS_DEQUE_FIXED_ITERATOR_HPP

#include "./types.hpp"

namespace ds {

template <typename FDeque> class fixed_deque_iterator {
public:
  using value = typename FDeque::value;
  using ptr = value*;
  using ref = value&;

private:
  ptr _ptr = nullptr;
  i32 head = 0;
  i32 capacity = 0;

public:
  explicit fixed_deque_iterator(ptr _ptr, i32 head, i32 capacity) noexcept
      : _ptr(_ptr), head(head), capacity(capacity) {
    if (head > capacity) {
      this->head = 0;
    }
  }

  ref operator*() const {
    return *(this->_ptr + this->head);
  }

  ptr operator->() {
    return this->_ptr + this->head;
  }

  fixed_deque_iterator& operator++() {
    ++this->head;
    if (this->head > this->capacity) {
      this->head = 0;
    }

    return *this;
  }

  friend bool
  operator==(const fixed_deque_iterator& lhs, const fixed_deque_iterator& rhs) {
    return lhs._ptr == rhs._ptr && lhs.head == rhs.head;
  }

  friend bool
  operator!=(const fixed_deque_iterator& lhs, const fixed_deque_iterator& rhs) {
    return lhs._ptr != rhs._ptr || lhs.head != rhs.head;
  }

  friend fixed_deque_iterator operator+(fixed_deque_iterator it, i32 offset) {
    it.head = (it.head + offset) % it.capacity;

    return it;
  }

  friend i32
  operator-(const fixed_deque_iterator& lhs, const fixed_deque_iterator& rhs) {
    return lhs.head >= rhs.head ? lhs.head - rhs.head
                                : lhs.head - rhs.head + lhs.capacity;
  }
};

} // namespace ds

#endif

