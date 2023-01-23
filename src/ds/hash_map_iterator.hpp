/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-05
 * Updated: 2023-01-05
 *===============================*/

#ifndef DS_HASH_MAP_ITERATOR_HPP
#define DS_HASH_MAP_ITERATOR_HPP

#include "../types.hpp"

namespace ds {

template <typename HashMap> class hash_map_iterator {
public:
  using key_type = typename HashMap::key_type;
  using key_ptr = key_type*;
  using key_ref = key_type&;
  using key_cref = const key_type&;

  using value_type = typename HashMap::value_type;
  using value_ptr = value_type*;
  using value_ref = value_type&;
  using value_cref = const value_type&;

  using node_type = typename HashMap::node_type;
  using node_ptr = node_type*;

  using container = typename HashMap::bucket_container;
  using container_ptr = container*;

private:
  container_ptr base = nullptr;
  node_ptr ptr = nullptr;
  i32 index = 0;

  void find_next_element() noexcept {
    if (this->ptr == nullptr) {
      return;
    }

    // If there is another node on the linked list
    if (this->ptr->next) {
      this->ptr = this->ptr->next;
      return;
    }

    ++this->index; // Move the index cursor

    // Find a bucket with a node
    node_ptr node = nullptr;
    while (this->index < this->base->size()) {
      node = (*this->base)[this->index];
      if (node) {
        this->ptr = node;
        return;
      }

      ++this->index;
    }

    // No more nodes
    this->ptr = nullptr;
  }

public:
  hash_map_iterator() noexcept = default;

  explicit hash_map_iterator(container_ptr base) noexcept : base(base) {
    // Find the first element
    for (auto* node : *this->base) {
      if (node) {
        this->ptr = node;
        return;
      }

      ++this->index;
    }
  }

  explicit hash_map_iterator(container_ptr base, key_ptr ptr) noexcept
      : base(base), ptr(ptr) {}

  // === Element Access === //
  key_cref key() const noexcept {
    return this->ptr->key;
  }

  value_ref value() noexcept {
    return this->ptr->value;
  }

  value_cref value() const noexcept {
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
  using key_ptr = key_type*;
  using key_ref = key_type&;
  using key_cref = const key_type&;

  using value_type = typename HashMap::value_type;
  using value_ptr = value_type*;
  using value_ref = value_type&;
  using value_cref = const value_type&;

  using node_type = typename HashMap::node_type;
  using node_ptr = node_type*;

  using container = typename HashMap::bucket_container;
  using container_cptr = const container*;

private:
  container_cptr base;
  node_ptr ptr = nullptr;
  i32 index = 0;

  void find_next_element() noexcept {
    if (this->ptr == nullptr) {
      return;
    }

    // If there is another node on the linked list
    if (this->ptr->next) {
      this->ptr = this->ptr->next;
      return;
    }

    ++this->index; // Move the index cursor

    // Find a bucket with a node
    node_ptr node = nullptr;
    while (this->index < this->base->size()) {
      node = (*this->base)[this->index];
      if (node) {
        this->ptr = node;
        return;
      }

      ++this->index;
    }

    // No more nodes
    this->ptr = nullptr;
  }

public:
  hash_map_const_iterator() noexcept = default;

  explicit hash_map_const_iterator(container_cptr base) noexcept : base(base) {
    // Find the first element
    node_ptr node = nullptr;
    while (this->index < this->base->size()) {
      node = (*this->base)[this->index];
      if (node) {
        this->ptr = node;
        return;
      }

      ++this->index;
    }
  }

  explicit hash_map_const_iterator(container_cptr base, key_ptr ptr) noexcept
      : base(base), ptr(ptr) {}

  // === Element Access === //
  key_cref key() const noexcept {
    return this->ptr->key;
  }

  value_cref value() const noexcept {
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

