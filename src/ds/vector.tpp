/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_VECTOR_TPP
#define DS_VECTOR_TPP

#include "./vector.hpp"
#include "types.hpp"
#include <cassert>
#include <type_traits>

// Implementation
namespace ds {

// === Copy ===
template <typename Derived, typename T>
error_code base_vector<Derived, T>::copy(const base_vector& rhs) noexcept {
  if (&rhs == this) {
    return error_code::OK;
  }

  if (rhs.is_empty()) {
    this->clear();
    return error_code::OK;
  }

  if (this->arr == nullptr) {
    TRY(this->allocate(rhs.capacity));
  } else if (this->capacity < rhs.capacity) {
    TRY(this->reallocate(rhs.capacity));
  }

  for (usize i = 0U; i < rhs.top; ++i) {
    if constexpr (std::is_copy_assignable<value>::value) {
      this->arr[i] = rhs.arr[i]; // Copyable
    } else {
      TRY(this->arr[i].copy(rhs.arr[i])); // Custom Object
    }
  }

  this->top = rhs.top;
  return error_code::OK;
}

// === Move ===
template <typename Derived, typename T>
base_vector<Derived, T>::base_vector(base_vector&& rhs) noexcept {
  this->arr = rhs.arr;
  this->top = rhs.top;
  this->capacity = rhs.capacity;

  rhs.arr = nullptr;
  rhs.capacity = rhs.top = 0;
}

template <typename Derived, typename T>
base_vector<Derived, T>& base_vector<Derived, T>::operator=(base_vector&& rhs
) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->destroy();

  this->arr = rhs.arr;
  this->top = rhs.top;
  this->capacity = rhs.capacity;

  rhs.arr = nullptr;
  rhs.capacity = rhs.top = 0;

  return *this;
}

// === Destructor ===
template <typename Derived, typename T>
void base_vector<Derived, T>::destroy() noexcept {
  if (this->arr) {
#if DS_TEST
    ds_test::free_ptr = this->arr;
#endif

    if constexpr (std::is_class<value>::value) {
      for (usize i = 0U; i < this->capacity; ++i) {
        this->arr[i].~T();
      }
    }

    // NOLINTNEXTLINE
    std::free(this->arr);
    this->arr = nullptr;
  }
}

template <typename Derived, typename T>
base_vector<Derived, T>::~base_vector() {
  this->destroy();
}

// === Memory ===
template <typename Derived, typename T>
error_code base_vector<Derived, T>::allocate(usize size) noexcept {
  assert(size > 0U);

  // NOLINTNEXTLINE
  this->arr = static_cast<ptr>(std::malloc(size * sizeof(T)));
  if (this->arr == nullptr) {
    return error_code::BAD_ALLOCATION;
  }

  // Data initialization
  if constexpr (std::is_class<value>::value) {
    new (this->arr) T[size];
  } else {
    std::memset(this->arr, 0, size * sizeof(T));
  }

  this->capacity = size;
  return error_code::OK;
}

template <typename Derived, typename T>
error_code base_vector<Derived, T>::reallocate(usize size) noexcept {
  assert(size >= this->capacity);

  // Free the data
  if constexpr (std::is_class<value>::value) {
    for (usize i = size; i < this->top; ++i) {
      this->arr[i].~T();
    }
  }

  // NOLINTNEXTLINE
  void* _ptr = std::realloc(this->arr, size * sizeof(T));
  if (_ptr == nullptr) {
    return error_code::BAD_ALLOCATION;
  }
  this->arr = static_cast<ptr>(_ptr);

  if constexpr (std::is_class<value>::value) {
    for (usize i = this->capacity; i < size; ++i) {
      new (this->arr + i) T;
    }
  } else {
    if (size > this->capacity) {
      std::memset(
          this->arr + this->capacity, 0, (size - this->capacity) * sizeof(T)
      );
    }
  }

  this->capacity = size;
  return error_code::OK;
}

template <typename Derived, typename T>
error_code base_vector<Derived, T>::grow(usize min_size) noexcept {
  if (min_size < 1)
    min_size = 1;

  if (this->arr == nullptr) {
    this->top = 0;
    return this->allocate(min_size);
  }

  if (min_size <= this->capacity) {
    return error_code::OK;
  }

  usize new_size = std::max((usize)1ULL, this->capacity) * 2;
  while (new_size < min_size) {
    new_size *= 2;
  }

  return this->reallocate(new_size);
}

template <typename Derived, typename T>
void base_vector<Derived, T>::shift(usize start, usize n) noexcept {
  --start;
  for (usize i = this->top - 1U; i != start; i--) {
    this->arr[i + n] = std::move(this->arr[i]);
  }
  this->top += n;
}

// === Element Access ===
template <typename Derived, typename T>
expected<T*, error_code> base_vector<Derived, T>::at(usize index
) const noexcept {
  if (index >= this->top) {
    return unexpected{error_code::INDEX_OUT_OF_BOUNDS};
  }

  return this->arr + index;
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::ref
base_vector<Derived, T>::operator[](usize index) const noexcept {
  assert(index < this->top);
  return this->arr[index];
}

template <typename Derived, typename T>
expected<T*, error_code> base_vector<Derived, T>::front() const noexcept {
  if (this->is_empty()) {
    return unexpected{error_code::CONTAINER_EMPTY};
  }

  return this->arr;
}

template <typename Derived, typename T>
expected<T*, error_code> base_vector<Derived, T>::back() const noexcept {
  if (this->is_empty()) {
    return unexpected{error_code::CONTAINER_EMPTY};
  }

  return this->arr + this->top - 1;
}

template <typename Derived, typename T>
T& base_vector<Derived, T>::front_unsafe() const noexcept {
  return *this->arr;
}

template <typename Derived, typename T>
T& base_vector<Derived, T>::back_unsafe() const noexcept {
  return this->arr[this->top - 1];
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::ptr
base_vector<Derived, T>::data() const noexcept {
  return this->arr;
}

// === Iterators ===
template <typename Derived, typename T>
typename base_vector<Derived, T>::iterator
base_vector<Derived, T>::begin() noexcept {
  return iterator(this->arr);
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::citerator
base_vector<Derived, T>::cbegin() const noexcept {
  return citerator(this->arr);
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::iterator
base_vector<Derived, T>::end() noexcept {
  return iterator(this->arr + this->top);
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::citerator
base_vector<Derived, T>::cend() const noexcept {
  return citerator(this->arr + this->top);
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::iterator
base_vector<Derived, T>::rbegin() noexcept {
  return iterator(this->arr + this->top - 1);
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::citerator
base_vector<Derived, T>::crbegin() const noexcept {
  return iterator(this->arr + this->top - 1);
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::iterator
base_vector<Derived, T>::rend() noexcept {
  return iterator(this->arr - 1);
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::citerator
base_vector<Derived, T>::crend() const noexcept {
  return iterator(this->arr - 1);
}

// === Capacity ===
template <typename Derived, typename T>
bool base_vector<Derived, T>::is_empty() const noexcept {
  return this->top == 0;
}

template <typename Derived, typename T>
usize base_vector<Derived, T>::get_size() const noexcept {
  return this->top;
}

template <typename Derived, typename T>
usize base_vector<Derived, T>::get_capacity() const noexcept {
  return this->capacity;
}

template <typename Derived, typename T>
error_code base_vector<Derived, T>::reserve(usize size) noexcept {
  if (this->arr == nullptr) {
    return this->allocate(size);
  }

  if (size <= this->capacity) {
    return error_code::OK;
  }

  TRY(this->reallocate(size));
  if (this->top > this->capacity)
    this->top = size;

  return error_code::OK;
}

// === Modifiers === //
template <typename Derived, typename T>
void base_vector<Derived, T>::clear() noexcept {
  if constexpr (std::is_class<value>::value) {
    for (usize i = 0U; i < this->top; ++i) {
      this->arr[i].~T();
    }
  }

  this->top = 0;
}

// * Insert * //
template <typename Derived, typename T>
template <typename... Args>
error_code base_vector<Derived, T>::insert_helper(
    usize index, cref first, Args&&... args
) noexcept {
  if constexpr (std::is_class<value>::value &&
                !std::is_copy_assignable<value>::value) {
    TRY(this->arr[index].copy(first));
  } else {
    this->arr[index] = first;
  }

  if constexpr (sizeof...(Args)) {
    return this->insert_helper(index + 1, std::forward<Args>(args)...);
  } else {
    return error_code::OK;
  }
}

template <typename Derived, typename T>
template <typename... Args>
error_code base_vector<Derived, T>::insert_helper(
    usize index, rref first, Args&&... args
) noexcept {
  this->arr[index] = std::move(first);

  if constexpr (sizeof...(Args)) {
    return this->insert_helper(index + 1, std::forward<Args>(args)...);
  } else {
    return error_code::OK;
  }
}

// * Push Back * //
template <typename Derived, typename T>
template <typename... Args>
error_code
base_vector<Derived, T>::push_back_helper(cref first, Args&&... args) noexcept {
  if constexpr (std::is_copy_assignable<value>::value) {
    this->arr[this->top] = first;
  } else {
    TRY(this->arr[this->top].copy(first));
  }
  ++this->top;

  if constexpr (sizeof...(Args)) {
    return this->push_back_helper(std::forward<Args>(args)...);
  } else {
    return error_code::OK;
  }
}

template <typename Derived, typename T>
template <typename... Args>
error_code
base_vector<Derived, T>::push_back_helper(rref first, Args&&... args) noexcept {
  this->arr[this->top++] = std::move(first);

  if constexpr (sizeof...(Args)) {
    return this->push_back_helper(std::forward<Args>(args)...);
  } else {
    return error_code::OK;
  }
}

// * End Push Back * //

template <typename Derived, typename T>
error_code base_vector<Derived, T>::resize(usize size) noexcept {
  if (this->arr == nullptr) {
    TRY(this->allocate(size));
    this->top = size;
    return error_code::OK;
  }

  if (size > this->capacity) {
    TRY(this->reallocate(size));
  }
  this->top = size;

  return error_code::OK;
}

template <typename Derived, typename T>
void base_vector<Derived, T>::erase_impl(usize index) noexcept {
  if (index < 0 || index >= this->top) {
    return;
  }

  // Unshift
  --this->top;
  while (index < this->top) {
    this->arr[index] = std::move(this->arr[index + 1]);
    index++;
  }
}

template <typename Derived, typename T>
expected<T, error_code> base_vector<Derived, T>::pop_back() noexcept {
  if (this->is_empty()) {
    return error_code::CONTAINER_EMPTY;
  }

  return std::move(this->arr[--this->top]);
}

template <typename Derived, typename T>
T&& base_vector<Derived, T>::pop_back_unsafe() noexcept {
  return std::move(this->arr[--this->top]);
}

template <typename Derived, typename T>
void base_vector<Derived, T>::pop_back_disc() noexcept {
  --this->top;
}

// === Operators / Non-member functions ===
template <typename Derived, typename T>
bool operator==(
    const base_vector<Derived, T>& lhs, const base_vector<Derived, T>& rhs
) noexcept {
  if (lhs.top != rhs.top)
    return false;

  for (usize i = 0U; i < lhs.top; ++i) {
    if (lhs.arr[i] != rhs.arr[i])
      return false;
  }

  return true;
}

template <typename Derived, typename T>
bool operator!=(
    const base_vector<Derived, T>& lhs, const base_vector<Derived, T>& rhs
) noexcept {
  return !(lhs == rhs);
}

} // namespace ds

#endif
