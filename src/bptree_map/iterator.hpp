/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-02
 * Updated: 2023-01-02
 *===============================*/

#ifndef DS_BPTREE_MAP_ITERATOR_HPP
#define DS_BPTREE_MAP_ITERATOR_HPP

#include "../types.hpp"

namespace ds {

template <typename BPTreeMap> class bptree_map_iterator {
public:
  using key_type = typename BPTreeMap::key_type;
  using key_ptr = key_type*;
  using key_ref = key_type&;

  using value_type = typename BPTreeMap::value_type;
  using value_ptr = value_type*;
  using value_ref = value_type&;

  using leaf_type = typename BPTreeMap::leaf_node;
  using leaf_ptr = leaf_type*;

private:
  leaf_ptr pointer = nullptr;
  i32 index = 0;

public:
  bptree_map_iterator() noexcept = default;

  explicit bptree_map_iterator(leaf_ptr pointer) noexcept : pointer(pointer) {}

  explicit bptree_map_iterator(leaf_ptr pointer, i32 index) noexcept
      : pointer(pointer), index(index) {}

  // Element Access
  key_type key() const noexcept {
    return this->pointer->at_key(this->index);
  }

  value_ref value() const noexcept {
    return this->pointer->at_value(this->index);
  }

  // Prefix
  bptree_map_iterator& operator++() noexcept {
    if (this->pointer == nullptr) {
      return *this;
    }

    ++this->index;
    if (this->pointer->get_size() != this->index) {
      return *this;
    }

    this->index = 0;
    this->pointer = this->pointer->get_next();
    return *this;
  }

  bptree_map_iterator& operator--() noexcept {
    if (this->pointer == nullptr) {
      return *this;
    }

    if (this->index > 0) {
      --this->index;
      return *this;
    }

    this->pointer = this->pointer->get_prev();
    if (this->pointer) {
      this->index = this->pointer->get_size() - 1;
    }
    return *this;
  }

  // Postfix
  const bptree_map_iterator& operator++(i32) noexcept {
    bptree_map_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  const bptree_map_iterator& operator--(i32) noexcept {
    bptree_map_iterator tmp = *this;
    --(*this);
    return tmp;
  }

  // Non-member Operations
  friend bool operator==(
      const bptree_map_iterator& lhs, const bptree_map_iterator& rhs
  ) noexcept {
    return lhs.pointer == rhs.pointer && lhs.index == rhs.index;
  }

  friend bool operator!=(
      const bptree_map_iterator& lhs, const bptree_map_iterator& rhs
  ) noexcept {
    return lhs.pointer != rhs.pointer || lhs.index != rhs.index;
  }
};

} // namespace ds

#endif

