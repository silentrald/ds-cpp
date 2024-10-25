/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-10-08
 *===============================*/

#ifndef DS_HASH_SET_HPP
#define DS_HASH_SET_HPP

#include "./hash.hpp"
// #include "./hash_map_iterator.hpp"
#include "./equal.hpp"
#include "prime.hpp"
#include "types.hpp"
#include <cstdlib>
#include <type_traits>
#include <utility>

#ifdef DS_TEST
#include <cstdio>
#endif

namespace ds {

inline const f32 HASHSET_LOAD_FACTOR = 0.9F;
inline const usize HASHSET_MAX_SIZE = USIZE_MAX - 1U;
// Indicator if the node is empty or not
inline const usize HASHSET_EMPTY_VALUE = USIZE_MAX;

/**
 * Hash map / table implementation with robin hood hashing
 *   backward shift deletion, without Value
 *
 * References:
 *  https://programming.guide/robin-hood-hashing.html
 *  https://codecapsule.com/2013/11/17/robin-hood-hashing-backward-shift-deletion
 **/
template <
    typename Derived, typename Key, typename Hash = hash<Key>,
    typename KeyEqual = equal<Key>>
class base_hash_set {
public:
  friend Derived;

  using key_type = Key;

  struct node_type {
    Key key{};
    usize distance = HASHSET_EMPTY_VALUE;

    inline void to_empty() noexcept {
      this->distance = HASHSET_EMPTY_VALUE;
    }

    [[nodiscard]] inline bool is_node_empty() const noexcept {
      return this->distance == HASHSET_EMPTY_VALUE;
    }
  };

  // using iterator =
  //     hash_map_iterator<base_hash_set<Derived, Key, Value, Hash, KeyEqual>>;
  // using citerator = hash_map_const_iterator<
  //     base_hash_set<Derived, Key, Value, Hash, KeyEqual>>;

  base_hash_set() noexcept = default;
  base_hash_set(const base_hash_set&) = delete;
  base_hash_set& operator=(const base_hash_set&) = delete;

  // === Copy ===

  /**
   * Copies the hash map
   *
   * @errors
   *  - error_code::BAD_ALLOCATION - bucket resize
   **/
  [[nodiscard]] error_code copy(const base_hash_set& other) noexcept {
    return error_code::NOT_IMPLEMENTED;
  }

  // === Move ===

  base_hash_set(base_hash_set&& other) noexcept
      : bucket(other.bucket),
        size(other.size),
        max_size(other.max_size),
        capacity(other.capacity) {
    other.bucket = nullptr;
  }

  base_hash_set& operator=(base_hash_set&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->bucket = rhs.bucket;
    this->size = rhs.size;
    this->max_size = rhs.max_size;
    this->capacity = rhs.capacity;
    rhs.bucket = nullptr;

    return *this;
  }

  // === Destructor === //

  ~base_hash_set() noexcept {
    this->destroy();
  }

  /**
   * Removes all elements
   **/
  void destroy() noexcept {
    if (this->bucket == nullptr) {
      return;
    }

    if constexpr (std::is_class<Key>::value) {
      for (usize i = 0; i < this->capacity; ++i) {
        this->bucket[i].~node_type();
      }
    }

    std::free(this->bucket); // NOLINT
    this->bucket = nullptr;
    this->size = this->max_size = this->capacity = 0U;
  }

  /**
   * Removes all elements without destroying each nodes.
   **/
  void clear() noexcept {
    if (this->bucket == nullptr) {
      return;
    }

    for (usize i = 0; i < this->capacity; ++i) {
      this->bucket[i].to_empty();
    }
    this->size = 0U;
  }

  // === Iterator === //

  // /**
  //  * Returns an iterator pointing at the first element found
  //  *
  //  * @return iterator
  //  **/
  // [[nodiscard]] iterator begin() noexcept;
  //
  // /**
  //  * Returns an iterator pointing at the first element found
  //  *
  //  * @return citerator
  //  **/
  // [[nodiscard]] citerator cbegin() const noexcept;
  //
  // /**
  //  * Returns an iterator pointing to null
  //  *
  //  * @return iterator
  //  **/
  // [[nodiscard]] iterator end() noexcept;
  //
  // /**
  //  * Returns an iterator pointing to null
  //  *
  //  * @return citerator
  //  **/
  // [[nodiscard]] citerator cend() const noexcept;

  // === Capacity ===

  /**
   * Check if the hash node has no nodes
   **/
  [[nodiscard]] bool is_empty() const noexcept {
    return this->size == 0;
  }

  /**
   * Returns the current size of the hash map
   **/
  [[nodiscard]] usize get_size() const noexcept {
    return this->size;
  }

  // === Modifiers ===

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error_code::BAD_ALLOCATION - bucket resize or key copy
   *  - error_code::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key
   **/
  [[nodiscard]] error_code insert(const Key& key) noexcept {
    return this->insert_impl(key);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error_code::BAD_ALLOCATION - bucket resize or value copy
   *  - error_code::CONTAINER_FULL - max limit of hash_map
   **/
  [[nodiscard]] error_code insert(Key&& key) noexcept {
    return this->insert_impl(std::move(key));
  }

  /**
   * Removes a key/value pair in the hash map
   * If no key was found, nothing will happen to the hash map
   **/
  void remove(const Key& key) noexcept {
    this->erase_impl<const Key&>(key);
  }

  // === Accessors === //

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const Key& key) const noexcept {
    return this->find(key);
  }

#ifdef DS_TEST
  void print() const noexcept {
    printf("=== hash_map ===\n");
    if (this->bucket == nullptr) {
      printf("No elements\n");
    }
    for (usize i = 0; i < this->capacity; ++i) {
      printf(
          USIZE_FORMAT ": (%16llx, %16llx) distance: %16llx\n", i,
          this->bucket[i].key, this->bucket[i].value, this->bucket[i].distance
      );
    }
  }
#endif

protected:
  node_type* bucket = nullptr;
  usize size = 0U;
  usize max_size = 0U;
  usize capacity = 0U;

  // === Helpers === //

  /**
   * Calculates the hash for the value, which already resizes the index within
   * the range of the bucket count
   **/
  template <typename Key_>
  [[nodiscard]] inline usize calculate_hash_index(Key_ key) const noexcept {
    return Hash{}(key) % this->capacity;
  }

  inline void insert_node(node_type&& node) noexcept {
    // NOTE: No infinite loop since 1 node will always be empty in any case
    for (usize index = this->calculate_hash_index<const Key&>(node.key);;) {
      // Place the node on an empty bucket slot
      if (this->bucket[index].is_node_empty()) {
        this->bucket[index] = std::move(node);
        ++this->size;
        break;
      }

      // If the key already exists, overwrite the value
      if (KeyEqual()(node.key, this->bucket[index].key)) {
        break;
      }

      // Swapping between the `rich` and `poor` nodes
      if (node.distance > this->bucket[index].distance) {
        std::swap(node, this->bucket[index]);
      }
      ++node.distance;

      if (++index >= this->capacity) {
        index = 0;
      }
    }
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error_code::BAD_ALLOCATION - bucket resize or key/value copy
   *  - error_code::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] inline error_code insert_impl(const Key& key) noexcept {
    TRY(this->check_allocation());

    node_type node{.distance = 0U};
    if constexpr (!std::is_class<Key>::value) {
      node.key = key;
    } else {
      TRY(node.key.copy(key));
    }
    this->insert_node(std::move(node));

    return error_code::OK;
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error_code::BAD_ALLOCATION - bucket resize or value copy
   *  - error_code::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for value
   **/
  [[nodiscard]] inline error_code insert_impl(Key&& key) noexcept {
    TRY(this->check_allocation());
    this->insert_node(node_type{.key = std::move(key), .distance = 0U});
    return error_code::OK;
  }

  template <typename Key_> inline void erase_impl(Key_ key) noexcept {
    usize index = this->calculate_hash_index<Key_>(key);

    // NOTE: No infinite loop since 1 node will always be empty in any case
    while (true) {
      if (this->bucket[index].is_node_empty()) {
        return;
      }

      if (KeyEqual()(key, this->bucket[index].key)) {
        --this->size;
        break;
      }

      if (++index >= this->capacity) {
        index = 0;
      }
    }

    // Backward shift delete algorithm

    // index -> capacity
    for (++index; index < this->capacity; ++index) {
      if (this->bucket[index].is_node_empty() ||
          this->bucket[index].distance == 0U) {
        this->bucket[index - 1U].to_empty();
        return;
      }
      --this->bucket[index].distance;
      this->bucket[index - 1U] = std::move(this->bucket[index]);
    }

    // capacity -> 0
    if (this->bucket[0].is_node_empty() || this->bucket[0].distance == 0U) {
      this->bucket[this->capacity - 1U].to_empty();
      return;
    }
    --this->bucket[0].distance;
    this->bucket[this->capacity - 1U] = std::move(this->bucket[0]);

    // 0 -> original hash
    // NOTE: No infinite loop since 1 node will always be empty in any case
    for (index = 1U;; ++index) {
      if (this->bucket[index].is_node_empty() ||
          this->bucket[index].distance == 0U) {
        this->bucket[index - 1U].to_empty();
        return;
      }
      --this->bucket[index].distance;
      this->bucket[index - 1U] = std::move(this->bucket[index]);
    }
  }

  [[nodiscard]] bool find(const Key& key) const noexcept {
    // NOTE: No infinite loop since 1 node will always be empty in any case
    for (usize index = this->calculate_hash_index<const Key&>(key);;) {
      if (this->bucket[index].is_node_empty()) {
        return false;
      }

      if (KeyEqual()(key, this->bucket[index].key)) {
        return true;
      }

      if (++index >= this->capacity) {
        index = 0U;
      }
    }
  }

  // === Memory === //

  /**
   * Check if the allocation can handle any mutation done to the hash map
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code check_allocation() noexcept {
    if (this->bucket == nullptr) {
      return this->allocate(get_first_prime());
    }

    if (this->size >= HASHSET_MAX_SIZE) {
      return error_code::CONTAINER_FULL;
    }

    if (this->size >= this->max_size) {
      return this->reallocate(this->capacity * 2U + 1U);
    }

    return error_code::OK;
  }

  /**
   *
   **/
  [[nodiscard]] error_code allocate(usize new_capacity) noexcept {
    // NOLINTNEXTLINE
    this->bucket = (node_type*)std::malloc(new_capacity * sizeof(node_type));
    if (this->bucket == nullptr) {
      return error_code::BAD_ALLOCATION;
    }

    new (this->bucket) node_type[new_capacity];

    this->capacity = new_capacity;
    this->max_size = new_capacity * HASHSET_LOAD_FACTOR;

    return error_code::OK;
  }

  [[nodiscard]] error_code reallocate(usize new_capacity) noexcept {
    // NOLINTNEXTLINE
    auto* new_bucket =
        // NOLINTNEXTLINE
        (node_type*)std::malloc(new_capacity * sizeof(node_type));
    if (new_bucket == nullptr) {
      return error_code::BAD_ALLOCATION;
    }

    new (new_bucket) node_type[new_capacity];

    // Swap the pointers and capacity to easily use the insert function
    std::swap(this->bucket, new_bucket);
    std::swap(this->capacity, new_capacity);
    this->size = 0;
    this->max_size = this->capacity * HASHSET_LOAD_FACTOR;

    // Transfer the old bucket to the new bucket
    for (usize i = 0; i < new_capacity; ++i) {
      if (new_bucket[i].is_node_empty()) {
        continue;
      }

      new_bucket[i].distance = 0U;
      this->insert_node(std::move(new_bucket[i]));
    }

    std::free(new_bucket); // NOLINT
    return error_code::OK;
  }
};

// === hash_set definition === //

template <
    typename Key, typename Hash = hash<Key>, typename KeyEqual = equal<Key>>
class hash_set
    : public base_hash_set<hash_set<Key, Hash, KeyEqual>, Key, Hash, KeyEqual> {
};

} // namespace ds

#ifndef DS_HASH_SET_STRING_HPP
#include "./hash_set_string.hpp"
#endif

#endif
