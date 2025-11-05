/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-05
 *===============================*/

#ifndef DS_HASH_MAP_ITERATOR_HPP
#define DS_HASH_MAP_ITERATOR_HPP

#include "./types.hpp"

namespace ds {

template <typename HashMap> class hash_map_iterator {
public:
  using key_type = typename HashMap::key_type;
  using value_type = typename HashMap::value_type;
  using node_type = typename HashMap::node_type;

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
  hash_map_iterator() noexcept = default;

  explicit hash_map_iterator(node_type* bucket, usize capacity) noexcept
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
  const key_type& key() const noexcept {
    return this->ptr->key;
  }

  value_type& value() noexcept {
    return this->ptr->value;
  }

  const value_type& value() const noexcept {
    return this->ptr->value;
  }

  // === Iterator Move === //
  hash_map_iterator& operator++() noexcept {
    this->find_next_element();
    return *this;
  }

  const hash_map_iterator operator++(i32) noexcept {
    hash_map_iterator tmp = *this;
    this->find_next_element();
    return tmp;
  }

  // === Non-member Operator === //
  friend bool operator==(
      const hash_map_iterator& lhs, const hash_map_iterator& rhs
  ) noexcept {
    return lhs.ptr == rhs.ptr;
  }

  friend bool operator!=(
      const hash_map_iterator& lhs, const hash_map_iterator& rhs
  ) noexcept {
    return lhs.ptr != rhs.ptr;
  }
};

template <typename HashMap> class hash_map_const_iterator {
public:
  using key_type = typename HashMap::key_type;
  using value_type = typename HashMap::value_type;
  using node_type = typename HashMap::node_type;

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
  hash_map_const_iterator() noexcept = default;

  explicit hash_map_const_iterator(node_type* bucket, usize capacity) noexcept
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
  const key_type& key() const noexcept {
    return this->ptr->key;
  }

  const value_type& value() const noexcept {
    return this->ptr->value;
  }

  // === Iterator Move === //
  hash_map_const_iterator& operator++() noexcept {
    this->find_next_element();
    return *this;
  }

  const hash_map_const_iterator operator++(i32) noexcept {
    hash_map_iterator tmp = *this;
    this->find_next_element();
    return tmp;
  }

  // === Non-member Operator === //
  friend bool operator==(
      const hash_map_const_iterator& lhs, const hash_map_const_iterator& rhs
  ) noexcept {
    return lhs.ptr == rhs.ptr;
  }

  friend bool operator!=(
      const hash_map_const_iterator& lhs, const hash_map_const_iterator& rhs
  ) noexcept {
    return lhs.ptr != rhs.ptr;
  }
};

} // namespace ds

#endif
