/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_VECTOR_STRING_HPP
#define DS_VECTOR_STRING_HPP

#include "./string.hpp"
#include "./vector.hpp"

namespace ds {

template <typename Allocator>
class vector<string, Allocator>
    : public base_vector<vector<string, Allocator>, string, Allocator> {
public:
  [[nodiscard]] error_code push(const string& element) noexcept {
    string str{};
    DS_TRY(str.copy(element));
    return this->push_impl(std::move(str));
  }

  [[nodiscard]] error_code push(string&& element) noexcept {
    return this->push_impl(std::move(element));
  }

  [[nodiscard]] error_code push(const c8* element) noexcept {
    string str{};
    DS_TRY(str.copy(element));
    return this->push_impl(std::move(str));
  }

  [[nodiscard]] error_code insert(usize index, const string& element) noexcept {
    string str{};
    DS_TRY(str.copy(element));
    return this->insert_impl(index, std::move(str));
  }

  [[nodiscard]] error_code insert(usize index, string&& element) noexcept {
    return this->insert_impl(index, std::move(element));
  }

  [[nodiscard]] error_code insert(usize index, const c8* element) noexcept {
    string str{};
    DS_TRY(str.copy(element));
    return this->insert_impl(index, std::move(str));
  }

#ifdef DS_TEST
  void print() const noexcept {
    printf("=== vector ===\n> capacity: " USIZE_FORMAT "\n", this->capacity);
    for (usize i = 0U; i < this->size; ++i) {
      printf(USIZE_FORMAT ": %s\n", i, this->array[i].c_str());
    }
  }
#endif
};

} // namespace ds

#endif
