/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_UNIQUE_PTR_TPP
#define DS_UNIQUE_PTR_TPP

#include "./unique_ptr.hpp"
#include "types.hpp"
#include <cstdlib>
#include <type_traits>

namespace ds {

// === Move === //

template <typename T>
unique_ptr<T>::unique_ptr(unique_ptr&& rhs) noexcept : data(rhs.data) {
  rhs.data = nullptr;
}

template <typename T>
unique_ptr<T>& unique_ptr<T>::operator=(unique_ptr&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->reset();
  this->data = rhs.data;
  rhs.data = nullptr;

  return *this;
}

// === Destructor === //

template <typename T> unique_ptr<T>::~unique_ptr() noexcept {
  this->reset();
}

template <typename T> void unique_ptr<T>::reset() noexcept {
  if (this->data == nullptr) {
    return;
  }

  if constexpr (std::is_class<T>::value) {
    this->data->~T();
  }
  std::free(this->data); // NOLINT
  this->data = nullptr;
}

// === Modifiers === //

template <typename T> error_code unique_ptr<T>::set(const T& data) noexcept {
  if (this->data) {
    if constexpr (std::is_class<T>::value) {
      TRY(this->data->copy(data));
    } else {
      *this->data = data;
    }

    return error_code::OK;
  }

  this->data = (T*)std::malloc(sizeof(T)); // NOLINT
  if (this->data == nullptr) {
    return error_code::BAD_ALLOCATION;
  }

  if constexpr (std::is_class<T>::value) {
    new (this->data) T();

    error_code error = this->data->copy(data);
    if (is_error(error)) {
      this->data.~T();
      std::free(this->data); // NOLINT
      this->data = nullptr;
      return error;
    }
  } else {
    *this->data = data;
  }

  return error_code::OK;
}

template <typename T> error_code unique_ptr<T>::set(T&& data) noexcept {
  if (this->data == nullptr) {
    this->data = (T*)std::malloc(sizeof(T)); // NOLINT
    if (this->data == nullptr) {
      return error_code::BAD_ALLOCATION;
    }

    if constexpr (std::is_class<T>::value) {
      new (this->data) T();
    }
  }

  *this->data = std::move(data);
  return error_code::OK;
}

template <typename T> T unique_ptr<T>::release() noexcept {
  auto* tmp = this->data;
  this->data = nullptr;
  return std::move(*tmp);
}

// === Observers === //

template <typename T> T* unique_ptr<T>::get_ptr() const noexcept {
  return this->data;
}

template <typename T> T* unique_ptr<T>::operator->() const noexcept {
  return this->data;
}

template <typename T> T& unique_ptr<T>::operator*() const& noexcept {
  return *this->data;
}

template <typename T> constexpr unique_ptr<T>::operator bool() const noexcept {
  return this->data != nullptr;
}

} // namespace ds

#endif
