/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-12-30
 *===============================*/

#ifndef DS_FIXED_DEQUE_IMPL_HPP
#define DS_FIXED_DEQUE_IMPL_HPP

#include "./def.hpp"
#include "ds/types.hpp"
#include <type_traits>

namespace ds {

// === Memory ===
template <typename T>
opt_error fixed_deque<T>::allocate(i32 capacity) noexcept {
  // NOLINTNEXTLINE
  this->arr = static_cast<pointer>(std::malloc((capacity + 1) * sizeof(T)));
  if (this->arr == nullptr) {
    return error{FDEQUE_BAD_ALLOC, def_err_vals};
  }

  this->_capacity = capacity;
  return null;
}

template <typename T>
opt_error fixed_deque<T>::reallocate(i32 capacity) noexcept {
  if (this->_capacity == capacity) {
    return null;
  }

  // Just resize the data
  if (this->empty() || this->head == 0) {
    return this->reallocate_grow(capacity);
  }

  if (this->head <= this->tail) {
    return this->reallocate_shift(capacity);
  }

  return this->reallocate_move(capacity);
}

template <typename T>
opt_error fixed_deque<T>::reallocate_grow(i32 capacity) noexcept {
  if constexpr (std::is_class<value>::value) {
    for (i32 i = capacity + 1; i <= this->_capacity; ++i) {
      this->arr[i].~T();
    }
  }

  // NOLINTNEXTLINE
  void* ptr = std::realloc(this->arr, (capacity + 1) * sizeof(T));
  if (ptr == nullptr) {
    return error{FDEQUE_BAD_ALLOC, def_err_vals};
  }
  this->arr = static_cast<pointer>(ptr);

  if constexpr (std::is_class<value>::value) {
    for (i32 i = this->_capacity + 1; i <= capacity; ++i) {
      new (this->arr + i) T;
    }
  }

  this->_capacity = capacity;

  return null;
}

// Shift the data
// +------+    +------+
// |__HxT_| -> |HxT___|
// +------+    +------+
template <typename T>
opt_error fixed_deque<T>::reallocate_shift(i32 capacity) noexcept {
  i32 i = 0;
  for (i32 j = this->head; j <= this->tail; ++i, ++j) {
    this->arr[i] = std::move(this->arr[j]);
  }
  this->head = 0;
  this->tail = i > capacity ? capacity - 1 : i - 1;

  if constexpr (std::is_class<value>::value) {
    for (i = capacity + 1; i <= this->_capacity; ++i) {
      this->arr[i].~T();
    }
  }

  // NOLINTNEXTLINE
  void* ptr = std::realloc(this->arr, (capacity + 1) * sizeof(T));
  if (ptr == nullptr) {
    return error{FDEQUE_BAD_ALLOC, def_err_vals};
  }
  this->arr = static_cast<pointer>(ptr);

  if constexpr (std::is_class<value>::value) {
    for (i = this->_capacity + 1; i <= capacity; ++i) {
      new (this->arr + i) T;
    }
  }

  this->_capacity = capacity;

  return null;
}

// Move the data
// +-----+    +---------+
// |xT_Hx| -> |HxxT_____|
// +-----+    +---------+
// +-------+    +-----+
// |xT___Hx| -> |HxxT_|
// +-------+    +-----+
// Move to the new container
template <typename T>
opt_error fixed_deque<T>::reallocate_move(i32 capacity) noexcept {
  // NOLINTNEXTLINE
  pointer ptr = static_cast<pointer>(std::malloc((capacity + 1) * sizeof(T)));
  if (ptr == nullptr) {
    return error{FDEQUE_BAD_ALLOC, def_err_vals};
  }

  i32 new_i = 0;
  i32 orig_i = this->head;
  while (true) {
    if constexpr (std::is_class<value>::value) {
      new (ptr + new_i) T(std::move(this->arr[orig_i]));
    } else {
      ptr[new_i] = this->arr[orig_i];
    }

    if (orig_i == this->tail) {
      break;
    }

    if (++new_i == capacity) {
      --new_i;
      break;
    }

    if (++orig_i > this->_capacity) {
      orig_i = 0;
    }
  }

  if constexpr (std::is_class<value>::value) {
    // Initialize the rest
    i32 i = 0;
    for (i = new_i + 1; i <= capacity; ++i) {
      new (ptr + i) T;
    }

    // Delete the remaining data from the first array
    for (i = 0; i < this->head; ++i) {
      this->arr[i].~T();
    }
    for (i = orig_i + 1; i <= this->_capacity; ++i) {
      this->arr[i].~T();
    }
  }

  // NOLINTNEXTLINE
  std::free(this->arr);
  this->arr = ptr;
  this->head = 0;
  this->tail = new_i;
  this->_capacity = capacity;

  return null;
}

// === Copy ===
template <typename T>
opt_error fixed_deque<T>::copy_center(const fixed_deque& rhs) noexcept {
  for (i32 i = this->head; i <= this->tail; ++i) {
    if constexpr (std::is_class<value>::value) {
      try_opt(this->arr[i].copy(rhs.arr[i]));
    } else {
      this->arr[i] = rhs.arr[i];
    }
  }

  return null;
}

template <typename T>
opt_error fixed_deque<T>::copy_ends(const fixed_deque& rhs) noexcept {
  i32 i = 0;
  for (; i <= this->tail; ++i) {
    if constexpr (std::is_class<value>::value) {
      try_opt(this->arr[i].copy(rhs.arr[i]));
    } else {
      this->arr[i] = rhs.arr[i];
    }
  }

  for (i = this->head; i <= this->_capacity; ++i) {
    if constexpr (std::is_class<value>::value) {
      try_opt(this->arr[i].copy(rhs.arr[i]));
    } else {
      this->arr[i] = rhs.arr[i];
    }
  }

  return null;
}

template <typename T>
opt_error fixed_deque<T>::copy(const fixed_deque& other) noexcept {
  if (&other == this) {
    return null;
  }

  if (this->_capacity == 0) {
    if (other._capacity == 0) {
      return null;
    }

    try_opt(this->allocate(other._capacity));
    if constexpr (std::is_class<value>::value) {
      new (this->arr) T[this->_capacity + 1];
    }
  } else if (this->_capacity != other._capacity) {
    try_opt(this->reallocate(other._capacity));
  }

  // Empty
  if (other.empty()) {
    this->clear();
    return null;
  }

  // Copy Contents
  this->head = other.head;
  this->tail = other.tail;

  // +-----------+
  // |__HxxxxT___|
  // +-----------+
  if (other.head <= other.tail) {
    return this->copy_center(std::move(other));
  }

  // +-----------+
  // |xxT___Hxxxx|
  // +-----------+
  return this->copy_ends(std::move(other));
}

// === Move ===
template <typename T> fixed_deque<T>::fixed_deque(fixed_deque&& rhs) noexcept {
  this->arr = rhs.arr;
  this->head = rhs.head;
  this->tail = rhs.tail;
  this->_capacity = rhs._capacity;

  rhs.arr = nullptr;
  rhs.head = -1;
  rhs.tail = 0;
  rhs._capacity = 0;
}

template <typename T>
fixed_deque<T>& fixed_deque<T>::operator=(fixed_deque&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->clear();

  this->arr = rhs.arr;
  this->head = rhs.head;
  this->tail = rhs.tail;
  this->_capacity = rhs._capacity;

  rhs.arr = nullptr;
  rhs.head = -1;
  rhs.tail = 0;
  rhs._capacity = 0;

  return *this;
}

// === Destructor ===
template <typename T> fixed_deque<T>::~fixed_deque() {
  if (this->arr) {
    if constexpr (std::is_class<value>::value) {
      for (i32 i = 0; i <= this->_capacity; ++i) {
        this->arr[i].~T();
      }
    }

    // NOLINTNEXTLINE
    std::free(this->arr);
    this->arr = nullptr;
  }
}

// === Element Access ===
template <typename T> expected_ptr<T> fixed_deque<T>::at(i32 index) noexcept {
  if (this->empty()) {
    return unexpected{error{FDEQUE_EMPTY, def_err_vals}};
  }

  if (index < 0 || index >= this->_capacity) {
    return unexpected{error{FDEQUE_OUT_OF_RANGE, def_err_vals}};
  }

  index += this->head;
  if (index > this->_capacity) {
    index -= this->_capacity + 1;
  }

  return this->arr + index;
}

// Shouldn't pose an error if capacity = 0
template <typename T>
typename fixed_deque<T>::ref fixed_deque<T>::operator[](i32 index) noexcept {
  if (this->empty()) {
    return this->arr[index];
  }

  return this->arr[(this->head + index) % (this->_capacity + 1)];
}

template <typename T> expected_ptr<T> fixed_deque<T>::front() noexcept {
  if (this->empty()) {
    return unexpected{error{FDEQUE_EMPTY, def_err_vals}};
  }

  return this->arr + this->head;
}

template <typename T> expected_ptr<T> fixed_deque<T>::back() noexcept {
  if (this->empty()) {
    return unexpected{error{FDEQUE_EMPTY, def_err_vals}};
  }

  return this->arr + this->tail;
}

// === Iterators ===
template <typename T>
typename fixed_deque<T>::iterator fixed_deque<T>::begin() noexcept {
  return iterator(this->arr, this->head > -1 ? this->head : 1, this->_capacity);
}

template <typename T>
typename fixed_deque<T>::citerator fixed_deque<T>::cbegin() const noexcept {
  return iterator(this->arr, this->head > -1 ? this->head : 1, this->_capacity);
}

template <typename T>
typename fixed_deque<T>::iterator fixed_deque<T>::end() noexcept {
  return iterator(this->arr, this->tail + 1, this->_capacity);
}

template <typename T>
typename fixed_deque<T>::citerator fixed_deque<T>::cend() const noexcept {
  return iterator(this->arr, this->tail + 1, this->_capacity);
}

// === Capacity ===
template <typename T> bool fixed_deque<T>::empty() const noexcept {
  return this->head == -1;
}

template <typename T> bool fixed_deque<T>::full() const noexcept {
  if (this->head == -1) {
    return this->_capacity == 0;
  }

  return (
      this->head - this->tail == 2 ||
      this->head - this->tail + this->_capacity == 1
  );
}

template <typename T> i32 fixed_deque<T>::size() const noexcept {
  if (this->head == -1)
    return 0;

  if (this->head <= this->tail) {
    return this->tail - this->head + 1;
  }
  return this->_capacity - this->head + this->tail + 2;
}

template <typename T> i32 fixed_deque<T>::capacity() const noexcept {
  return this->_capacity;
}

// === Modifiers ===
template <typename T> void fixed_deque<T>::clear() noexcept {
  if (this->empty()) {
    return;
  }

  if constexpr (std::is_class<value>::value) {
    if (this->head <= this->tail) {
      for (i32 i = this->head; i <= this->tail; ++i) {
        this->arr[i].~T();
      }
    } else {
      for (i32 i = 0; i <= this->tail; ++i) {
        this->arr[i].~T();
      }

      for (i32 i = this->head; i <= this->_capacity; ++i) {
        this->arr[i].~T();
      }
    }
  }

  this->head = -1;
  this->tail = 0;
}

template <typename T>
opt_error fixed_deque<T>::push_front(rref element) noexcept {
  if (this->full()) {
    return error{FDEQUE_FULL, def_err_vals};
  }

  if (this->head == -1) {
    this->head = 0;
  } else if (this->head == 0) {
    this->head = this->_capacity;
  } else {
    --this->head;
  }
  this->arr[this->head] = std::move(element);

  return null;
}

template <typename T>
opt_error fixed_deque<T>::push_front(cref element) noexcept {
  if (this->full()) {
    return error{FDEQUE_FULL, def_err_vals};
  }

  if (this->head == -1) {
    this->head = 0;
  } else if (this->head == 0) {
    this->head = this->_capacity;
  } else {
    --this->head;
  }

  if constexpr (std::is_class<value>::value) {
    this->arr[this->head].copy(element);
  } else {
    this->arr[this->head] = element;
  }

  return null;
}

template <typename T>
opt_error fixed_deque<T>::push_back(rref element) noexcept {
  if (this->full()) {
    return error{FDEQUE_FULL, def_err_vals};
  }

  if (this->head == -1) {
    this->head = 0;
  } else if (this->tail == this->_capacity) {
    this->tail = 0;
  } else {
    ++this->tail;
  }

  this->arr[this->tail] = std::move(element);

  return null;
}

template <typename T>
opt_error fixed_deque<T>::push_back(cref element) noexcept {
  if (this->full()) {
    return error{FDEQUE_FULL, def_err_vals};
  }

  if (this->head == -1) {
    this->head = 0;
  } else if (this->tail == this->_capacity) {
    this->tail = 0;
  } else {
    ++this->tail;
  }

  if constexpr (std::is_class<value>::value) {
    try_opt(this->arr[this->tail].copy(element));
  } else {
    this->arr[this->tail] = element;
  }

  return null;
}

template <typename T> expected<T> fixed_deque<T>::pop_front() noexcept {
  if (this->empty()) {
    return unexpected{error{FDEQUE_EMPTY, def_err_vals}};
  }

  T t = std::move(this->arr[this->head]);
  if (this->head == this->tail) {
    this->clear();
    return t;
  }

  if (this->head >= this->_capacity) {
    this->head = 0;
  } else {
    ++this->head;
  }

  return t;
}

template <typename T> opt_error fixed_deque<T>::pop_front_disc() noexcept {
  if (this->empty()) {
    return error{FDEQUE_EMPTY, def_err_vals};
  }

  this->arr[this->head].~T();
  if (this->head == this->tail) {
    this->clear();
    return null;
  }

  if (this->head >= this->_capacity) {
    this->head = 0;
  } else {
    ++this->head;
  }

  return null;
}

template <typename T> expected<T> fixed_deque<T>::pop_back() noexcept {
  if (this->empty()) {
    return unexpected{error{FDEQUE_EMPTY, def_err_vals}};
  }

  T t = std::move(this->arr[this->tail]);
  if (this->head == this->tail) {
    this->clear();
    return t;
  }

  if (this->tail == 0) {
    this->tail = this->_capacity;
  } else {
    --this->tail;
  }

  return t;
}

template <typename T> opt_error fixed_deque<T>::pop_back_disc() noexcept {
  if (this->empty()) {
    return error{FDEQUE_EMPTY, def_err_vals};
  }

  this->arr[this->tail].~T();
  if (this->head == this->tail) {
    this->clear();
    return null;
  }

  if (this->tail == 0) {
    this->tail = this->_capacity;
  } else {
    --this->tail;
  }
  return null;
}

template <typename T> opt_error fixed_deque<T>::remove_front(i32 n) noexcept {
  if (this->empty()) {
    return error{FDEQUE_EMPTY, def_err_vals};
  }

  i32 sz = this->size();
  if (n > sz) {
    return error{FDEQUE_REMOVE, def_err_vals};
  }

  if (n == sz) {
    this->clear();
    return null;
  }

  this->head += n;
  if (this->head > this->_capacity) {
    this->head -= this->_capacity + 1;
  }

  return null;
}

template <typename T> opt_error fixed_deque<T>::remove_back(i32 n) noexcept {
  if (this->empty()) {
    return error{FDEQUE_EMPTY, def_err_vals};
  }

  i32 sz = this->size();
  if (n > sz) {
    return error{FDEQUE_REMOVE, def_err_vals};
  }

  if (n == sz) {
    this->clear();
    return null;
  }

  this->tail -= n;
  if (this->tail < 0) {
    this->tail += this->_capacity + 1;
  }

  return null;
}

template <typename T> opt_error fixed_deque<T>::resize(i32 size) noexcept {
  if (size < 0) {
    return error{FDEQUE_NEGATIVE_CAP, def_err_vals};
  }

  if (this->arr) {
    return this->reallocate(size);
  }

  try_opt(this->allocate(size));
  if constexpr (std::is_class<value>::value) {
    new (this->arr) T[size + 1];
  }
  return null;
}

} // namespace ds

#endif

