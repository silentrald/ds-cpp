/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-29
 *===============================*/

#ifndef DS_HASH_MAP_STRING_HPP
#define DS_HASH_MAP_STRING_HPP

#include "./hash_map.hpp"
#include "ds/types.hpp"

namespace ds {

// Reimplementation
// === Key string ; Value string ===
template <typename Hash = hash<string>, typename KeyEqual = equal<string>>
class string_both_hash_map : public base_hash_map<
                                 string_both_hash_map<Hash, KeyEqual>, string,
                                 string, Hash, KeyEqual> {
public:
  using key_type = string;
  using key_ptr = string*;
  using key_ref = string&;
  using key_cref = const string&;
  using key_rref = string&&;

  using value_type = string;
  using value_ptr = string*;
  using value_ref = string&;
  using value_cref = const string&;
  using value_rref = string&&;

  using node_type = typename base_hash_map<
      string_both_hash_map<Hash, KeyEqual>, string, string, Hash,
      KeyEqual>::node_type;
  using node_ptr = node_type*;

  using hash_type = typename Hash::hash_return;

  // === Modifiers ===

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key or value
   **/
  [[nodiscard]] opt_err insert(key_cref key, value_cref value) noexcept {
    return this->template insert_impl<key_cref, value_cref>(key, value);
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
    return this->template insert_impl<key_cref, value_rref>(
        key, std::move(value)
    );
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
    return this->template insert_impl<key_rref, value_cref>(
        std::move(key), value
    );
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] opt_err insert(key_rref key, value_rref value) noexcept {
    return this->template insert_impl<key_rref, value_rref>(
        std::move(key), std::move(value)
    );
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key and value
   **/
  [[nodiscard]] opt_err insert(const char* key, const char* value) noexcept {
    return this->template insert_impl<const char*, const char*>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key and value
   **/
  [[nodiscard]] opt_err insert(const char* key, value_cref value) noexcept {
    return this->template insert_impl<const char*, value_cref>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key and value
   **/
  [[nodiscard]] opt_err insert(const char* key, value_rref value) noexcept {
    return this->template insert_impl<const char*, value_rref>(
        key, std::move(value)
    );
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key and value
   **/
  [[nodiscard]] opt_err insert(key_cref key, const char* value) noexcept {
    return this->template insert_impl<key_cref, const char*>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying value
   **/
  [[nodiscard]] opt_err insert(key_rref key, const char* value) noexcept {
    return this->template insert_impl<key_rref, const char*>(
        std::move(key), value
    );
  }

  /**
   * Deletes a key in the hash map
   **/
  void erase(key_cref key) noexcept {
    this->template erase_impl<key_cref>(key);
  }

  /**
   * Deletes a key in the hash map
   **/
  void erase(const char* key) noexcept {
    this->template erase_impl<const char*>(key);
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
    auto* node = this->template get_node(key);
    if (node == nullptr) {
      return NOT_FOUND_EXP;
    }

    return &node->value;
  }

  /**
   * Safe lookup
   *
   *   If the key was found then return the value
   *   Else return an error
   *
   * @errors
   *   - key was not found
   **/
  [[nodiscard]] exp_ptr_err<value_type> at(const char* key) noexcept {
    auto* node = this->get_node(key);
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
   **/
  [[nodiscard]] value_ptr operator[](key_cref key) const noexcept {
    auto* node = this->template get_node<key_cref>(key);
    return node ? &node->value : nullptr;
  }

  /**
   * Unsafe lookup
   *
   *   If a value was found then return the value's address
   *   Else return nullptr
   **/
  [[nodiscard]] value_ptr operator[](const char* key) const noexcept {
    auto* node = this->get_node(key);
    return node ? &node->value : nullptr;
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(key_cref key) const noexcept {
    return this->template contains_impl<key_cref>(key);
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const char* key) const noexcept {
    return this->template contains_impl<const char*>(key);
  }
};

// === Key string ; Value non-string ===
template <
    typename Value, typename Hash = hash<string>,
    typename KeyEqual = equal<string>>
class string_key_hash_map : public base_hash_map<
                                string_key_hash_map<Value, Hash, KeyEqual>,
                                string, Value, Hash, KeyEqual> {
public:
  using key_type = string;
  using key_ptr = string*;
  using key_ref = string&;
  using key_cref = const string&;
  using key_rref = string&&;

  using value_type = Value;
  using value_ptr = Value*;
  using value_ref = Value&;
  using value_cref = const Value&;
  using value_rref = Value&&;

  using node_type = typename base_hash_map<
      string_key_hash_map<Value, Hash, KeyEqual>, string, Value, Hash,
      KeyEqual>::node_type;
  using node_ptr = node_type*;

  using hash_type = typename Hash::hash_return;

  // === Modifiers ===

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key or value
   **/
  [[nodiscard]] opt_err insert(key_cref key, value_cref value) noexcept {
    return this->template insert_impl<key_cref, value_cref>(key, value);
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
    return this->template insert_impl<key_cref, value_rref>(
        key, std::move(value)
    );
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
    return this->template insert_impl<key_rref, value_cref>(
        std::move(key), value
    );
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] opt_err insert(key_rref key, value_rref value) noexcept {
    return this->template insert_impl<key_rref, value_rref>(
        std::move(key), std::move(value)
    );
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key and value
   **/
  [[nodiscard]] opt_err insert(const char* key, value_cref value) noexcept {
    return this->template insert_impl<const char*, value_cref>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key and value
   **/
  [[nodiscard]] opt_err insert(const char* key, value_rref value) noexcept {
    return this->template insert_impl<const char*, value_rref>(
        key, std::move(value)
    );
  }

  /**
   * Deletes a key in the hash map
   **/
  void erase(key_cref key) noexcept {
    this->template erase_impl<key_cref>(key);
  }

  /**
   * Deletes a key in the hash map
   **/
  void erase(const char* key) noexcept {
    this->template erase_impl<const char*>(key);
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
    auto* node = this->template get_node(key);
    if (node == nullptr) {
      return NOT_FOUND_EXP;
    }

    return &node->value;
  }

  /**
   * Safe lookup
   *
   *   If the key was found then return the value
   *   Else return an error
   *
   * @errors
   *   - key was not found
   **/
  [[nodiscard]] exp_ptr_err<value_type> at(const char* key) noexcept {
    auto* node = this->get_node(key);
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
   **/
  [[nodiscard]] value_ptr operator[](key_cref key) const noexcept {
    auto* node = this->template get_node<key_cref>(key);
    return node ? &node->value : nullptr;
  }

  /**
   * Unsafe lookup
   *
   *   If a value was found then return the value's address
   *   Else return nullptr
   **/
  [[nodiscard]] value_ptr operator[](const char* key) const noexcept {
    auto* node = this->get_node(key);
    return node ? &node->value : nullptr;
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(key_cref key) const noexcept {
    return this->template contains_impl<key_cref>(key);
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const char* key) const noexcept {
    return this->template contains_impl<const char*>(key);
  }
};

// === Key non-string ; Value string ===
template <
    typename Key, typename Hash = hash<string>,
    typename KeyEqual = equal<string>>
class string_value_hash_map : public base_hash_map<
                                  string_value_hash_map<Key, Hash, KeyEqual>,
                                  Key, string, Hash, KeyEqual> {
public:
  using key_type = Key;
  using key_ptr = Key*;
  using key_ref = Key&;
  using key_cref = const Key&;
  using key_rref = Key&&;

  using value_type = string;
  using value_ptr = string*;
  using value_ref = string&;
  using value_cref = const string&;
  using value_rref = string&&;

  using node_type = typename base_hash_map<
      string_value_hash_map<Key, Hash, KeyEqual>, Key, string, Hash,
      KeyEqual>::node_type;
  using node_ptr = node_type*;

  using hash_type = typename Hash::hash_return;

  // === Modifiers ===

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key or value
   **/
  [[nodiscard]] opt_err insert(key_cref key, value_cref value) noexcept {
    return this->template insert_impl<key_cref, value_cref>(key, value);
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
    return this->template insert_impl<key_cref, value_rref>(
        key, std::move(value)
    );
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
    return this->template insert_impl<key_rref, value_cref>(
        std::move(key), value
    );
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   **/
  [[nodiscard]] opt_err insert(key_rref key, value_rref value) noexcept {
    return this->template insert_impl<key_rref, value_rref>(
        std::move(key), std::move(value)
    );
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying key and value
   **/
  [[nodiscard]] opt_err insert(key_cref key, const char* value) noexcept {
    return this->template insert_impl<key_cref, const char*>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - bad allocation in resizing the buckets
   *  - bad allocation in creating the node
   *  - bad allocation in copying value
   **/
  [[nodiscard]] opt_err insert(key_rref key, const char* value) noexcept {
    return this->template insert_impl<key_rref, const char*>(
        std::move(key), value
    );
  }
};

// === Specialization === //
template <typename Hash, typename KeyEqual>
class hash_map<string, string, Hash, KeyEqual>
    : public string_both_hash_map<Hash, KeyEqual> {};

template <typename Hash>
class hash_map<string, string, Hash> : public string_both_hash_map<Hash> {};

template <> class hash_map<string, string> : public string_both_hash_map<> {};

template <typename Value, typename Hash, typename KeyEqual>
class hash_map<string, Value, Hash, KeyEqual>
    : public string_key_hash_map<Value, Hash, KeyEqual> {};

template <typename Value, typename Hash>
class hash_map<string, Value, Hash> : public string_key_hash_map<Value, Hash> {
};

template <typename Value>
class hash_map<string, Value> : public string_key_hash_map<Value> {};

template <typename Key, typename Hash, typename KeyEqual>
class hash_map<Key, string, Hash, KeyEqual>
    : public string_value_hash_map<Key, Hash, KeyEqual> {};

template <typename Key, typename Hash>
class hash_map<Key, string, Hash> : public string_value_hash_map<Key, Hash> {};

template <typename Key>
class hash_map<Key, string> : public string_value_hash_map<Key> {};

} // namespace ds

#endif

