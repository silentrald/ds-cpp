/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_EXPECTED_TPP
#define DS_EXPECTED_TPP

#include "./expected.hpp"
#include <utility>

namespace ds {

// === Move === //

template <typename T, typename E>
expected<T, E>::expected(expected&& rhs) noexcept
    : has_value(rhs.has_value), has_error(rhs.has_error) {
  if (this->has_value) {
    this->init_value();
    this->val = std::move(rhs.val);
    rhs.has_value = false;
  } else if (this->has_error) {
    this->init_error();
    this->err = std::move(rhs.err);
    rhs.has_error = false;
  }
}

template <typename T, typename E>
expected<T, E>::expected(T&& rhs) noexcept : has_value(true) {
  this->init_value();
  this->val = std::move(rhs);
}

template <typename T, typename E>
expected<T, E>::expected(const T& rhs) noexcept : has_value(true) {
  if constexpr (std::is_class<T>::value) {
    new (&this->val) T;
    this->val.copy(rhs);
  } else {
    this->val = rhs; // Will fail if not copyable
  }
}

template <typename T, typename E>
expected<T, E>::expected(unexpected<E>&& rhs) noexcept : has_error(true) {
  this->init_error();
  this->err = std::move(rhs.error());
}

template <typename T, typename E>
expected<T, E>& expected<T, E>::operator=(expected&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  this->destroy();

  if (rhs.has_value) {
    this->has_value = true;
    rhs.has_value = false;

    this->init_value();
    this->val = std::move(rhs.val);
  } else if (rhs.has_error) {
    this->has_error = true;
    rhs.has_error = false;

    this->init_error();
    this->err = std::move(rhs.err);
  }

  return *this;
}

template <typename T, typename E>
expected<T, E>& expected<T, E>::operator=(T&& rhs) noexcept {
  if (this->has_value) {
    this->val = std::move(rhs);
    return *this;
  }

  if (this->has_error) {
    this->destroy_error();
  }

  this->has_value = true;
  this->init_value();
  this->val = std::move(rhs.val);

  return *this;
}

template <typename T, typename E>
expected<T, E>& expected<T, E>::operator=(unexpected<E>&& rhs) noexcept {
  if (this->has_error) {
    this->err = std::move(rhs.error());
    return *this;
  }

  if (this->has_value) {
    this->destroy_value();
  }

  this->has_error = true;
  this->init_error();
  this->err = std::move(rhs.error());

  return *this;
}

// === Destructor === //
template <typename T, typename E> expected<T, E>::~expected() noexcept {
  this->destroy();
}

template <typename T, typename E> void expected<T, E>::destroy() noexcept {
  if (this->has_value) {
    this->destroy_value();
  } else if (this->has_error) {
    this->destroy_error();
  }
}

// === Observers === //

template <typename T, typename E> T* expected<T, E>::operator->() noexcept {
  return &this->val;
}

template <typename T, typename E>
const T* expected<T, E>::operator->() const noexcept {
  return &this->val;
}

template <typename T, typename E> T& expected<T, E>::operator*() & noexcept {
  return this->val;
}

template <typename T, typename E>
const T& expected<T, E>::operator*() const& noexcept {
  return this->val;
}

template <typename T, typename E>
expected<T, E>::operator bool() const noexcept {
  return this->has_value;
}

template <typename T, typename E> T& expected<T, E>::value() noexcept {
  return this->val;
}

template <typename T, typename E>
const T& expected<T, E>::value() const noexcept {
  return this->val;
}

template <typename T, typename E> E& expected<T, E>::error() noexcept {
  return this->err;
}

template <typename T, typename E>
const E& expected<T, E>::error() const noexcept {
  return this->err;
}

template <typename T, typename E>
T& expected<T, E>::value_or(T&& default_value) const& noexcept {
  return this->has_value ? this->val : default_value;
}

// === Non-member function === //
template <typename T, typename E>
constexpr bool
operator==(const expected<T, E>& lhs, const expected<T, E>& rhs) noexcept {
  if (lhs.has_value && rhs.has_value) {
    return lhs.val == rhs.val;
  }

  if (lhs.has_error && rhs.has_error) {
    return lhs.err == rhs.err;
  }

  return !lhs.has_value && !lhs.has_error && !rhs.has_value && !rhs.has_error;
}

template <typename T, typename E>
constexpr bool operator==(const expected<T, E>& lhs, const T& rhs) noexcept {
  return lhs.has_value ? lhs.val == rhs : false;
}

template <typename T, typename E>
constexpr bool
operator==(const expected<T, E>& lhs, const unexpected<E>& rhs) noexcept {
  return lhs.has_error ? false : lhs.err = rhs.err;
}

} // namespace ds

#endif
