/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_VECTOR_HPP
#define DS_VECTOR_HPP

#include "./allocator.hpp"
#include "./vector_iterator.hpp"
#include "types.hpp"
#include <cassert>
#include <cstring>
#include <type_traits>

#ifdef DS_TEST
#include <cstdio>
#endif

namespace ds {

inline const usize VECTOR_INITIAL_SIZE = 8U;

/**
 * Array container with dynamic sizing.
 */
template <typename Derived, typename T, typename Allocator = allocator<T>>
class base_vector {
public:
  friend Derived;

  using value_type = T;

  using iterator = vector_iterator<T>;
  using const_iterator = vector_iterator<const T>;

  base_vector() noexcept = default;
  base_vector(const base_vector&) = delete;
  base_vector& operator=(const base_vector& rhs) = delete;

  // === Move === //

  /**
   * Initializes the vector and moves the passed vector
   **/
  base_vector(base_vector&& other) noexcept
      : array(other.array), size(other.size), capacity(other.capacity) {
    other.array = nullptr;
    other.size = other.capacity = 0U;
  }

  /**
   * Moves the passed vector
   * This frees the currect vector
   **/
  base_vector& operator=(base_vector&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->destroy();

    this->array = rhs.array;
    this->size = rhs.size;
    this->capacity = rhs.capacity;

    rhs.array = nullptr;
    rhs.size = rhs.capacity = 0U;

    return *this;
  }

  // === Copy === //

  /**
   * Copies the passed vector.
   * This will resize if the passed vector is bigger
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const base_vector& other) noexcept {
    if (this == &other) {
      return error_code::OK;
    }

    if (other.is_empty()) {
      this->destroy();
      return error_code::OK;
    }

    if (this->array == nullptr) {
      TRY(this->allocate(other.capacity));
    } else if (this->capacity < other.capacity) {
      TRY(this->reallocate(other.capacity));
    }

    for (usize i = 0U; i < other.size; ++i) {
      if constexpr (!std::is_class<T>::value) {
        this->array[i] = other.array[i];
      } else {
        TRY(this->array[i].copy(other.array[i]));
      }
    }

    this->size = other.size;
    return error_code::OK;
  }

  // === Destructor === //

  ~base_vector() noexcept {
    this->destroy();
  }

  /**
   * Frees the elements and data of the vector
   **/
  void destroy() noexcept {
    if (this->array == nullptr) {
      return;
    }

    if constexpr (std::is_class<T>::value) {
      for (usize i = 0; i < this->capacity; ++i) {
        this->array[i].~T();
      }
    }

    Allocator{}.deallocate(this->array);
    this->array = nullptr;
    this->size = this->capacity = 0U;
  }

  /**
   * Empties the vector, but does not free the data
   **/
  void clear() noexcept {
    this->size = 0;
  }

  // === Accessing === //

  /**
   * Safe index accessing
   *
   * @errors
   *  - error_code::INDEX_OUT_OF_BOUNDS
   **/
  [[nodiscard]] expected<T*, error_code> at(usize index) const noexcept {
    if (index > this->size) {
      return unexpected{error_code::INDEX_OUT_OF_BOUNDS};
    }
    return this->array + index;
  }

  /**
   * Unsafe index accessing
   **/
  [[nodiscard]] T& operator[](usize index) noexcept {
    assert(index <= this->size);
    return this->array[index];
  }

  /**
   * Unsafe index accessing
   **/
  [[nodiscard]] const T& operator[](usize index) const noexcept {
    assert(index <= this->size);
    return this->array[index];
  }

  /**
   * Unsafe accessing to return the first element of the array.
   **/
  [[nodiscard]] T& front() noexcept {
    assert(this->size > 0U);
    return this->array[0];
  }

  /**
   * Unsafe accessing to return the first element of the array.
   **/
  [[nodiscard]] const T& front() const noexcept {
    assert(this->size > 0U);
    return this->array[0];
  }

  /**
   * [Unsafe] accessing to return the last element of the array.
   **/
  [[nodiscard]] T& back() noexcept {
    assert(this->size > 0U);
    return this->array[this->size - 1U];
  }

  /**
   * [Unsafe] accessing to return the last element of the array.
   **/
  [[nodiscard]] const T& back() const noexcept {
    assert(this->size > 0U);
    return this->array[this->size - 1U];
  }

  /**
   * [Safe] accessing to return the first element of the array.
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY
   **/
  [[nodiscard]] expected<T*, error_code> front_safe() const noexcept {
    if (this->size == 0U) {
      return unexpected{error_code::CONTAINER_EMPTY};
    }
    return this->array;
  }

  /**
   * [Safe] accessing to return the last element of the array.
   **/
  [[nodiscard]] expected<T*, error_code> back_safe() const noexcept {
    if (this->size == 0U) {
      return unexpected{error_code::CONTAINER_EMPTY};
    }
    return this->array + this->size - 1U;
  }

  /**
   * Returns the ptr of the vector container
   **/
  [[nodiscard]] T* get_data() const noexcept {
    return this->array;
  }

  // === Iterators === //

  /**
   * Points to the 0-th index of the vector
   **/
  [[nodiscard]] iterator begin() noexcept {
    return iterator{this->array};
  }

  /**
   * Points to the 0-th index of the vector
   **/
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return const_iterator{this->array};
  }

  /**
   * Points to the top index of the vector
   **/
  [[nodiscard]] iterator end() noexcept {
    return iterator{this->array + this->size};
  }

  /**
   * Points to the top index of the vector
   **/
  [[nodiscard]] const_iterator cend() const noexcept {
    return const_iterator{this->array + this->size};
  }

  /**
   * Points at the top-1 index of the vector
   **/
  [[nodiscard]] iterator rbegin() noexcept {
    return iterator{this->array + this->size - 1U};
  }

  /**
   * Points at the top-1 index of the vector
   **/
  [[nodiscard]] const_iterator crbegin() const noexcept {
    return const_iterator{this->array + this->size - 1U};
  }

  /**
   * Points at the -1th index of the vector
   **/
  [[nodiscard]] iterator rend() noexcept {
    return iterator{this->array - 1U};
  }

  /**
   * Points at the -1th index of the vector
   **/
  [[nodiscard]] const_iterator crend() const noexcept {
    return const_iterator{this->array - 1U};
  }

  // === Sizes === //

  /**
   * Check if the vector has no elements
   *
   * @return bool
   **/
  [[nodiscard]] bool is_empty() const noexcept {
    return this->size == 0;
  }

  /**
   * Returns the current size of the vector
   **/
  [[nodiscard]] usize get_size() const noexcept {
    return this->size;
  }

  /**
   * Returns the size of the memory block allocated
   **/
  [[nodiscard]] usize get_capacity() const noexcept {
    return this->capacity;
  }

  /**
   * Reserves size for the vector
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code reserve(usize new_capacity) noexcept {
    if (this->array == nullptr) {
      TRY(this->allocate(new_capacity));
    } else if (new_capacity > this->capacity) {
      TRY(this->reallocate(new_capacity));
    }
    return error_code::OK;
  }

  // === Mutations === //

  [[nodiscard]] error_code push(const T& element) noexcept {
    if constexpr (std::is_class<T>::value) {
      T e{};
      TRY(e.copy(element));
      return this->push_impl(std::move(e));
    } else {
      return this->push_impl(element);
    }
  }

  [[nodiscard]] error_code push(T&& element) noexcept {
    return this->push_impl(std::move(element));
  }

  [[nodiscard]] error_code insert(usize index, const T& element) noexcept {
    if constexpr (std::is_class<T>::value) {
      T e{};
      TRY(e.copy(element));
      return this->insert_impl(index, std::move(e));
    } else {
      return this->insert_impl(index, element);
    }
  }

  [[nodiscard]] error_code insert(usize index, T&& element) noexcept {
    return this->insert_impl(index, std::move(element));
  }

  /**
   * Resizes the vector
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code resize(usize size) noexcept {
    if (this->array == nullptr) {
      TRY(this->allocate(size));
      if constexpr (!std::is_class<T>::value) {
        std::memset(this->array, sizeof(T), size);
      }
    } else if (this->array) {
      TRY(this->reallocate(size));
      if constexpr (!std::is_class<T>::value) {
        // Reinitialize the popped values
        std::memset(this->array, sizeof(T), size - this->size);
      }
    }
    this->size = size;
    return error_code::OK;
  }

  /**
   * [Unsafe] removes the element at the index
   **/
  void remove(usize index) noexcept {
    assert(index < this->size);

    --this->size;
    for (; index < this->size; ++index) {
      this->array[index] = std::move(this->array[index + 1]);
    }
  }

  /**
   * [Safe] removes the element at the index
   *
   * @errors
   *  - error_code::INDEX_OUT_OF_BOUNDS
   **/
  [[nodiscard]] error_code remove_safe(usize index) noexcept {
    if (index >= this->size) {
      return error_code::INDEX_OUT_OF_BOUNDS;
    }

    --this->size;
    for (; index < this->size; ++index) {
      this->array[index] = std::move(this->array[index + 1]);
    }
    return error_code::OK;
  }

  /**
   * [Unsafe] pops an element from the back and returns the element
   **/
  T pop() noexcept {
    assert(this->size > 0U);
    return std::move(this->array[--this->size]);
  }

  /**
   * [Safe] pops an element from the back and returns the element
   *
   * @errors
   *  - error_code::CONTAINER_EMPTY
   **/
  [[nodiscard]] expected<T, error_code> pop_safe() noexcept {
    if (this->size == 0U) {
      return unexpected{error_code::CONTAINER_EMPTY};
    }
    return std::move(this->array[--this->size]);
  }

#ifdef DS_TEST
  void print() const noexcept {
    printf("=== vector ===\n> capacity: " USIZE_FORMAT "\n", this->capacity);
    for (usize i = 0U; i < this->size; ++i) {
      printf(USIZE_FORMAT ": %llx\n", i, this->array[i]);
    }
  }
#endif

protected:
  T* array = nullptr;
  usize size = 0;
  usize capacity = 0;

  // === Helpers === //

  [[nodiscard]] inline error_code push_impl(T element) noexcept {
    TRY(this->check_allocation());

    this->array[this->size] = std::move(element);
    ++this->size;

    return error_code::OK;
  }

  [[nodiscard]] inline error_code insert_impl(usize index, T element) noexcept {
    if (index > this->size) {
      return error_code::INDEX_OUT_OF_BOUNDS;
    }

    TRY(this->check_allocation());

    // Shift
    for (usize i = this->size; i > index; --i) {
      this->array[i] = std::move(this->array[i - 1]);
    }
    this->array[index] = std::move(element);
    ++this->size;

    return error_code::OK;
  }

  // === Memory === //

  /**
   * Allocates a new memory block for the container
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code allocate(usize new_capacity) noexcept {
    assert(new_capacity > 0U);

    this->array =
        static_cast<T*>(Allocator{}.allocate(new_capacity * sizeof(T)));
    if (this->array == nullptr) {
      return error_code::BAD_ALLOCATION;
    }

    // Data initialization
    if constexpr (std::is_class<T>::value) {
      new (this->array) T[new_capacity];
    }

    this->capacity = new_capacity;
    return error_code::OK;
  }

  /**
   * Reallocates the old memory block, always assume that
   *   new_capacity > this->capacity
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code reallocate(usize new_capacity) noexcept {
    assert(new_capacity > 0U);

    auto* new_array = static_cast<T*>(
        Allocator{}.reallocate(this->array, new_capacity * sizeof(T))
    );
    if (new_array == nullptr) {
      return error_code::BAD_ALLOCATION;
    }
    this->array = new_array;

    if constexpr (std::is_class<T>::value) {
      new (this->array + this->capacity) T[new_capacity - this->capacity];
    }

    this->capacity = new_capacity;
    return error_code::OK;
  }

  /**
   * Calculates the new size before allocation/reallocation
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code check_allocation() noexcept {
    if (this->array == nullptr) {
      TRY(this->allocate(VECTOR_INITIAL_SIZE));
    } else if (this->size == this->capacity) {
      TRY(this->reallocate(this->capacity * 2U));
    }
    return error_code::OK;
  }
};

} // namespace ds

// === Vector declaration === //

namespace ds {

template <typename T, typename Allocator = allocator<T>>
class vector : public base_vector<vector<T, Allocator>, T, Allocator> {};

} // namespace ds

#ifndef DS_VECTOR_STRING_HPP
#include "./vector_string.hpp"
#endif

#endif
