/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-12-30
 *===============================*/

#ifndef DS_VECTOR_IMPL_HPP
#define DS_VECTOR_IMPL_HPP

#include "./def.hpp"

// Implementation
namespace ds {

// === Copy ===
template <typename Derived, typename T>
opt_err base_vector<Derived, T>::copy(const base_vector& rhs) noexcept {
  if (&rhs == this) {
    return null;
  }

  if (!this->arr) {
    try_opt(this->allocate(rhs._max_size));
  } else if (this->_max_size < rhs._max_size) {
    try_opt(this->reallocate(rhs._max_size));
  }

  for (i32 i = 0; i < rhs.top; ++i) {
    if constexpr (std::is_class<value>::value) {
      try_opt(this->arr[i].copy(rhs.arr[i])); // Custom Object
    } else {
      this->arr[i] = rhs.arr[i]; // Primitive
    }
  }

  this->top = rhs.top;

  return null;
}

// === Move ===
template <typename Derived, typename T>
base_vector<Derived, T>::base_vector(base_vector&& rhs) noexcept {
  this->arr = rhs.arr;
  this->top = rhs.top;
  this->_max_size = rhs._max_size;

  rhs.arr = nullptr;
  rhs._max_size = rhs.top = 0;
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
  this->_max_size = rhs._max_size;

  rhs.arr = nullptr;
  rhs._max_size = rhs.top = 0;

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
      for (i32 i = 0; i < this->_max_size; ++i) {
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
opt_err base_vector<Derived, T>::allocate(i32 size) noexcept {
  // Min size is 1
  if (size < 1)
    size = 1;

  // NOLINTNEXTLINE
  this->arr = static_cast<ptr>(std::malloc(size * sizeof(T)));
  if (this->arr == nullptr) {
    return error{VECTOR_BAD_ALLOC};
  }

  // Data initialization
  if constexpr (std::is_class<value>::value) {
    new (this->arr) T[size];
  } else {
    std::memset(this->arr, 0, size * sizeof(T));
  }

  this->_max_size = size;
  return null;
}

template <typename Derived, typename T>
opt_err base_vector<Derived, T>::reallocate(i32 size) noexcept {
  if (size < 1)
    size = 1;

  // Free the data
  if constexpr (std::is_class<value>::value) {
    for (i32 i = size; i < this->top; ++i) {
      this->arr[i].~T();
    }
  }

  // NOLINTNEXTLINE
  void* _ptr = std::realloc(this->arr, size * sizeof(T));
  if (_ptr == nullptr) {
    return error{
        VECTOR_BAD_ALLOC,
    };
  }
  this->arr = static_cast<ptr>(_ptr);

  if constexpr (std::is_class<value>::value) {
    for (i32 i = this->_max_size; i < size; ++i) {
      new (this->arr + i) T();
    }
  } else {
    if (size > this->_max_size) {
      std::memset(
          this->arr + this->_max_size, 0, (size - this->_max_size) * sizeof(T)
      );
    }
  }

  this->_max_size = size;

  return null;
}

template <typename Derived, typename T>
opt_err base_vector<Derived, T>::grow(i32 min_size) noexcept {
  if (min_size < 1)
    min_size = 1;

  if (this->arr == nullptr) {
    this->top = 0;
    return this->allocate(min_size);
  }

  if (min_size <= this->_max_size) {
    return null;
  }

  // TODO: Optimization for K = 2
  i32 new_size = std::max(1, this->_max_size) * this->K;
  while (new_size < min_size) {
    new_size *= this->K;
  }

  return this->reallocate(new_size);
}

template <typename Derived, typename T>
void base_vector<Derived, T>::shift(i32 start, i32 n) noexcept {
  for (i32 i = this->top - 1; i >= start; i--) {
    this->arr[i + n] = std::move(this->arr[i]);
  }
  this->top += n;
}

// === Element Access ===
template <typename Derived, typename T>
exp_ptr_err<T> base_vector<Derived, T>::at(i32 index) const noexcept {
  if (index < 0 || index >= this->top) {
    return unexpected(error{VECTOR_OUT_OF_RANGE});
  }

  return this->arr + index;
}

template <typename Derived, typename T>
typename base_vector<Derived, T>::ref
base_vector<Derived, T>::operator[](i32 index) const noexcept {
  return this->arr[index];
}

template <typename Derived, typename T>
exp_ptr_err<T> base_vector<Derived, T>::front() const noexcept {
  if (this->empty()) {
    return unexpected(error{VECTOR_EMPTY});
  }

  return this->arr;
}

template <typename Derived, typename T>
exp_ptr_err<T> base_vector<Derived, T>::back() const noexcept {
  if (this->empty()) {
    return unexpected(error{VECTOR_EMPTY});
  }

  return this->arr + this->top - 1;
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
bool base_vector<Derived, T>::empty() const noexcept {
  return this->top == 0;
}

template <typename Derived, typename T>
i32 base_vector<Derived, T>::size() const noexcept {
  return this->top;
}

template <typename Derived, typename T>
i32 base_vector<Derived, T>::max_size() const noexcept {
  return this->_max_size;
}

template <typename Derived, typename T>
opt_err base_vector<Derived, T>::reserve(i32 size) noexcept {
  if (this->arr == nullptr) {
    return this->allocate(size);
  }

  if (size <= this->_max_size) {
    return null;
  }

  try_opt(this->reallocate(size));
  if (this->top > this->_max_size)
    this->top = size;

  return null;
}

// === Modifiers === //
template <typename Derived, typename T>
void base_vector<Derived, T>::clear() noexcept {
  if constexpr (std::is_class<value>::value) {
    for (i32 i = 0; i < this->top; ++i) {
      this->arr[i].~T();
    }
  }

  this->top = 0;
}

// * Insert * //
template <typename Derived, typename T>
template <typename... Args>
opt_err base_vector<Derived, T>::insert_helper(
    i32 index, cref first, Args&&... args
) noexcept {
  if constexpr (std::is_class<value>::value) {
    try_opt(this->arr[index].copy(first));
  } else {
    this->arr[index] = first;
  }

  if constexpr (sizeof...(Args)) {
    return this->insert_helper(index + 1, std::forward<Args>(args)...);
  } else {
    return null;
  }
}

template <typename Derived, typename T>
template <typename... Args>
opt_err base_vector<Derived, T>::insert_helper(
    i32 index, rref first, Args&&... args
) noexcept {
  this->arr[index] = std::move(first);

  if constexpr (sizeof...(Args)) {
    return this->insert_helper(index + 1, std::forward<Args>(args)...);
  } else {
    return null;
  }
}

// * Push Back * //
template <typename Derived, typename T>
template <typename... Args>
opt_err
base_vector<Derived, T>::push_back_helper(cref first, Args&&... args) noexcept {
  if constexpr (std::is_class<value>::value) {
    try_opt(this->arr[this->top].copy(first));
  } else {
    this->arr[this->top] = first;
  }
  ++this->top;

  if constexpr (sizeof...(Args)) {
    return this->push_back_helper(std::forward<Args>(args)...);
  } else {
    return null;
  }
}

template <typename Derived, typename T>
template <typename... Args>
opt_err
base_vector<Derived, T>::push_back_helper(rref first, Args&&... args) noexcept {
  this->arr[this->top++] = std::move(first);

  if constexpr (sizeof...(Args)) {
    return this->push_back_helper(std::forward<Args>(args)...);
  } else {
    return null;
  }
}

// * End Push Back * //

template <typename Derived, typename T>
opt_err base_vector<Derived, T>::resize(i32 size) noexcept {
  if (this->arr == nullptr) {
    try_opt(this->allocate(size));
    this->top = size;
    return null;
  }

  if (size > this->_max_size) {
    try_opt(this->reallocate(size));
  }
  this->top = size;

  return null;
}

template <typename Derived, typename T>
void base_vector<Derived, T>::erase_impl(i32 index) noexcept {
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
exp_err<T> base_vector<Derived, T>::pop_back() noexcept {
  if (this->empty()) {
    return unexpected{error{VECTOR_EMPTY, def_err_vals}};
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

  for (i32 i = 0; i < lhs.top; ++i) {
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

