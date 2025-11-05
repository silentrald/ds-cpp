/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-10-09
 *===============================*/

#ifndef DS_HASH_MAP_STRING_HPP
#define DS_HASH_MAP_STRING_HPP

#include "./hash_map.hpp"
#include "./string.hpp"
#include "types.hpp"
#include <type_traits>

namespace ds {

// === hash_map with string key/value === //

template <
    typename Derived, typename Hash = hash<string>,
    typename KeyEqual = equal<string>>
class string_hash_map
    : public base_hash_map<Derived, string, string, Hash, KeyEqual> {
public:
  using key_type = string;
  using value_type = string;
  using node_type = typename base_hash_map<
      Derived, string, string, Hash, KeyEqual>::node_type;

  // === Accessors === //

  /**
   * Safe lookup - If the key was found then return the value
   *   else return an error
   *
   * @errors
   *   - error::NOT_FOUND
   **/
  [[nodiscard]] expected<string*, error_code> at(const string& key) noexcept {
    auto* value = this->template find<string*>(key);
    if (value == nullptr) {
      return unexpected<error_code>{error::NOT_FOUND};
    }
    return value;
  }

  /**
   * Safe lookup - If the key was found then return the value
   *   else return an error
   *
   * @errors
   *   - error::NOT_FOUND
   **/
  [[nodiscard]] expected<string*, error_code> at(const c8* key) noexcept {
    auto* value = this->find_with_c8<string*>(key);
    if (value == nullptr) {
      return unexpected<error_code>{error::NOT_FOUND};
    }
    return value;
  }

  /**
   * Unsafe lookup
   *
   *   if a value was found then return the value's address
   *   else return nullptr
   **/
  [[nodiscard]] string* operator[](const string& key) noexcept {
    return this->template find<string*>(key);
  }

  /**
   * Unsafe lookup
   *
   *   if a value was found then return the value's address
   *   else return nullptr
   **/
  [[nodiscard]] string* operator[](const c8* key) noexcept {
    return this->find_with_c8<string*>(key);
  }

  /**
   * Unsafe lookup - if a value was found then return the value's address
   *   else return nullptr
   **/
  [[nodiscard]] const string* operator[](const c8* key) const noexcept {
    return this->find_with_c8<string*>(key);
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const string& key) const noexcept {
    return this->template find<bool>(key);
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const c8* key) const noexcept {
    return this->find_with_c8<bool>(key);
  }

  // === Mutations === //

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] error_code
  insert(const string& key, const string& value) noexcept {
    return this->insert_impl(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key
   **/
  [[nodiscard]] error_code insert(const string& key, string&& value) noexcept {
    return this->insert_impl(std::move(key), value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for value
   **/
  [[nodiscard]] error_code insert(string&& key, const string& value) noexcept {
    return this->insert_impl(key, std::move(value));
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   **/
  [[nodiscard]] error_code insert(string&& key, string&& value) noexcept {
    return this->insert_impl(std::move(key), std::move(value));
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] error_code insert(const c8* key, const string& value) noexcept {
    return this->template insert_c8_key_impl<const string&>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key
   **/
  [[nodiscard]] error_code insert(const c8* key, string&& value) noexcept {
    return this->template insert_c8_key_impl<string&&>(key, std::move(value));
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] error_code insert(const string& key, const c8* value) noexcept {
    return this->template insert_c8_value_impl<const string&>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for value
   **/
  [[nodiscard]] error_code insert(string&& key, const c8* value) noexcept {
    return this->template insert_c8_value_impl<string&&>(std::move(key), value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] error_code insert(const c8* key, const c8* value) noexcept {
    DS_TRY(this->check_allocation());

    node_type node{.distance = 0U};
    DS_TRY(node.key.copy(key));
    DS_TRY(node.value.copy(value));
    this->insert_node(std::move(node));

    return error::OK;
  }

  /**
   * Removes a key/value pair in the hash map
   * If no key was found, nothing will happen to the hash map
   **/
  void remove(const string& key) noexcept {
    this->template erase_impl<const string&>(key);
  }

  /**
   * Removes a key/value pair in the hash map
   * If no key was found, nothing will happen to the hash map
   **/
  void remove(const c8* key) noexcept {
    this->template erase_impl<const c8*>(key);
  }

#ifdef DS_TEST
  void print() const noexcept {
    printf("=== hash_map ===\n");
    if (this->bucket == nullptr) {
      printf("No elements\n");
    }
    for (usize i = 0; i < this->capacity; ++i) {
      printf(
          USIZE_FORMAT ": (%s, %s) distance: %16llx\n", i,
          this->bucket[i].key.c_str(), this->bucket[i].value.c_str(),
          this->bucket[i].distance
      );
    }
  }
#endif

protected:
  template <typename Value_>
  [[nodiscard]] inline error_code
  insert_c8_key_impl(const c8* key, Value_ value) noexcept {
    DS_TRY(this->check_allocation());

    if constexpr (std::is_rvalue_reference<Value_>::value) {
      node_type node{.value = std::move(value), .distance = 0U};
      DS_TRY(node.key.copy(key));
      this->insert_node(std::move(node));
    } else {
      node_type node{.distance = 0U};
      DS_TRY(node.value.copy(value));
      DS_TRY(node.key.copy(key));
      this->insert_node(std::move(node));
    }

    return error::OK;
  }

  template <typename Key_>
  [[nodiscard]] inline error_code
  insert_c8_value_impl(Key_ key, const c8* value) noexcept {
    DS_TRY(this->check_allocation());

    if constexpr (std::is_rvalue_reference<Key_>::value) {
      node_type node{.key = std::move(key), .distance = 0U};
      DS_TRY(node.value.copy(value));
      this->insert_node(std::move(node));
    } else {
      node_type node{.distance = 0U};
      DS_TRY(node.key.copy(key));
      DS_TRY(node.value.copy(value));
      this->insert_node(std::move(node));
    }

    return error::OK;
  }

  template <typename Return>
  [[nodiscard]] Return find_with_c8(const c8* key) const noexcept {
    if (this->is_empty()) {
      if constexpr (std::is_same<Return, string*>::value) {
        return nullptr;
      } else if constexpr (std::is_same<Return, bool>::value) {
        return false;
      }
    }

    // NOTE: No infinite loop since 1 node will always be empty in any case
    for (usize index = this->calculate_hash_index(key);;) {
      if (this->bucket[index].is_node_empty()) {
        if constexpr (std::is_same<Return, string*>::value) {
          return nullptr;
        } else if constexpr (std::is_same<Return, bool>::value) {
          return false;
        }
      }

      if (KeyEqual()(key, this->bucket[index].key)) {
        if constexpr (std::is_same<Return, string*>::value) {
          return &this->bucket[index].value;
        } else if constexpr (std::is_same<Return, bool>::value) {
          return true;
        }
      }

      if (++index >= this->capacity) {
        index = 0U;
      }
    }
  }
};

template <
    typename Derived, typename Value, typename Hash = hash<string>,
    typename KeyEqual = equal<string>>
class string_key_hash_map
    : public base_hash_map<Derived, string, Value, Hash, KeyEqual> {
public:
  using key_type = string;
  using value_type = Value;
  using node_type =
      typename base_hash_map<Derived, string, Value, Hash, KeyEqual>::node_type;

  // === Accessors === //

  /**
   * Safe lookup - If the key was found then return the value
   *   else return an error
   *
   * @errors
   *   - error::NOT_FOUND
   **/
  [[nodiscard]] expected<Value*, error_code> at(const string& key) noexcept {
    auto* value = this->template find<Value*>(key);
    if (value == nullptr) {
      return unexpected<error_code>{error::NOT_FOUND};
    }
    return value;
  }

  /**
   * Safe lookup - If the key was found then return the value
   *   else return an error
   *
   * @errors
   *   - error::NOT_FOUND
   **/
  [[nodiscard]] expected<Value*, error_code> at(const c8* key) noexcept {
    auto* value = this->find_with_c8<Value*>(key);
    if (value == nullptr) {
      return unexpected<error_code>{error::NOT_FOUND};
    }
    return value;
  }

  /**
   * Unsafe lookup
   *
   *   if a value was found then return the value's address
   *   else return nullptr
   **/
  [[nodiscard]] Value* operator[](const string& key) noexcept {
    return this->template find<Value*>(key);
  }

  /**
   * Unsafe lookup
   *
   *   if a value was found then return the value's address
   *   else return nullptr
   **/
  [[nodiscard]] Value* operator[](const c8* key) noexcept {
    return this->find_with_c8<Value*>(key);
  }

  /**
   * Unsafe lookup - if a value was found then return the value's address
   *   else return nullptr
   **/
  [[nodiscard]] const Value* operator[](const c8* key) const noexcept {
    return this->find_with_c8<Value*>(key);
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const string& key) const noexcept {
    return this->template find<bool>(key);
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const c8* key) const noexcept {
    return this->find_with_c8<bool>(key);
  }

  // === Mutations === //

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] error_code
  insert(const string& key, const Value& value) noexcept {
    return this->insert_impl(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key
   **/
  [[nodiscard]] error_code insert(const string& key, Value&& value) noexcept {
    return this->insert_impl(key, std::move(value));
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for value
   **/
  [[nodiscard]] error_code insert(string&& key, const Value& value) noexcept {
    return this->insert_impl(std::move(key), value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   **/
  [[nodiscard]] error_code insert(string&& key, Value&& value) noexcept {
    return this->insert_impl(std::move(key), std::move(value));
  }

  [[nodiscard]] error_code insert(const c8* key, const Value& value) noexcept {
    return this->insert_c8_key_impl<const Value&>(key, value);
  }

  [[nodiscard]] error_code insert(const c8* key, Value&& value) noexcept {
    return this->insert_c8_key_impl<Value&&>(key, std::move(value));
  }

  /**
   * Removes a key/value pair in the hash map
   * If no key was found, nothing will happen to the hash map
   **/
  void remove(const string& key) noexcept {
    this->template erase_impl<const string&>(key);
  }

  /**
   * Removes a key/value pair in the hash map
   * If no key was found, nothing will happen to the hash map
   **/
  void remove(const c8* key) noexcept {
    this->template erase_impl<const c8*>(key);
  }

#ifdef DS_TEST
  void print() const noexcept {
    printf("=== hash_map ===\n");
    if (this->bucket == nullptr) {
      printf("No elements\n");
    }
    for (usize i = 0; i < this->capacity; ++i) {
      printf(
          USIZE_FORMAT ": (%s, %16llx) distance: %16llx\n", i,
          this->bucket[i].key.c_str(), this->bucket[i].value,
          this->bucket[i].distance
      );
    }
  }
#endif

protected:
  template <typename Value_>
  [[nodiscard]] inline error_code
  insert_c8_key_impl(const c8* key, Value_ value) noexcept {
    DS_TRY(this->check_allocation());

    if constexpr (std::is_rvalue_reference<Value_>::value) {
      node_type node{.value = std::move(value), .distance = 0U};
      DS_TRY(node.key.copy(key));
      this->insert_node(std::move(node));
    } else {
      node_type node{.distance = 0U};
      DS_TRY(node.key.copy(key));
      if constexpr (!std::is_class<value_type>::value) {
        node.value = value;
      } else {
        DS_TRY(node.value.copy(value));
      }
      this->insert_node(std::move(node));
    }

    return error::OK;
  }

  template <typename Return>
  [[nodiscard]] Return find_with_c8(const c8* key) const noexcept {
    if (this->is_empty()) {
      if constexpr (std::is_same<Return, Value*>::value) {
        return nullptr;
      } else if constexpr (std::is_same<Return, bool>::value) {
        return false;
      }
    }

    // NOTE: No infinite loop since 1 node will always be empty in any case
    for (usize index = this->calculate_hash_index(key);;) {
      if (this->bucket[index].is_node_empty()) {
        if constexpr (std::is_same<Return, Value*>::value) {
          return nullptr;
        } else if constexpr (std::is_same<Return, bool>::value) {
          return false;
        }
      }

      if (KeyEqual()(key, this->bucket[index].key)) {
        if constexpr (std::is_same<Return, Value*>::value) {
          return &this->bucket[index].value;
        } else if constexpr (std::is_same<Return, bool>::value) {
          return true;
        }
      }

      ++index;
      if (index >= this->capacity) {
        index = 0U;
      }
    }
  }
};

template <
    typename Derived, typename Key, typename Hash = hash<Key>,
    typename KeyEqual = equal<Key>>
class string_value_hash_map
    : public base_hash_map<Derived, Key, string, Hash, KeyEqual> {
public:
  using key_type = Key;
  using value_type = string;
  using node_type =
      typename base_hash_map<Derived, Key, string, Hash, KeyEqual>::node_type;

  // === Mutations === //

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] error_code
  insert(const Key& key, const string& value) noexcept {
    return this->insert_impl(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key
   **/
  [[nodiscard]] error_code insert(const Key& key, string&& value) noexcept {
    return this->insert_impl(key, std::move(value));
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for value
   **/
  [[nodiscard]] error_code insert(Key&& key, const string& value) noexcept {
    return this->insert_impl(std::move(key), value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   **/
  [[nodiscard]] error_code insert(Key&& key, string&& value) noexcept {
    return this->insert_impl(std::move(key), std::move(value));
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key or value
   **/
  [[nodiscard]] error_code insert(const Key& key, const c8* value) noexcept {
    return this->insert_c8_value_impl<const Key&>(key, value);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error::BAD_ALLOCATION - bucket resize or value copy
   *  - error::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for value
   **/
  [[nodiscard]] error_code insert(Key&& key, const c8* value) noexcept {
    return this->insert_c8_value_impl<Key&&>(std::move(key), value);
  }

protected:
  template <typename Key_>
  [[nodiscard]] inline error_code
  insert_c8_value_impl(Key_ key, const c8* value) noexcept {
    DS_TRY(this->check_allocation());

    if constexpr (std::is_rvalue_reference<Key_>::value) {
      node_type node{.key = std::move(key), .distance = 0U};
      DS_TRY(node.value.copy(value));
      this->insert_node(std::move(node));
    } else {
      node_type node{.distance = 0U};
      if constexpr (!std::is_class<key_type>::value) {
        node.key = key;
      } else {
        DS_TRY(node.key.copy(key));
      }
      DS_TRY(node.value.copy(value));
      this->insert_node(std::move(node));
    }

    return error::OK;
  }
};

// === hash_map string specializations === //

template <typename Hash, typename KeyEqual>
class hash_map<string, string, Hash, KeyEqual>
    : public string_hash_map<
          hash_map<string, string, Hash, KeyEqual>, Hash, KeyEqual> {};

template <typename Hash>
class hash_map<string, string, Hash>
    : public string_hash_map<hash_map<string, string, Hash>, Hash> {};

template <>
class hash_map<string, string>
    : public string_hash_map<hash_map<string, string>> {};

template <typename Value, typename Hash, typename KeyEqual>
class hash_map<string, Value, Hash, KeyEqual>
    : public string_key_hash_map<
          hash_map<string, Value, Hash, KeyEqual>, Value, Hash, KeyEqual> {};

template <typename Value, typename Hash>
class hash_map<string, Value, Hash>
    : public string_key_hash_map<hash_map<string, Value, Hash>, Value, Hash> {};

template <typename Value>
class hash_map<string, Value>
    : public string_key_hash_map<hash_map<string, Value>, Value> {};

template <typename Key, typename Hash, typename KeyEqual>
class hash_map<Key, string, Hash, KeyEqual>
    : public string_value_hash_map<
          hash_map<Key, string, Hash, KeyEqual>, Key, Hash, KeyEqual> {};

template <typename Key, typename Hash>
class hash_map<Key, string, Hash>
    : public string_value_hash_map<hash_map<Key, string, Hash>, Key, Hash> {};

template <typename Key>
class hash_map<Key, string>
    : public string_value_hash_map<hash_map<Key, string>, Key> {};

} // namespace ds

#endif
