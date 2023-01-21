/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 * Updated: 2023-01-20
 *===============================*/

#ifndef DS_OPTIONAL_IMPL_HPP
#define DS_OPTIONAL_IMPL_HPP

#include "./def.hpp"

namespace ds {

// === Move === //
template <typename T> optional<T>::optional(optional&& rhs) noexcept {
  if (!rhs.set) {
    return;
  }

  this->val = std::move(rhs.val);
  this->set = true;
}

template <typename T>
optional<T>& optional<T>::operator=(optional<T>&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  if (!rhs.set) {
    this->destroy();
    return *this;
  }

  this->val = std::move(rhs.val);
  this->set = true;

  return *this;
}

template <typename T>
optional<T>::optional(T&& rhs) noexcept : val(std::move(rhs)), set(true) {}

template <typename T> optional<T>& optional<T>::operator=(T&& rhs) noexcept {
  this->val = std::move(rhs);
  this->set = true;

  return *this;
}

// === Destructor === //
template <typename T> optional<T>::~optional<T>() noexcept {
  this->destroy();
}

template <typename T> void optional<T>::destroy() noexcept {
  if (this->set) {
    this->set = false;
    this->val.~T();
  }
}

// === Null === //
template <typename T> optional<T>::optional(const null_t& rhs) noexcept {}

template <typename T> optional<T>::optional(null_t&& rhs) noexcept {}

template <typename T>
optional<T>& optional<T>::operator=(const null_t& rhs) noexcept {
  this->destroy();
  return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(null_t&& rhs) noexcept {
  this->destroy();
  return *this;
}

// === Observers === //
template <typename T> T* optional<T>::operator->() noexcept {
  return &this->val;
}

template <typename T> const T* optional<T>::operator->() const noexcept {
  return &this->val;
}

template <typename T> T& optional<T>::operator*() & noexcept {
  return this->val;
}

template <typename T> const T& optional<T>::operator*() const& noexcept {
  return this->val;
}

template <typename T> constexpr optional<T>::operator bool() const noexcept {
  return this->set;
}

} // namespace ds

#endif

