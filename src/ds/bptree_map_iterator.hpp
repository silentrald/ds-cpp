/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-02
 *===============================*/

#ifndef DS_BPTREE_MAP_ITERATOR_HPP
#define DS_BPTREE_MAP_ITERATOR_HPP

#include "./types.hpp"

namespace ds {

template <typename BPTreeMap> class bptree_map_iterator {
public:
  using key_type = typename BPTreeMap::key_type;
  using value_type = typename BPTreeMap::value_type;
  using leaf_node = typename BPTreeMap::leaf_node;

private:
  leaf_node* pointer = nullptr;
  isize index = 0;

public:
  bptree_map_iterator() noexcept = default;

  explicit bptree_map_iterator(leaf_node* pointer) noexcept
      : pointer(pointer) {}

  explicit bptree_map_iterator(leaf_node* pointer, isize index) noexcept
      : pointer(pointer), index(index) {}

  // Element Access
  [[nodiscard]] key_type key() const noexcept {
    return this->pointer->at_key(this->index);
  }

  [[nodiscard]] value_type& value() const noexcept {
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
  const bptree_map_iterator operator++(i32) noexcept {
    bptree_map_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  const bptree_map_iterator operator--(i32) noexcept {
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
