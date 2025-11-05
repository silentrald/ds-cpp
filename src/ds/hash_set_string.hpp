/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-10-09
 *===============================*/

#ifndef DS_HASH_SET_STRING_HPP
#define DS_HASH_SET_STRING_HPP

#include "./hash_set.hpp"
#include "./string.hpp"
#include "types.hpp"

namespace ds {

// === hash_set with string key === //

template <
    typename Derived, typename Hash = hash<string>,
    typename KeyEqual = equal<string>>
class string_hash_set : public base_hash_set<Derived, string, Hash, KeyEqual> {
public:
  using key_type = string;
  using node_type =
      typename base_hash_set<Derived, string, Hash, KeyEqual>::node_type;

  // === Accessors === //

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const string& key) const noexcept {
    return this->find(key);
  }

  /**
   * Checks if the value exists in the hash map
   **/
  [[nodiscard]] bool contains(const c8* key) const noexcept {
    // NOTE: No infinite loop since 1 node will always be empty in any case
    for (usize index = this->template calculate_hash_index<const c8*>(key);;) {
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

  // === Mutations === //

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error_codes::BAD_ALLOCATION - bucket resize or value copy
   *  - error_codes::CONTAINER_FULL - max limit of hash_map
   *  - error_code from copy for key
   **/
  [[nodiscard]] error_code insert(const string& key) noexcept {
    return this->insert_impl(key);
  }

  /**
   * Inserts a value in the hash map
   *
   * @errors
   *  - error_codes::BAD_ALLOCATION - bucket resize or value copy
   *  - error_codes::CONTAINER_FULL - max limit of hash_map
   **/
  [[nodiscard]] error_code insert(string&& key) noexcept {
    return this->insert_impl(std::move(key));
  }

  [[nodiscard]] error_code insert(const c8* key) noexcept {
    DS_TRY(this->check_allocation());

    node_type node{.distance = 0U};
    DS_TRY(node.key.copy(key));
    this->insert_node(std::move(node));

    return error_codes::OK;
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
    printf("=== hash_set ===\n");
    if (this->bucket == nullptr) {
      printf("No elements\n");
    }
    for (usize i = 0; i < this->capacity; ++i) {
      printf(
          USIZE_FORMAT ": (%s) distance: %16llx\n", i,
          this->bucket[i].key.c_str(), this->bucket[i].distance
      );
    }
  }
#endif
};

// === hash_map string specializations === //

template <typename Hash, typename KeyEqual>
class hash_set<string, Hash, KeyEqual>
    : public string_hash_set<hash_set<string, Hash, KeyEqual>, Hash, KeyEqual> {
};

template <typename Hash>
class hash_set<string, Hash>
    : public string_hash_set<hash_set<string, Hash>, Hash> {};

template <>
class hash_set<string> : public string_hash_set<hash_set<string>> {};

} // namespace ds

#endif
