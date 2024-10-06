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
#include "types.hpp"

namespace ds {

template <> class vector<string> : public base_vector<vector<string>, string> {
public:
  error_code insert(const iterator& it) noexcept = delete;
  error_code push_back() noexcept = delete;

  /**
   * Inserts an element starting from the iterator
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   *  - error_code::INDEX_OUT_OF_BOUNDS
   **/
  template <typename... Args, typename = all_string_type<Args...>>
  [[nodiscard]] error_code insert(const iterator& it, Args&&... args) noexcept {
    return this->insert(it - this->begin(), std::forward<Args>(args)...);
  }

  /**
   * Inserts an element starting from the iterator
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   *  - error_code::INDEX_OUT_OF_BOUNDS
   **/
  template <typename... Args, typename = all_string_type<Args...>>
  [[nodiscard]] error_code insert(i32 index, Args&&... args) noexcept {
    if (index < 0 || index > this->top) {
      return error_code::INDEX_OUT_OF_BOUNDS;
    }

    if (index == this->top) {
      return this->push_back(std::forward<Args>(args)...);
    }

    TRY(this->grow(this->top + sizeof...(Args)));
    this->shift(index, sizeof...(Args));

    return this->insert_helper(index, std::forward<Args>(args)...);
  }

  /**
   * Pushes an element at the back of the vector
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  template <typename... Args, typename = all_string_type<Args...>>
  [[nodiscard]] error_code push_back(Args&&... args) noexcept {
    TRY(this->grow(this->top + sizeof...(Args)));

    return this->push_back_helper(std::forward<Args>(args)...);
  }

private:
  // === Insert === //

  template <typename... Args>
  [[nodiscard]] error_code
  insert_helper(i32 index, const string& first, Args&&... args) noexcept {
    TRY(this->arr[index].copy(first));

    if constexpr (sizeof...(Args)) {
      return this->insert_helper(index + 1, std::forward<Args>(args)...);
    } else {
      return error_code::OK;
    }
  }

  template <typename... Args>
  [[nodiscard]] error_code
  insert_helper(i32 index, string&& first, Args&&... args) noexcept {
    this->arr[index] = std::move(first);

    if constexpr (sizeof...(Args)) {
      return this->insert_helper(index + 1, std::forward<Args>(args)...);
    } else {
      return error_code::OK;
    }
  }

  template <typename... Args>
  [[nodiscard]] error_code
  insert_helper(i32 index, const char* first, Args&&... args) noexcept {
    TRY(this->arr[index].copy(first));

    if constexpr (sizeof...(Args)) {
      return this->insert_helper(index + 1, std::forward<Args>(args)...);
    } else {
      return error_code::OK;
    }
  }

  template <typename... Args>
  [[nodiscard]] error_code
  push_back_helper(const string& first, Args&&... args) noexcept {
    TRY(this->arr[this->top].copy(first));
    ++this->top;

    if constexpr (sizeof...(Args)) {
      return this->push_back_helper(std::forward<Args>(args)...);
    } else {
      return error_code::OK;
    }
  }

  template <typename... Args>
  [[nodiscard]] error_code
  push_back_helper(string&& first, Args&&... args) noexcept {
    this->arr[this->top++] = std::move(first);

    if constexpr (sizeof...(Args)) {
      return this->push_back_helper(std::forward<Args>(args)...);
    } else {
      return error_code::OK;
    }
  }

  template <typename... Args>
  [[nodiscard]] error_code
  push_back_helper(const char* first, Args&&... args) noexcept {
    TRY(this->arr[this->top].copy(first));
    ++this->top;

    if constexpr (sizeof...(Args)) {
      return this->push_back_helper(std::forward<Args>(args)...);
    } else {
      return error_code::OK;
    }
  }
};

} // namespace ds

#endif
