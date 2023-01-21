/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-01-03
 *===============================*/

#ifndef DS_VECTOR_STRING_HPP
#define DS_VECTOR_STRING_HPP

#include "../string/def.hpp"
#include "./def.hpp"

namespace ds {

template <> class vector<string> : public base_vector<vector<string>, string> {
private:
  // * Insert * //
  template <typename... Args>
  [[nodiscard]] opt_err
  insert_helper(i32 index, cref first, Args&&... args) noexcept {
    try_opt(this->arr[index].copy(first));

    if constexpr (sizeof...(Args)) {
      return this->insert_helper(index + 1, std::forward<Args>(args)...);
    } else {
      return null;
    }
  }

  template <typename... Args>
  [[nodiscard]] opt_err
  insert_helper(i32 index, rref first, Args&&... args) noexcept {
    this->arr[index] = std::move(first);

    if constexpr (sizeof...(Args)) {
      return this->insert_helper(index + 1, std::forward<Args>(args)...);
    } else {
      return null;
    }
  }

  template <typename... Args>
  [[nodiscard]] opt_err
  insert_helper(i32 index, const char* first, Args&&... args) noexcept {
    try_opt(this->arr[index].copy(first));

    if constexpr (sizeof...(Args)) {
      return this->insert_helper(index + 1, std::forward<Args>(args)...);
    } else {
      return null;
    }
  }

  template <typename... Args>
  [[nodiscard]] opt_err push_back_helper(cref first, Args&&... args) noexcept {
    try_opt(this->arr[this->top].copy(first));
    ++this->top;

    if constexpr (sizeof...(Args)) {
      return this->push_back_helper(std::forward<Args>(args)...);
    } else {
      return null;
    }
  }

  template <typename... Args>
  [[nodiscard]] opt_err push_back_helper(rref first, Args&&... args) noexcept {
    this->arr[this->top++] = std::move(first);

    if constexpr (sizeof...(Args)) {
      return this->push_back_helper(std::forward<Args>(args)...);
    } else {
      return null;
    }
  }

  template <typename... Args>
  [[nodiscard]] opt_err
  push_back_helper(const char* first, Args&&... args) noexcept {
    try_opt(this->arr[this->top].copy(first));
    ++this->top;

    if constexpr (sizeof...(Args)) {
      return this->push_back_helper(std::forward<Args>(args)...);
    } else {
      return null;
    }
  }

public:
  opt_err insert(const iterator& it) noexcept = delete;
  opt_err push_back() noexcept = delete;

  /**
   * Inserts an element starting from the iterator
   *
   * @errors
   *  - bad allocation
   *  - index out of range
   **/
  template <typename... Args, typename = all_string_type<Args...>>
  [[nodiscard]] opt_err insert(const iterator& it, Args&&... args) noexcept {
    return this->insert(it - this->begin(), std::forward<Args>(args)...);
  }

  /**
   * Inserts an element starting from the iterator
   *
   * @errors
   *  - bad allocation
   *  - index out of range
   **/
  template <typename... Args, typename = all_string_type<Args...>>
  [[nodiscard]] opt_err insert(i32 index, Args&&... args) noexcept {
    if (index < 0 || index > this->top) {
      return error{VECTOR_OUT_OF_RANGE};
    }

    if (index == this->top) {
      return this->push_back(std::forward<Args>(args)...);
    }

    try_opt(this->grow(this->top + sizeof...(Args)));
    this->shift(index, sizeof...(Args));

    return this->insert_helper(index, std::forward<Args>(args)...);
  }

  /**
   * Pushes an element at the back of the vector
   *
   * @errors
   *  - bad allocation
   **/
  template <typename... Args, typename = all_string_type<Args...>>
  [[nodiscard]] opt_err push_back(Args&&... args) noexcept {
    try_opt(this->grow(this->top + sizeof...(Args)));

    return this->push_back_helper(std::forward<Args>(args)...);
  }
};

} // namespace ds

#endif

