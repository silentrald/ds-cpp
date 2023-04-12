/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#ifndef DS_HASH_SET_ITERATOR_HPP
#define DS_HASH_SET_ITERATOR_HPP

#include "./types.hpp"

namespace ds {

template <typename HashSet> class hash_set_iterator {
public:
  using key_type = typename HashSet::key_type;
  using key_ptr = key_type*;
  using key_ref = key_type&;
  using key_cref = const key_type&;
  using key_cptr = const key_type*;

  using node_type = typename HashSet::node_type;
  using nodeptr = node_type*;

  using container = typename HashSet::bucket_container;
  using containerptr = container*;

private:
  containerptr base = nullptr;
  nodeptr ptr = nullptr;
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
    nodeptr node = nullptr;
    while (this->index < this->base->size()) {
      node = (*base)[this->index];
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
  explicit hash_set_iterator() noexcept = default;

  explicit hash_set_iterator(containerptr base) noexcept : base(base) {
    // Find the first element
    for (auto* node : *this->base) {
      if (node) {
        this->ptr = node;
        return;
      }

      ++this->index;
    }
  }

  explicit hash_set_iterator(containerptr base, key_ptr ptr) noexcept
      : base(base), ptr(ptr) {}

  key_cref operator*() const noexcept {
    return this->ptr->key;
  }

  key_cptr operator->() const noexcept {
    return &this->ptr->key;
  }

  hash_set_iterator& operator++() noexcept {
    this->find_next_element();
    return *this;
  }

  const hash_set_iterator operator++(i32) noexcept {
    hash_set_iterator tmp = *this;
    this->find_next_element();
    return tmp;
  }

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

} // namespace ds

#endif

