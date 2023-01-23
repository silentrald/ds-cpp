/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 * Updated: 2023-01-20
 *===============================*/

#ifndef DS_UNIQUE_PTR_TPP
#define DS_UNIQUE_PTR_TPP

#include "../macro.hpp"
#include "../type_traits.hpp"
#include "./unique_ptr.hpp"
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
template <typename T> opt_err unique_ptr<T>::set(cref data) noexcept {
  if (!this->data) {
    this->data = new (std::nothrow) T(); // NOLINT
    if (this->data == nullptr) {
      return error{UPTR_BAD_ALLOC, def_err_vals};
    }

    if constexpr (has_copy_method<value>::value) {
      auto err = this->data->copy(data);
      if (err) {
        delete this->data;
        this->data = nullptr;
        return err;
      }
      return null;
    }
  }

  if constexpr (has_copy_method<value>::value) {
    try_opt(this->data->copy(data));
  } else {
    *this->data = data;
  }

  return null;
}

template <typename T> opt_err unique_ptr<T>::set(rref data) noexcept {
  if (this->data) {
    *this->data = std::move(data);
    return null;
  }

  this->data = new (std::nothrow) T(); // NOLINT
  if (this->data == nullptr) {
    return error{UPTR_BAD_ALLOC, def_err_vals};
  }

  *this->data = std::move(data);
  return null;
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

