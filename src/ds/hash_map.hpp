/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#ifndef DS_HASH_MAP_HPP
#define DS_HASH_MAP_HPP

#include "./hash.hpp"
#include "./hash_map_iterator.hpp"
#include "ds/equal.hpp"
#include "ds/macro.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "ds/vector.hpp"
#include <utility>

namespace ds {

template <
    typename Derived, typename Key, typename Value, typename Hash = hash<Key>,
    typename KeyEqual = equal<Key>>
class base_hash_map {
public:
  friend Derived;

  using key_type = Key;
  using key_ptr = Key*;
  using key_ref = Key&;
  using key_cref = const Key&;
  using key_rref = Key&&;

  using value_type = Value;
  using value_ptr = Value*;
  using value_ref = Value&;
  using value_cref = const Value&;
  using value_rref = Value&&;

  using hash_type = typename Hash::hash_return;

  struct node {
    Key key;
    Value value;
    node* next = nullptr;
  };

  using node_type = node;
  using node_ptr = node*;
  using bucket_container = vector<node_ptr>;

  using iterator =
      hash_map_iterator<base_hash_map<Derived, Key, Value, Hash, KeyEqual>>;
  using citerator = hash_map_const_iterator<
      base_hash_map<Derived, Key, Value, Hash, KeyEqual>>;

protected:
  bucket_container buckets{};
  i32 _size = 0;
  i32 _max_size = 0;
  f32 _max_load_factor = 1.0F;

  // === Hashing ===

  /**
   * Calculates the hash for the value, which already resizes the index within
   * the range of the bucket count
   *
   * @return hash_type
   **/
  template <typename Key_>
  [[nodiscard]] hash_type calculate_hash_index(Key_ key) const noexcept;

  // * Insert Helper *

  /**
   * Create the node pointer, returns null if a bad allocation error happened
   *
   * @errors
   *  - bad allocation in creating the node
   *  - bad allocation in copying key or value
   **/
  template <typename Key_, typename Value_>
  [[nodiscard]] node_ptr create_node(Key_ key, Value_ value) noexcept;

  template <typename Key_, typename Value_>
  [[nodiscard]] opt_err insert_impl(Key_ key, Value_ value) noexcept;
  template <typename Key_> void erase_impl(Key_ key) noexcept;

  // * Lookup Helper *

  /**
   * Returns the node at with the key provided
   *
   * @return node_ptr
   **/
  template <typename Key_>
  [[nodiscard]] node_ptr get_node(Key_ key) const noexcept;

  template <typename Key_>
  [[nodiscard]] bool contains_impl(Key_ key) const noexcept;

public:
  base_hash_map() noexcept = default;
  base_hash_map(const base_hash_map&) = delete;
  base_hash_map& operator=(const base_hash_map&) = delete;

  // === Copy ===

  /**
   * Copies the hash map
   *
   * @errors
   *  - bad allocation resizing the bucket
   *  - bad allocation in node creation
   **/
  [[nodiscard]] opt_err copy(const base_hash_map& other) noexcept;

  // === Move ===
  base_hash_map(base_hash_map&& rhs) noexcept;
  base_hash_map& operator=(base_hash_map&& rhs) noexcept;

  // === Destructor ===
  ~base_hash_map() noexcept;

  // === Iterator ===

  /**
   * Returns an iterator pointing at the first element found
   *
   * @return iterator
   **/
  [[nodiscard]] iterator begin() noexcept;

  /**
   * Returns an iterator pointing at the first element found
   *
   * @return citerator
   **/
  [[nodiscard]] citerator cbegin() const noexcept;

  /**
   * Returns an iterator pointing to null
   *
   * @return iterator
   **/
  [[nodiscard]] iterator end() noexcept;

  /**
   * Returns an iterator pointing to null
   *
   * @return citerator
   **/
  [[nodiscard]] citerator cend() const noexcept;

  // === Capacity ===

  /**
   * Check if the hash node has no nodes
   *
   * @return bool
   **/
  [[nodiscard]] bool is_empty() const noexcept;

  /**
   * Returns the current size of the hash map
   *
   * @return i32
   **/
  [[nodiscard]] i32 size() const noexcept;

  // === Modifiers ===

  /**
   * Removes all the nodes from the hash map
   **/
  void clear() noexcept;

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key or value
   **/
  [[nodiscard]] opt_err insert(key_cref key, value_cref value) noexcept {
    return this->insert_impl<key_cref, value_cref>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key
   **/
  [[nodiscard]] opt_err insert(key_cref key, value_rref value) noexcept {
    return this->insert_impl<key_cref, value_rref>(key, std::move(value));
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying value
   **/
  [[nodiscard]] opt_err insert(key_rref key, value_cref value) noexcept {
    return this->insert_impl<key_rref, value_cref>(std::move(key), value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] opt_err insert(key_rref key, value_rref value) noexcept {
    return this->insert_impl<key_rref, value_rref>(
        std::move(key), std::move(value)
    );
  }

  /**
   * Deletes a key in the hash map
   **/
  void erase(key_cref key) noexcept {
    this->erase_impl<key_cref>(key);
  }

  // === Lookup ===

  /**
   * Safe lookup
   *
   *   If the key was found then return the value
   *   Else return an error
   *
   * @errors
   *   - key was not found
   **/
  [[nodiscard]] exp_ptr_err<value_type> at(key_cref key) noexcept {
    auto* node = this->get_node<key_cref>(key);
    if (node == nullptr) {
      return NOT_FOUND_EXP;
    }

    return &node->value;
  }

  /**
   * Unsafe lookup
   *
   *   If a value was found then return the value's address
   *   Else return nullptr
   *
   * @return value_ptr
   **/
  [[nodiscard]] value_ptr operator[](key_cref key) const noexcept {
    auto* node = this->get_node<key_cref>(key);
    return node ? &node->value : nullptr;
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(key_cref key) const noexcept {
    return this->contains_impl<key_cref>(key);
  }

  // === Bucket Interface ===

  /**
   * Get the number of buckets in the hash table
   *
   * @return i32
   **/
  [[nodiscard]] i32 bucket_count() const noexcept;

  /**
   * Grows and rehashes the hash map
   *
   * @errors
   *  - bad allocation in resizing the number of buckets
   **/
  [[nodiscard]] opt_err rehash(i32 count) noexcept;
};

} // namespace ds

#ifndef DS_HASH_MAP_TPP
#include "./hash_map.tpp"
#endif

namespace ds {

template <
    typename Key, typename Value, typename Hash = hash<Key>,
    typename KeyEqual = equal<Key>>
class hash_map
    : public base_hash_map<
          hash_map<Key, Value, Hash, KeyEqual>, Key, Value, Hash, KeyEqual> {};

} // namespace ds

#ifndef DS_HASH_MAP_STRING_HPP
#include "./hash_map_string.hpp"
#endif

#endif

