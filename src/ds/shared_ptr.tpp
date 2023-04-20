/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_SHARED_PTR_TPP
#define DS_SHARED_PTR_TPP

#include "./shared_ptr.hpp"
#include "ds-error/types.hpp"
#include "ds/macro.hpp"
#include "ds/types.hpp"
#include <type_traits>

namespace ds {

// === Copy === //
template <typename T>
err_code shared_ptr<T>::copy(const shared_ptr& other) noexcept {
  if (&other == this || this->data == other.data) {
    return ec::SUCCESS;
  }

  this->destroy();
  if (other.data == nullptr) {
    return ec::SUCCESS;
  }

  this->data = other.data;
  this->counter = other.counter;
  ++(*this->counter);

  return ec::SUCCESS;
}

// === Move === //
template <typename T>
shared_ptr<T>::shared_ptr(shared_ptr&& rhs) noexcept
    : data(rhs.data), counter(rhs.counter) {
  rhs.data = nullptr;
  rhs.counter = nullptr;
}

template <typename T>
shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr<T>&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  this->data = rhs.data;
  this->counter = rhs.counter;

  rhs.data = nullptr;
  rhs.counter = nullptr;

  return *this;
}

// === Memory === //
template <typename T> err_code shared_ptr<T>::allocate() noexcept {
  this->data = new (std::nothrow) T(); // NOLINT
  if (this->data == nullptr) {
    return ec::BAD_ALLOC;
  }

  this->counter = new int(1); // NOLINT
  if (this->counter == nullptr) {
    this->destroy_data_ptr();
    return ec::BAD_ALLOC;
  }

  return ec::SUCCESS;
}

// === Destructor === //
template <typename T> void shared_ptr<T>::destroy_data_ptr() noexcept {
#if DS_TEST
  ds_test::free_ptr = this->data;
#endif

  delete this->data;
  this->data = nullptr;
}

template <typename T> void shared_ptr<T>::destroy_counter_ptr() noexcept {
  delete this->counter;
  this->counter = nullptr;
}

template <typename T> void shared_ptr<T>::destroy_all_ptrs() noexcept {
  this->destroy_data_ptr();
  this->destroy_counter_ptr();
}

template <typename T> void shared_ptr<T>::destroy() noexcept {
  if (this->data == nullptr) {
    return;
  }

  if (--(*this->counter)) {
    this->data = nullptr;
    this->counter = nullptr;
    return;
  }

  this->destroy_all_ptrs();
}

template <typename T> shared_ptr<T>::~shared_ptr() noexcept {
  this->destroy();
}

// === Modifiers === //
template <typename T> err_code shared_ptr<T>::init() noexcept {
  if (this->data) {
    return ec::ALREADY_SET;
  }

  return this->allocate();
}

template <typename T>
template <typename Data_>
err_code shared_ptr<T>::set_impl(Data_ data) noexcept {
  if (this->data) {
    this->destroy();
  }

  try_err_code(this->allocate());

  if constexpr (std::is_rvalue_reference<Data_>::value) {
    *this->data = std::move(data);
  } else if constexpr (std::is_class<value>::value) {
    if (this->data->copy(data)) {
      this->destroy_all_ptrs();
      return ec::BAD_ALLOC;
    }
  } else {
    *this->data = data;
  }

  return ec::SUCCESS;
}

template <typename T> void shared_ptr<T>::release() noexcept {
  this->destroy();
}

// === Observers === //
template <typename T>
typename shared_ptr<T>::ptr shared_ptr<T>::get() const noexcept {
  return this->data;
}

template <typename T>
typename shared_ptr<T>::ref shared_ptr<T>::operator*() const noexcept {
  return *this->data;
}

template <typename T>
typename shared_ptr<T>::ptr shared_ptr<T>::operator->() const noexcept {
  return this->data;
}

template <typename T> i32 shared_ptr<T>::count() const noexcept {
  return this->counter != nullptr ? *this->counter : 0;
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

