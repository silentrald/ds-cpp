/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-26
 *===============================*/

#ifndef DS_HASH_SET_HPP
#define DS_HASH_SET_HPP

#include "./equal.hpp"
#include "./error.hpp"
#include "./hash.hpp"
#include "./hash_set_iterator.hpp"
#include "./macro.hpp"
#include "./prime.hpp"
#include "./types.hpp"
#include "./vector.hpp"
#include <new>

namespace ds {

const char* const HASH_SET_BAD_ALLOC = "hash_set:bad_alloc";

template <
    typename Derived, typename Key, typename Hash = hash<Key>,
    typename Equal = equal<Key>>
class base_hash_set {
public:
  friend Derived;

  using key_type = Key;
  using key_ptr = Key*;
  using key_ref = Key&;
  using key_cref = const Key&;
  using key_rref = Key&&;

  using iterator = hash_set_iterator<base_hash_set<Derived, Key, Hash, Equal>>;
  using citerator =
      hash_set_iterator<const base_hash_set<Derived, Key, Hash, Equal>>;

  using hash_type = typename Hash::hash_return;

  struct node {
    key_type key;
    node* next = nullptr;
  };

  using node_type = node;
  using node_ptr = node*;
  using bucket_container = vector<node_ptr>;

protected:
  bucket_container buckets{};
  i32 _size = 0;
  i32 _max_size = 0;
  f32 _max_load_factor = 1.0F;

  // === Hashing ===

  /**
   * Calculates the hash for the key, which already resizes the index within
   * the range of the bucket count
   *
   * @return hash_type
   **/
  template <typename Key_> hash_type calculate_hash_index(Key_ key) noexcept;

  // * Insert Helper *

  /**
   * Create the node pointer, returns null if a bad allocation error happened
   *
   * @errors
   *  - bad allocation in creating the node
   *  - bad allocation in copying key
   **/
  template <typename Key_>
  [[nodiscard]] node_ptr create_node(Key_ key) noexcept;

  /**
   * Inserts a value in the hash map, Key_ and Value_ should must support the
   * copy function
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key
   **/
  template <typename Key_> [[nodiscard]] opt_err insert_impl(Key_ key) noexcept;

  /**
   * Deletes a key in the hash set
   **/
  template <typename Key_> void erase_impl(Key_ key) noexcept;

  // * Lookup Helper *

  /**
   * Returns the node at with the key provided
   *
   * @return node_ptr
   **/
  template <typename Key_> [[nodiscard]] node_ptr get_node(Key_ key) noexcept;

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return iterator
   **/
  template <typename Key_> [[nodiscard]] iterator find_impl(Key_ key) noexcept;

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return citerator
   **/
  template <typename Key_>
  [[nodiscard]] citerator find_impl(Key_ key) const noexcept;

  /**
   * Checks if the key exists in the hash set
   *
   * @return bool
   **/
  template <typename Key_> [[nodiscard]] bool contains_impl(Key_ key) noexcept;

public:
  base_hash_set() noexcept = default;
  base_hash_set(const base_hash_set&) = delete;
  base_hash_set& operator=(const base_hash_set&) = delete;

  // === Copy ===

  /**
   * Copies the hash set
   *
   * @errors
   *  - bad allocation resizing the bucket
   *  - bad allocation in node creation
   **/
  [[nodiscard]] opt_err copy(const base_hash_set& other) noexcept;

  // === Move ===

  base_hash_set(base_hash_set&& rhs) noexcept;
  base_hash_set& operator=(base_hash_set&& rhs) noexcept;

  // === Destructor ===

  ~base_hash_set() noexcept;

  // === Iterators ===

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
   * Check if the hash set has no nodes
   *
   * @return bool
   **/
  [[nodiscard]] bool is_empty() const noexcept;

  /**
   * Returns the current size of the hash set
   *
   * @return i32
   **/
  [[nodiscard]] i32 size() const noexcept;

  // === Modifiers ===

  /**
   * Removes all the nodes from the hash set
   **/
  void clear() noexcept;

  /**
   * Inserts a key in the hash set
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] opt_err insert(key_cref key) noexcept {
    return this->insert_impl<key_cref>(key);
  }

  /**
   * Inserts a key in the hash set
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] opt_err insert(key_rref key) noexcept {
    return this->insert_impl<key_cref>(std::move(key));
  }

  /**
   * Deletes a key in the hash set
   **/
  void erase(key_cref key) noexcept {
    this->erase_impl<key_cref>(key);
  }

  // === Lookup ===

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find(key_cref key) noexcept {
    return this->find_impl<key_cref>(key);
  }

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find(key_cref key) const noexcept {
    return this->find_impl<key_cref>(key);
  }

  /**
   * Checks if the key exists in the hash set
   *
   * @return bool
   **/
  [[nodiscard]] bool contains(key_cref key) noexcept {
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
   * Grows and rehashes the hash set
   *
   * @errors
   *  - bad allocation in resizing the number of buckets
   **/
  [[nodiscard]] opt_err rehash(i32 count) noexcept;
};

} // namespace ds

#ifndef DS_HASH_SET_TPP
#include "./hash_set.tpp"
#endif

namespace ds {

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

