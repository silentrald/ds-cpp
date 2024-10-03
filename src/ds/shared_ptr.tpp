/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_SHARED_PTR_TPP
#define DS_SHARED_PTR_TPP

#include "./shared_ptr.hpp"
#include "types.hpp"
#include <cstdlib>
#include <type_traits>

namespace ds {

// === Copy === //

template <typename T>
error_code shared_ptr<T>::copy(const shared_ptr& other) noexcept {
  if (&other == this || this->data == other.data) {
    return error_code::OK;
  }

  this->reset();
  if (other.data == nullptr) {
    return error_code::OK;
  }

  this->data = other.data;
  ++(this->data->count);

  return error_code::OK;
}

// === Move === //

template <typename T>
shared_ptr<T>::shared_ptr(shared_ptr&& rhs) noexcept : data(rhs.data) {
  rhs.data = nullptr;
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr<T>&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->data = rhs.data;
  rhs.data = nullptr;

  return *this;
}

// === Memory === //

template <typename T> error_code shared_ptr<T>::allocate() noexcept {
  this->data = (Data*)std::malloc(sizeof(Data)); // NOLINT
  if (this->data == nullptr) {
    return error_code::BAD_ALLOCATION;
  }

  this->data->count = 1U;
  return error_code::OK;
}

// === Destructor === //

template <typename T> void shared_ptr<T>::reset() noexcept {
  if (this->data == nullptr) {
    return;
  }

  if (--(this->data->count) == 0) {
    if constexpr (std::is_class<T>::value) {
      this->data->val.~T();
    }
    std::free(this->data); // NOLINT
  }
  this->data = nullptr;
}

template <typename T> shared_ptr<T>::~shared_ptr() noexcept {
  this->reset();
}

// === Modifiers === //

template <typename T>
template <typename Data_>
error_code shared_ptr<T>::set_impl(Data_ data) noexcept {
  if (this->data != nullptr) {
    this->reset();
  }

  TRY(this->allocate());

  if constexpr (std::is_rvalue_reference<Data_>::value) {
    this->data->val = std::move(data);
  } else if constexpr (std::is_copy_assignable<value>::value) {
    this->data->val = data;
  } else {
    if (is_error(this->data->val.copy(data))) {
      std::free(this->data); // NOLINT
      this->data = nullptr;

      return error_code::BAD_ALLOCATION;
    }
  }

  return error_code::OK;
}

// === Observers === //

template <typename T> T* shared_ptr<T>::get() const noexcept {
  return (T*)this->data;
}

template <typename T> T& shared_ptr<T>::operator*() const noexcept {
  return *(T*)this->data;
}

template <typename T> T* shared_ptr<T>::operator->() const noexcept {
  return (T*)this->data;
}

template <typename T> usize shared_ptr<T>::get_count() const noexcept {
  return this->data == nullptr ? 0 : this->data->count;
}

template <typename T> shared_ptr<T>::operator bool() const noexcept {
  return this->data != nullptr;
}

// === Non-member Operators === //

template <typename T>
bool operator==(const shared_ptr<T>& lhs, const shared_ptr<T>& rhs) noexcept {
  return lhs.data == rhs.data;
}

template <typename T>
bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<T>& rhs) noexcept {
  return lhs.data != rhs.data;
}

} // namespace ds

#endif
