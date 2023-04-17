/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_UNIQUE_PTR_TPP
#define DS_UNIQUE_PTR_TPP

#include "./unique_ptr.hpp"
#include "ds/macro.hpp"
#include "ds/type_traits.hpp"
#include "ds/types.hpp"
#include <new>

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

  this->destroy();
  this->data = rhs.data;
  rhs.data = nullptr;

  return *this;
}

// === Destructor === //
template <typename T> unique_ptr<T>::~unique_ptr() noexcept {
  this->destroy();
}

template <typename T> void unique_ptr<T>::destroy() noexcept {
  if (this->data) {
    delete this->data;
    this->data = nullptr;
  }
}

// === Modifiers === //
template <typename T> err_code unique_ptr<T>::init() noexcept {
  if (this->data) {
    return ec::ALREADY_SET;
  }

  this->data = new (std::nothrow) T(); // NOLINT
  if (this->data == nullptr) {
    return ec::BAD_ALLOC;
  }
  return ec::SUCCESS;
}

template <typename T> err_code unique_ptr<T>::set(ptr data) noexcept {
  if (this->data) {
    return ec::ALREADY_SET;
  }

  this->data = data;
  return ec::SUCCESS;
}

template <typename T> err_code unique_ptr<T>::set(cref data) noexcept {
  if (this->data) {
    return ec::ALREADY_SET;
  }

  this->data = new (std::nothrow) T(); // NOLINT
  if (this->data == nullptr) {
    return ec::BAD_ALLOC;
  }

  if constexpr (has_copy_method<value>::value) {
    auto err = this->data->copy(data);
    if (err) {
      delete this->data;
      this->data = nullptr;
      return err;
    }
  } else {
    *this->data = data;
  }

  return ec::SUCCESS;
}

template <typename T> err_code unique_ptr<T>::set(rref data) noexcept {
  if (this->data) {
    return ec::ALREADY_SET;
  }

  this->data = new (std::nothrow) T(); // NOLINT
  if (this->data == nullptr) {
    return ec::BAD_ALLOC;
  }

  *this->data = std::move(data);
  return ec::SUCCESS;
}

template <typename T> T* unique_ptr<T>::release() noexcept {
  auto* tmp = this->data;
  this->data = nullptr;
  return tmp;
}

template <typename T> void unique_ptr<T>::reset() noexcept {
  this->destroy();
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

