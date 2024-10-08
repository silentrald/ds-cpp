/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_VECTOR_HPP
#define DS_VECTOR_HPP

#include "./type_traits.hpp"
#include "./vector_iterator.hpp"
#include "types.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#ifdef DS_TEST
#include "../../tests/main.hpp"
#endif

namespace ds {

/**
 * Vector class that uses expected as error handling
 *
 * @refer https://en.cppref.com/w/cpp/container/vector
 */
template <typename Derived, typename T> class base_vector {
public:
  friend Derived;

  using value_type = T;

  using iterator = vector_iterator<base_vector<Derived, T>>;
  using citerator = vector_iterator<const base_vector<Derived, T>>;

protected:
  T* arr = nullptr;
  usize top = 0;
  usize capacity = 0;

  void destroy() noexcept;

  // === Memory ===
  /**
   * Allocates a new memory block for the container
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] error_code allocate(usize size) noexcept;

  /**
   * Reallocates the old memory block
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] error_code reallocate(usize size) noexcept;

  /**
   * Calculates the new size before allocation/reallocation
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] error_code grow(usize min_size) noexcept;

  // Helper function for insert to move elements
  /**
   * Shift the elements to the right to the end of the array.
   *
   * [*****___] -> [**___***]
   *    ^ - start
   **/
  void shift(usize start, usize n) noexcept;

  // === Insert === //
  template <typename... Args>
  [[nodiscard]] error_code
  insert_helper(usize index, const T& first, Args&&... args) noexcept;

  template <typename... Args>
  [[nodiscard]] error_code
  insert_helper(usize index, T&& first, Args&&... args) noexcept;

  // === Push Back === //
  template <typename... Args>
  [[nodiscard]] error_code
  push_back_helper(const T& first, Args&&... args) noexcept;

  template <typename... Args>
  [[nodiscard]] error_code push_back_helper(T&& first, Args&&... args) noexcept;

  void erase_impl(usize index) noexcept;

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
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const base_vector& rhs) noexcept;

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
   *  - error_code::INDEX_OUT_OF_BOUNDS
   **/
  [[nodiscard]] expected<T*, error_code> at(usize index) const noexcept;

  /**
   * Unsafe index accessing
   **/
  [[nodiscard]] T& operator[](usize index) const noexcept;

  /**
   * Returns the expected ptr at the start of the vector
   *
   * @errors
   *  - Out of range
   **/
  [[nodiscard]] expected<T*, error_code> front() const noexcept;

  /**
   * Returns the expected ptr at the end of the vector
   *
   * @errors
   *  - error_code::INDEX_OUT_OF_BOUNDS
   *
   **/
  [[nodiscard]] expected<T*, error_code> back() const noexcept;

  /**
   * Returns the expected ptr at the start of the vector
   **/
  [[nodiscard]] T& front_unsafe() const noexcept;

  /**
   * Returns the expected ptr at the end of the vector
   **/
  [[nodiscard]] T& back_unsafe() const noexcept;

  /**
   * Returns the ptr of the vector container
   **/
  [[nodiscard]] T* data() const noexcept;

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
   **/
  [[nodiscard]] usize get_size() const noexcept;

  /**
   * Returns the size of the memory block allocated
   **/
  [[nodiscard]] usize get_capacity() const noexcept;

  /**
   * Reserves size for the vector
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code reserve(usize size) noexcept;

  // === Modifiers === //
  /**
   * Clears the vector
   **/
  void clear() noexcept;

  /**
   * Resizes the vector
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code resize(usize size) noexcept;

  /**
   * Removes the element at the iterator
   **/
  void erase(const iterator& it) noexcept {
    this->erase_impl(it - this->begin());
  }

  /**
   * Removes the element at the iterator
   **/
  void erase(usize index) noexcept {
    this->erase_impl(index);
  }

  /**
   * Pops an element from the back and returns the element
   *
   * @errors
   * - error_code::CONTAINER_EMPTY
   **/
  [[nodiscard]] expected<T, error_code> pop_back() noexcept;

  /**
   * Pops an element from the back and returns the element
   * Should crash when this is empty
   **/
  [[nodiscard]] T&& pop_back_unsafe() noexcept;

  /**
   * Pops an element from the back and discards it
   **/
  void pop_back_disc() noexcept;

  // TODO: Change this to usize index
  error_code insert(const iterator& it) noexcept = delete;
  error_code push_back() noexcept = delete;

  /**
   * Inserts an element starting from the iterator
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   *  - error_code::INDEX_OUT_OF_BOUNDS
   **/
  template <typename... Args, typename = all_same_type<T, Args...>>
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
  template <typename... Args, typename = all_same_type<T, Args...>>
  [[nodiscard]] error_code insert(usize index, Args&&... args) noexcept {
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
  template <typename... Args, typename = all_same_type<T, Args...>>
  [[nodiscard]] error_code push_back(Args&&... args) noexcept {
    TRY(this->grow(this->top + sizeof...(Args)));

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
