/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-05
 *===============================*/

#ifndef DS_HASH_SET_ITERATOR_HPP
#define DS_HASH_SET_ITERATOR_HPP

#include "./types.hpp"

namespace ds {

template <typename HashSet> class hash_set_iterator {
public:
  using key_type = typename HashSet::key_type;
  using node_type = typename HashSet::node_type;

private:
  node_type* bucket = nullptr;
  node_type* ptr = nullptr;
  usize index = 0;
  usize capacity = 0;

  void find_next_element() noexcept {
    if (this->ptr == nullptr) {
      return;
    }

    // Find a bucket with a node
    while (++this->index < this->capacity) {
      this->ptr = this->bucket + this->index;
      if (!this->ptr->is_node_empty()) {
        return;
      }
    }

    // No more nodes
    this->ptr = nullptr;
  }

public:
  hash_set_iterator() noexcept = default;

  explicit hash_set_iterator(node_type* bucket, usize capacity) noexcept
      : bucket(bucket), capacity(capacity) {
    // Find the first element
    for (this->index = 0; this->index < capacity; ++this->index) {
      this->ptr = bucket + this->index;
      if (!this->ptr->is_node_empty()) {
        return;
      }
    }
    this->ptr = nullptr;
  }

  // === Element Access === //
  const key_type& operator*() const noexcept {
    return this->ptr->key;
  }

  const key_type* operator->() const noexcept {
    return &this->ptr->key;
  }

  // === Iterator Move === //
  hash_set_iterator& operator++() noexcept {
    this->find_next_element();
    return *this;
  }

  const hash_set_iterator operator++(i32) noexcept {
    hash_set_iterator tmp = *this;
    this->find_next_element();
    return tmp;
  }

  // === Non-member Operator === //
  friend bool operator==(
      const hash_set_iterator& lhs, const hash_set_iterator& rhs
  ) noexcept {
    return lhs.ptr == rhs.ptr;
  }

  friend bool operator!=(
      const hash_set_iterator& lhs, const hash_set_iterator& rhs
  ) noexcept {
    return lhs.ptr != rhs.ptr;
  }
};

template <typename HashSet> class hash_set_const_iterator {
public:
  using key_type = typename HashSet::key_type;
  using node_type = typename HashSet::node_type;

private:
  node_type* bucket = nullptr;
  node_type* ptr = nullptr;
  usize index = 0;
  usize capacity = 0;

  void find_next_element() noexcept {
    if (this->ptr == nullptr) {
      return;
    }

    // Find a bucket with a node
    while (++this->index < this->capacity) {
      this->ptr = this->bucket + this->index;
      if (!this->ptr->is_node_empty()) {
        return;
      }
    }

    // No more nodes
    this->ptr = nullptr;
  }

public:
  hash_set_const_iterator() noexcept = default;

  explicit hash_set_const_iterator(node_type* bucket, usize capacity) noexcept
      : bucket(bucket), capacity(capacity) {
    // Find the first element
    for (this->index = 0; this->index < capacity; ++this->index) {
      this->ptr = bucket + this->index;
      if (!this->ptr->is_node_empty()) {
        return;
      }
    }
    this->ptr = nullptr;
  }

  // === Element Access === //
  const key_type& operator*() const noexcept {
    return this->ptr->key;
  }

  const key_type* operator->() const noexcept {
    return &this->ptr->key;
  }

  // === Iterator Move === //
  hash_set_const_iterator& operator++() noexcept {
    this->find_next_element();
    return *this;
  }

  const hash_set_const_iterator operator++(i32) noexcept {
    hash_set_iterator tmp = *this;
    this->find_next_element();
    return tmp;
  }

  // === Non-member Operator === //
  friend bool operator==(
      const hash_set_const_iterator& lhs, const hash_set_const_iterator& rhs
  ) noexcept {
    return lhs.ptr == rhs.ptr;
  }

  friend bool operator!=(
      const hash_set_const_iterator& lhs, const hash_set_const_iterator& rhs
  ) noexcept {
    return lhs.ptr != rhs.ptr;
  }
};

} // namespace ds

#endif
