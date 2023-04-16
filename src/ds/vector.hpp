/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_VECTOR_HPP
#define DS_VECTOR_HPP

#include "./vector_iterator.hpp"
#include "ds-error/types.hpp"
#include "ds/macro.hpp"
#include "ds/string.hpp"
#include "ds/type_traits.hpp"
#include "ds/types.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>

#ifdef DS_TEST
#include "../../tests/main.hpp"
#endif

namespace ds {

const char* const VECTOR_BAD_ALLOC = "vector:bad_alloc";
const char* const VECTOR_OUT_OF_RANGE = "vector:out_of_range";
const char* const VECTOR_EMPTY = "vector:empty";

/**
 * Vector class that uses expected as error handling
 *
 * @refer https://en.cppref.com/w/cpp/container/vector
 */
template <typename Derived, typename T> class base_vector {
public:
  friend Derived;

  using value = T;
  using ref = T&;
  using cref = const T&;
  using rref = T&&;
  using ptr = T*;

  using iterator = vector_iterator<base_vector<Derived, T>>;
  using citerator = vector_iterator<const base_vector<Derived, T>>;

protected:
  ptr arr = nullptr;
  i32 top = 0;
  i32 _max_size = 0;

  void destroy() noexcept;

  // === Memory ===
  /**
   * Allocates a new memory block for the container
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] opt_err allocate(i32 size) noexcept;

  /**
   * Reallocates the old memory block
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] opt_err reallocate(i32 size) noexcept;

  /**
   * Calculates the new size before allocation/reallocation
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] opt_err grow(i32 min_size) noexcept;

  // Helper function for insert to move elements
  /**
   * Shift the elements to the right to the end of the array.
   *
   * [*****___] -> [**___***]
   *    ^ - start
   **/
  void shift(i32 start, i32 n) noexcept;

  // === Insert === //
  template <typename... Args>
  [[nodiscard]] opt_err
  insert_helper(i32 index, cref first, Args&&... args) noexcept;

  template <typename... Args>
  [[nodiscard]] opt_err
  insert_helper(i32 index, rref first, Args&&... args) noexcept;

  // === Push Back === //
  template <typename... Args>
  [[nodiscard]] opt_err push_back_helper(cref first, Args&&... args) noexcept;

  template <typename... Args>
  [[nodiscard]] opt_err push_back_helper(rref first, Args&&... args) noexcept;

  void erase_impl(i32 index) noexcept;

public:
  base_vector() noexcept = default;
  base_vector(const base_vector&) = delete;
  base_vector& operator=(const base_vector& rhs) = delete;

  // === Copy === //
  /**
   * Copies the passed vector.
   * This will resize if the passed vector is bigger
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] opt_err copy(const base_vector& rhs) noexcept;

  // === Move === //
  /**
   * Initializes the vector and moves the passed vector
   **/
  base_vector(base_vector&& rhs) noexcept;

  /**
   * Moves the passed vector
   * This frees the currect vector
   **/
  base_vector& operator=(base_vector&& rhs) noexcept;

  // === Deconstructor === //
  ~base_vector();

  // === Element Access === //
  /**
   * Safe index accessing
   *
   * @errors
   *  - Out of range
   *
   * @return exp_ptr_err<value>
   **/
  [[nodiscard]] exp_ptr_err<T> at(i32 index) const noexcept;

  /**
   * Unsafe index accessing
   *
   * @return ref
   **/
  [[nodiscard]] ref operator[](i32 index) const noexcept;

  /**
   * Returns the expected ptr at the start of the vector
   *
   * @errors
   *  - Out of range
   *
   *  @return exp_ptr_err<value>
   **/
  [[nodiscard]] exp_ptr_err<T> front() const noexcept;

  /**
   * Returns the expected ptr at the end of the vector
   *
   * @errors
   *  - Out of range
   *
   *  @return exp_ptr_err<value>
   **/
  [[nodiscard]] exp_ptr_err<T> back() const noexcept;

  /**
   * Returns the expected ptr at the start of the vector
   *
   *  @return ref
   **/
  [[nodiscard]] ref front_unsafe() const noexcept;

  /**
   * Returns the expected ptr at the end of the vector
   *
   *  @return ref
   **/
  [[nodiscard]] ref back_unsafe() const noexcept;

  /**
   * Returns the ptr of the vector container
   *
   * @return ptr
   **/
  [[nodiscard]] ptr data() const noexcept;

  // === Iterators === //
  /**
   * Points to the 0-th index of the vector
   **/
  [[nodiscard]] iterator begin() noexcept;

  /**
   * Points to the 0-th index of the vector
   **/
  [[nodiscard]] citerator cbegin() const noexcept;

  /**
   * Points to the top index of the vector
   **/
  [[nodiscard]] iterator end() noexcept;

  /**
   * Points to the top index of the vector
   **/
  [[nodiscard]] citerator cend() const noexcept;

  /**
   * Points at the top-1 index of the vector
   **/
  [[nodiscard]] iterator rbegin() noexcept;

  /**
   * Points at the top-1 index of the vector
   **/
  [[nodiscard]] citerator crbegin() const noexcept;

  /**
   * Points at the -1th index of the vector
   **/
  [[nodiscard]] iterator rend() noexcept;

  /**
   * Points at the -1th index of the vector
   **/
  [[nodiscard]] citerator crend() const noexcept;

  // === Capacity === //
  /**
   * Check if the vector has no elements
   *
   * @return bool
   **/
  [[nodiscard]] bool is_empty() const noexcept;

  /**
   * Returns the current size of the vector
   *
   * @return i32
   **/
  [[nodiscard]] i32 size() const noexcept;

  /**
   * Returns the size of the memory block allocated
   *
   * @return i32
   **/
  [[nodiscard]] i32 max_size() const noexcept;

  /**
   * Reserves size for the vector
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] opt_err reserve(i32 size) noexcept;

  // === Modifiers === //
  /**
   * Clears the vector
   **/
  void clear() noexcept;

  /**
   * Resizes the vector
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] opt_err resize(i32 size) noexcept;

  /**
   * Removes the element at the iterator
   **/
  void erase(const iterator& it) noexcept {
    this->erase_impl(it - this->begin());
  }

  /**
   * Removes the element at the iterator
   **/
  void erase(i32 index) noexcept {
    this->erase_impl(index);
  }

  /**
   * Pops an element from the back and returns the element
   *
   * @errors
   * - When the vector is empty
   **/
  [[nodiscard]] exp_err<T> pop_back() noexcept;

  /**
   * Pops an element from the back and returns the element
   * Should crash when this is empty
   **/
  [[nodiscard]] T&& pop_back_unsafe() noexcept;

  /**
   * Pops an element from the back and discards it
   **/
  void pop_back_disc() noexcept;

  opt_err insert(const iterator& it) noexcept = delete;
  opt_err push_back() noexcept = delete;

  /**
   * Inserts an element starting from the iterator
   *
   * @errors
   *  - bad allocation
   *  - index out of range
   **/
  template <typename... Args, typename = all_same_type<T, Args...>>
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
  template <typename... Args, typename = all_same_type<T, Args...>>
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
  template <typename... Args, typename = all_same_type<T, Args...>>
  [[nodiscard]] opt_err push_back(Args&&... args) noexcept {
    try_opt(this->grow(this->top + sizeof...(Args)));

    return this->push_back_helper(std::forward<Args>(args)...);
  }

  // === Non-member Operators === //
  template <typename Derived_, typename T_>
  friend bool operator==(
      const base_vector<Derived_, T_>& lhs, const base_vector<Derived_, T_>& rhs
  ) noexcept;

  template <typename Derived_, typename T_>
  friend bool operator!=(
      const base_vector<Derived_, T_>& lhs, const base_vector<Derived_, T_>& rhs
  ) noexcept;
};

} // namespace ds

#ifndef DS_VECTOR_TPP
#include "./vector.tpp"
#endif

// * Vector declaration *
namespace ds {
template <typename T> class vector : public base_vector<vector<T>, T> {};
} // namespace ds

#ifndef DS_VECTOR_STRING_HPP
#include "./vector_string.hpp"
#endif

#endif

