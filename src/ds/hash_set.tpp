/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-26
 *===============================*/

#ifndef DS_HASH_SET_TPP
#define DS_HASH_SET_TPP

#include "../prime.hpp"
#include "./hash_set.hpp"
#include "ds/macro.hpp"
#include "ds/types.hpp"
#include <new>
#include <type_traits>
#include <utility>

// Implementation
namespace ds {

// === Copy ===
template <typename Derived, typename Key, typename Hash, typename Equal>
opt_err
base_hash_set<Derived, Key, Hash, Equal>::copy(const base_hash_set& other
) noexcept {
  if (&other == this) {
    return null;
  }

  this->clear();

  // Copy the bucket
  if (this->buckets.size() != other.buckets.size()) {
    try_opt(this->buckets.resize(other.buckets.size()));
  }

  node_ptr node = nullptr;
  for (i32 i = 0; i < this->buckets.size(); i++) {
    for (auto* other_node = other.buckets[i]; other_node;
         other_node = other_node->next) {
      // Copy the node
      if constexpr (std::is_class<key_type>::value) {
        node = this->create_node<key_cref>(other_node->key);
      } else {
        node = this->create_node<key_type>(other_node->key);
      }

      // If new fails, destroy the current hash_set
      if (node == nullptr) {
        this->clear();
        return BAD_ALLOC_OPT;
      }

      this->buckets[i] = node;
    }
  }

  // Copy the other variables
  this->_size = other._size;
  this->_max_size = other._max_size;
  this->_max_load_factor = other._max_load_factor;

  // Hash function should be the same

  return null;
}

// === Move ===
template <typename Derived, typename Key, typename Hash, typename Equal>
base_hash_set<Derived, Key, Hash, Equal>::base_hash_set(base_hash_set&& rhs
) noexcept
    : buckets(std::move(rhs.buckets)),
      _size(rhs._size),
      _max_size(rhs._max_size),
      _max_load_factor(rhs._max_load_factor) {
  rhs._size = rhs._max_size = 0;
  rhs._max_load_factor = 1.0F;
}

template <typename Derived, typename Key, typename Hash, typename Equal>
base_hash_set<Derived, Key, Hash, Equal>&
base_hash_set<Derived, Key, Hash, Equal>::operator=(base_hash_set&& rhs
) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->clear();

  this->buckets = std::move(rhs.buckets);
  this->_size = rhs._size;
  this->_max_size = rhs._max_size;
  this->_max_load_factor = rhs._max_load_factor;

  rhs._size = rhs._max_size = 0;
  rhs._max_load_factor = 1.0F;

  return *this;
}

// === Destructor ===
template <typename Derived, typename Key, typename Hash, typename Equal>
base_hash_set<Derived, Key, Hash, Equal>::~base_hash_set() noexcept {
  for (auto* tmp : this->buckets) {
    for (auto* node = tmp; node; node = tmp) {
      tmp = node->next;
      delete node; // NOLINT
    }
  }
}

// === Hashing ===
template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
typename base_hash_set<Derived, Key, Hash, Equal>::hash_type
base_hash_set<Derived, Key, Hash, Equal>::calculate_hash_index(Key_ key
) noexcept {
  hash_type index = Hash{}(key);
  index %= this->buckets.size();
  if (index < 0)
    index += this->buckets.size();

  return index;
}

// === Iterators ===
template <typename Derived, typename Key, typename Hash, typename Equal>
typename base_hash_set<Derived, Key, Hash, Equal>::iterator
base_hash_set<Derived, Key, Hash, Equal>::begin() noexcept {
  return iterator{&this->buckets};
}

template <typename Derived, typename Key, typename Hash, typename Equal>
typename base_hash_set<Derived, Key, Hash, Equal>::citerator
base_hash_set<Derived, Key, Hash, Equal>::cbegin() const noexcept {
  return citerator{&this->buckets};
}

template <typename Derived, typename Key, typename Hash, typename Equal>
typename base_hash_set<Derived, Key, Hash, Equal>::iterator
base_hash_set<Derived, Key, Hash, Equal>::end() noexcept {
  return iterator{};
}

template <typename Derived, typename Key, typename Hash, typename Equal>
typename base_hash_set<Derived, Key, Hash, Equal>::citerator
base_hash_set<Derived, Key, Hash, Equal>::cend() const noexcept {
  return citerator{};
}

// === Capacity ===
template <typename Derived, typename Key, typename Hash, typename Equal>
bool base_hash_set<Derived, Key, Hash, Equal>::is_empty() const noexcept {
  return this->_size == 0;
}

template <typename Derived, typename Key, typename Hash, typename Equal>
i32 base_hash_set<Derived, Key, Hash, Equal>::size() const noexcept {
  return this->_size;
}

// === Modifiers ===
template <typename Derived, typename Key, typename Hash, typename Equal>
void base_hash_set<Derived, Key, Hash, Equal>::clear() noexcept {
  node_ptr tmp = nullptr;

  for (auto*& head : this->buckets) {
    // Loop through all the nodes in the linked list
    for (auto* node = head; node; node = tmp) {
      tmp = node->next;
      delete node; // NOLINT
    }

    head = nullptr;
  }

  this->_size = 0;
}

template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
typename base_hash_set<Derived, Key, Hash, Equal>::node_ptr
base_hash_set<Derived, Key, Hash, Equal>::create_node(Key_ key) noexcept {
  node_ptr node = new (std::nothrow) node_type{.next = nullptr}; // NOLINT
  if (node == nullptr) {
    return nullptr;
  }

  if constexpr (std::is_rvalue_reference<key_type>::value) {
    node->key = std::move(key);
  } else if constexpr (std::is_copy_assignable<key_type>::value) {
    node->key = key;
  } else {
    if (node->key.copy(key)) {
      delete node; // NOLINT
      return nullptr;
    }
  }

  return node;
}

template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
opt_err base_hash_set<Derived, Key, Hash, Equal>::insert_impl(Key_ key
) noexcept {
  if (this->_size + 1 > this->_max_size) {
    try_opt(this->rehash(next_greater_prime_i32(this->_size)));
  }

  hash_type index = this->calculate_hash_index(key);

  // Try to insert to the bucket
  auto* bucket = this->buckets[index];
  if (bucket == nullptr) { // Create the node
    auto* node = this->create_node<Key_>(std::forward<Key_>(key));
    if (node == nullptr) {
      return BAD_ALLOC_OPT;
    }

    this->buckets[index] = node;
    ++this->_size;
    return null;
  }

  // Try to check for colission
  while (bucket->next) {
    // Value already exists
    if (Equal{}(bucket->key, key)) {
      return null;
    }

    bucket = bucket->next;
  }

  if (Equal{}(bucket->key, key)) {
    return null;
  }

  // Insert the node to the end of the bucket
  auto* node = this->create_node<Key_>(std::forward<Key_>(key));
  if (node == nullptr) {
    return BAD_ALLOC_OPT;
  }

  bucket->next = node;
  ++this->_size;
  return null;
}

template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
void base_hash_set<Derived, Key, Hash, Equal>::erase_impl(Key_ key) noexcept {
  hash_type index = this->calculate_hash_index<Key_>(std::forward<Key_>(key));

  // Try to search for the node
  auto* head = this->buckets[index];

  // If bucket is empty, no deletion
  if (head == nullptr) {
    return;
  }

  // Check if the head node is the key
  if (Equal{}(head->key, key)) {
    // Move the key_ptr to the next element and delete the current node
    this->buckets[index] = head->next;
    --this->_size;
    delete head; // NOLINT
    return;
  }

  for (auto* curr = head->next; curr; head = curr, curr = curr->next) {
    /**
     * Point the head to the next of current and delete the current node
     *
     * Visualization:
     *  X -> H -> C -> X -> nullptr
     *   result: X -> H -> X -> nullptr
     *  X -> X -> H -> C -> nullptr
     *   result: X -> X -> H -> nullptr
     **/
    if (Equal{}(curr->key, key)) {
      head->next = curr->next;
      --this->_size;
      delete curr; // NOLINT
      return;
    }
  }
}

// === Lookup ===
template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
typename base_hash_set<Derived, Key, Hash, Equal>::node_ptr
base_hash_set<Derived, Key, Hash, Equal>::get_node(Key_ key) noexcept {
  for (auto* node =
           this->buckets[this->calculate_hash_index<Key_>(std::forward<Key_>(key
           ))];
       node; node = node->next) {
    if (Equal{}(node->key, key)) {
      return node;
    }
  }

  return nullptr;
}

template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
typename base_hash_set<Derived, Key, Hash, Equal>::iterator
base_hash_set<Derived, Key, Hash, Equal>::find_impl(Key_ key) noexcept {
  auto* node = this->get_node<Key_>(key);
  return node ? iterator{&this->buckets, node} : iterator{};
}

template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
typename base_hash_set<Derived, Key, Hash, Equal>::citerator
base_hash_set<Derived, Key, Hash, Equal>::find_impl(Key_ key) const noexcept {
  auto* node = this->get_node<Key_>(key);
  return node ? iterator{&this->buckets, node} : iterator{};
}

template <typename Derived, typename Key, typename Hash, typename Equal>
template <typename Key_>
bool base_hash_set<Derived, Key, Hash, Equal>::contains_impl(Key_ key
) noexcept {
  return this->get_node<Key_>(key) != nullptr;
}

// === Bucket Interface ===
template <typename Derived, typename Key, typename Hash, typename Equal>
i32 base_hash_set<Derived, Key, Hash, Equal>::bucket_count() const noexcept {
  return this->buckets.size();
}

template <typename Derived, typename Key, typename Hash, typename Equal>
opt_err base_hash_set<Derived, Key, Hash, Equal>::rehash(i32 count) noexcept {
  if (count < 1) {
    count = 11;
  }

  bucket_container old_buckets{};
  try_opt(old_buckets.resize(count));

  // Swap the buckets so calculate_hash_index would work
  std::swap(this->buckets, old_buckets);

  // Move the elements from the old to the new bucket
  hash_type index = 0;
  for (auto* tmp : old_buckets) {
    // Loop to all the linked list node
    for (auto* node = tmp; node; node = tmp) {
      tmp = node->next;
      index = this->calculate_hash_index<key_cref>(node->key);

      // Insert the node at the start of the linked list
      node->next = this->buckets[index];
      this->buckets[index] = node;
    }
  }

  this->_max_size = count * this->_max_load_factor;
  return null;
}

} // namespace ds

#endif

