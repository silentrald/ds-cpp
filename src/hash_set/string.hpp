/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-29
 * Updated: 2022-12-29
 *===============================*/

#ifndef DS_HASH_SET_STRING_HPP
#define DS_HASH_SET_STRING_HPP

#include "../hash/def.hpp"
#include "../hash_set/def.hpp"
#include "../string/def.hpp"
#include "ds/equal/def.hpp"

namespace ds {

template <typename Hash = hash<string>, typename KeyEqual = equal<string>>
class string_hash_set
    : public base_hash_set<
          string_hash_set<Hash, KeyEqual>, string, Hash, KeyEqual> {
public:
  using key_type = string;
  using key_ptr = string*;
  using key_ref = string&;
  using key_cref = const string&;
  using key_rref = string&&;

  using node_type = typename base_hash_set<
      string_hash_set<Hash, KeyEqual>, string, Hash, KeyEqual>::node_type;
  using node_ptr = node_type*;

  using iterator =
      hash_set_iterator<base_hash_set<string_hash_set<Hash>, string, Hash>>;
  using citerator = const iterator;

  using hash_type = typename Hash::hash_return;

  // === Modifiers ===

  /**
   * Inserts a key in the hash set
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] ds::opt_error insert(key_cref key) noexcept {
    return this->template insert_impl<key_cref>(key);
  }

  /**
   * Inserts a key in the hash set
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] ds::opt_error insert(key_rref key) noexcept {
    return this->template insert_impl<key_rref>(std::move(key));
  }

  /**
   * Inserts a key in the hash set
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] ds::opt_error insert(const char* key) noexcept {
    return this->template insert_impl<const char*>(key);
  }

  /**
   * Deletes a key in the hash set
   **/
  void erase(key_cref key) noexcept {
    this->template erase_impl<key_cref>(key);
  }

  /**
   * Deletes a key in the hash set
   **/
  void erase(const char* key) noexcept {
    this->template erase_impl<const char*>(key);
  }

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find(key_cref key) noexcept {
    return this->template find_impl<key_cref>(key);
  }

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return iterator
   **/
  [[nodiscard]] iterator find(const char* key) noexcept {
    return this->template find_impl<const char*>(key);
  }

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find(key_cref key) const noexcept {
    return this->template find_impl<key_cref>(key);
  }

  /**
   * Finds and returns the iterator pointing the element
   *
   * @return citerator
   **/
  [[nodiscard]] citerator find(const char* key) const noexcept {
    return this->template find_impl<const char*>(key);
  }

  /**
   * Checks if the key exists in the hash set
   *
   * @return bool
   **/
  [[nodiscard]] bool contains(key_cref key) noexcept {
    return this->template contains_impl<key_cref>(key);
  }

  /**
   * Checks if the key exists in the hash set
   *
   * @return bool
   **/
  [[nodiscard]] bool contains(const char* key) noexcept {
    return this->template contains_impl<const char*>(key);
  }
};

// === Specialization === //
template <typename Hash, typename KeyEqual>
class hash_set<string, Hash, KeyEqual>
    : public string_hash_set<Hash, KeyEqual> {};

template <typename Hash>
class hash_set<string, Hash> : public string_hash_set<Hash> {};

template <> class hash_set<string> : public string_hash_set<> {};

} // namespace ds

#endif

