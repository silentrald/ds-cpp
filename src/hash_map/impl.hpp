/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 * Updated: 2022-12-29
 *===============================*/

#ifndef DS_HASH_MAP_IMPL_HPP
#define DS_HASH_MAP_IMPL_HPP

#include "../prime.hpp"
#include "./def.hpp"
#include <new>

namespace ds {

// === Copy ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
opt_error base_hash_map<Derived, Key, Value, Hash, KeyEqual>::copy(
    const base_hash_map& other
) noexcept {
  if (&other == this) {
    return null;
  }

  return error{"not_implemented", def_err_vals};
}

// === Move ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::base_hash_map(
    base_hash_map&& rhs
) noexcept
    : buckets(std::move(rhs.buckets)),
      _size(rhs._size),
      _max_size(rhs._max_size),
      _max_load_factor(rhs._max_load_factor) {
  rhs._size = rhs._max_size = 0;
  rhs._max_load_factor = 1.0F;
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
base_hash_map<Derived, Key, Value, Hash, KeyEqual>&
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::operator=(
    base_hash_map&& rhs
) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->buckets = std::move(rhs.buckets);
  this->_size = rhs._size;
  this->_max_size = rhs._max_size;
  this->_max_load_factor = rhs._max_load_factor;

  return *this;
}

// === Destructor ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::~base_hash_map() noexcept {
  for (auto* tmp : this->buckets) {
    for (auto* node = tmp; node; node = tmp) {
      tmp = node->next;
      delete node; // NOLINT
    }
  }
}

// === Hashing ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
template <typename Key_>
typename base_hash_map<Derived, Key, Value, Hash, KeyEqual>::hash_type
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::calculate_hash_index(
    Key_ key
) const noexcept {
  hash_type index = Hash{}(key);
  index %= this->buckets.size();
  if (index < 0)
    index += this->buckets.size();

  return index;
}

// === Iterators ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
typename base_hash_map<Derived, Key, Value, Hash, KeyEqual>::iterator
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::begin() noexcept {
  return iterator{&this->buckets};
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
typename base_hash_map<Derived, Key, Value, Hash, KeyEqual>::citerator
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::cbegin() const noexcept {
  return iterator{&this->buckets};
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
typename base_hash_map<Derived, Key, Value, Hash, KeyEqual>::iterator
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::end() noexcept {
  return iterator{};
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
typename base_hash_map<Derived, Key, Value, Hash, KeyEqual>::citerator
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::cend() const noexcept {
  return iterator{};
}

// === Capacity ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
bool base_hash_map<Derived, Key, Value, Hash, KeyEqual>::empty(
) const noexcept {
  return this->_size == 0;
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
i32 base_hash_map<Derived, Key, Value, Hash, KeyEqual>::size() const noexcept {
  return this->_size;
}

// === Modifiers ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
void base_hash_map<Derived, Key, Value, Hash, KeyEqual>::clear() noexcept {
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

// * Inserts *
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
template <typename Key_, typename Value_>
typename base_hash_map<Derived, Key, Value, Hash, KeyEqual>::node_ptr
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::create_node(
    Key_ key, Value_ value
) noexcept {
  node_ptr ptr = new (std::nothrow) node_type{.next = nullptr}; // NOLINT
  if (ptr == nullptr) {
    return nullptr;
  }

  if constexpr (std::is_rvalue_reference<key_type>::value) {
    ptr->key = std::move(key);
  } else if constexpr (std::is_class<key_type>::value) {
    if (ptr->key.copy(key)) {
      delete ptr; // NOLINT
      return nullptr;
    }
  } else {
    // Copyable without exceptions
    ptr->key = key;
  }

  if constexpr (std::is_rvalue_reference<value_type>::value) {
    ptr->value = std::move(value);
  } else if constexpr (std::is_class<value_type>::value) {
    if (ptr->value.copy(value)) {
      delete ptr; // NOLINT
      return nullptr;
    }
  } else {
    // Copyable without exceptions
    ptr->value = value;
  }

  return ptr;
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
template <typename Key_, typename Value_>
opt_error base_hash_map<Derived, Key, Value, Hash, KeyEqual>::insert_impl(
    Key_ key, Value_ value
) noexcept {
  if (this->_size + 1 > this->_max_size) {
    try_opt(this->rehash(next_greater_prime_i32(this->_size)));
  }

  hash_type index = this->calculate_hash_index<Key_>(std::forward<Key_>(key));

  // Try to insert to the bucket
  auto* bucket = this->buckets[index];
  if (bucket == nullptr) {
    auto* node = this->create_node<Key_, Value_>(
        std::forward<Key_>(key), std::forward<Value_>(value)
    );
    if (node == nullptr) {
      return error{HASH_MAP_BAD_ALLOC, def_err_vals};
    }

    this->buckets[index] = node;
    ++this->_size;
    return null;
  }

  // Try to check for colission
  while (bucket->next) {
    if (!KeyEqual{}(bucket->key, key)) {
      bucket = bucket->next;
      continue;
    }

    // Overwrites the value at the current node
    if constexpr (std::is_class<value_type>::value) {
      try_opt(bucket->value.copy(value));
    } else {
      bucket->value = value;
    }

    return null;
  }

  // Overwrites the value at the last node
  if (KeyEqual{}(bucket->key, key)) {
    if constexpr (std::is_class<value_type>::value) {
      try_opt(bucket->value.copy(value));
    } else {
      bucket->value = value;
    }

    return null;
  }

  auto* node = this->create_node<Key_, Value_>(
      std::forward<Key_>(key), std::forward<Value_>(value)
  );
  if (node == nullptr) {
    return error{HASH_MAP_BAD_ALLOC, def_err_vals};
  }

  bucket->next = node;
  ++this->_size;
  return null;
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
template <typename Key_>
void base_hash_map<Derived, Key, Value, Hash, KeyEqual>::erase_impl(Key_ key
) noexcept {
  hash_type index = this->calculate_hash_index<Key_>(key);
  auto* head = this->buckets[index];

  // If bucket is empty, no deletion
  if (head == nullptr) {
    return;
  }

  // Check if the head node is the equivalent
  if (KeyEqual{}(head->key, key)) {
    // Move the ptr to the next element and delete the current node
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
    if (KeyEqual{}(curr->key, key)) {
      head->next = curr->next;
      --this->_size;
      delete curr; // NOLINT
      return;
    }
  }
}

// === Lookup ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
template <typename Key_>
typename base_hash_map<Derived, Key, Value, Hash, KeyEqual>::node_ptr
base_hash_map<Derived, Key, Value, Hash, KeyEqual>::get_node(Key_ key
) const noexcept {
  for (auto* node =
           this->buckets[this->calculate_hash_index<Key_>(std::forward<Key_>(key
           ))];
       node; node = node->next) {
    if (KeyEqual{}(node->key, key)) {
      return node;
    }
  }

  return nullptr;
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
template <typename Key_>
bool base_hash_map<Derived, Key, Value, Hash, KeyEqual>::contains_impl(Key_ key
) const noexcept {
  return this->get_node<Key_>(key) != nullptr;
}

// === Bucket Interface ===
template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
i32 base_hash_map<Derived, Key, Value, Hash, KeyEqual>::bucket_count(
) const noexcept {
  return this->buckets.size();
}

template <
    typename Derived, typename Key, typename Value, typename Hash,
    typename KeyEqual>
opt_error base_hash_map<Derived, Key, Value, Hash, KeyEqual>::rehash(i32 count
) noexcept {
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

