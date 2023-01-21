/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 * Updated: 2023-01-20
 *===============================*/

#ifndef DS_EXPECTED_IMPL_HPP
#define DS_EXPECTED_IMPL_HPP

#include "./def.hpp"
#include <utility>

namespace ds {

// * expected * //
// === Move === //
template <typename T, typename E>
expected<T, E>::expected(expected&& rhs) noexcept
    : has_val(rhs.has_val), has_err(rhs.has_err) {
  if (this->has_val) {
    new (&this->val) T;
    this->val = std::move(rhs.val);
    return;
  }

  if (this->has_err) {
    new (&this->err) E;
    this->err = std::move(rhs.err);
  }
}

template <typename T, typename E>
expected<T, E>::expected(T&& rhs) noexcept : has_val(true) {
  new (&this->val) T;
  this->val = std::move(rhs);
}

template <typename T, typename E>
expected<T, E>::expected(unexpected<E>&& rhs) noexcept : has_err(true) {
  new (&this->err) E;
  this->err = std::move(rhs.error());
}

template <typename T, typename E>
expected<T, E>& expected<T, E>::operator=(expected&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  this->destroy();

  if (rhs.has_val) {
    this->has_val = true;
    rhs.has_val = false;

    new (&this->val) T;
    this->val = std::move(rhs.val);
  } else if (this->has_err) {
    this->has_err = true;
    rhs.has_err = false;

    new (&this->err) E;
    this->err = std::move(rhs.err);
  }

  return *this;
}

template <typename T, typename E>
expected<T, E>& expected<T, E>::operator=(T&& rhs) noexcept {
  if (this->has_val) {
    this->val = std::move(rhs);
    return *this;
  }

  if (this->has_err) {
    this->has_err = false;
    this->err.~E();
  }
  this->has_val = true;
  new (&this->val) T;
  this->val = std::move(rhs.val);

  return *this;
}

template <typename T, typename E>
expected<T, E>& expected<T, E>::operator=(unexpected<E>&& rhs) noexcept {
  if (this->has_err) {
    this->err = std::move(rhs.error());
    return *this;
  }

  if (this->has_val) {
    this->has_val = false;
    this->val.~T();
  }
  this->has_err = true;
  new (&this->err) E;
  this->err = std::move(rhs.error());

  return *this;
}

// === Destructor === //
template <typename T, typename E> expected<T, E>::~expected() noexcept {
  this->destroy();
}

template <typename T, typename E> void expected<T, E>::destroy() noexcept {
  if (this->has_val) {
    this->has_val = false;
    this->val.~T();
  } else if (this->has_err) {
    this->has_err = false;
    this->err.~E();
  }
}

// === Observers === //
template <typename T, typename E>
T* expected<T, E>::operator->() const noexcept {
  return &this->val;
}

template <typename T, typename E>
T& expected<T, E>::operator*() const& noexcept {
  return this->val;
}

template <typename T, typename E>
expected<T, E>::operator bool() const noexcept {
  return this->has_val;
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
  return this->has_val ? this->val : default_value;
}

// === Non-member function === //
template <typename T, typename E>
constexpr bool
operator==(const expected<T, E>& lhs, const expected<T, E>& rhs) noexcept {
  if (lhs.has_val && rhs.has_val) {
    return lhs.val == rhs.val;
  }

  if (lhs.has_err && rhs.has_err) {
    return lhs.err == rhs.err;
  }

  return !lhs.has_val && !lhs.has_err && !rhs.has_val && !rhs.has_err;
}

template <typename T, typename E>
constexpr bool operator==(const expected<T, E>& lhs, const T& rhs) noexcept {
  return lhs.has_val ? lhs.val == rhs : false;
}

template <typename T, typename E>
constexpr bool
operator==(const expected<T, E>& lhs, const unexpected<E>& rhs) noexcept {
  return lhs.has_err ? false : lhs.err = rhs.err;
}

// * expected_ptr * //
// === Move === //
template <typename T, typename E>
expected_ptr<T, E>::expected_ptr(expected_ptr&& rhs) noexcept
    : ptr(rhs.ptr),
      err(std::move(rhs.err)),
      has_err(rhs.has_err),
      has_ptr(rhs.has_ptr) {
  rhs.ptr = nullptr;
}

template <typename T, typename E>
expected_ptr<T, E>::expected_ptr(T* rhs) noexcept {
  if (rhs) {
    this->has_ptr = true;
    this->ptr = rhs;
  }
}

template <typename T, typename E>
expected_ptr<T, E>::expected_ptr(unexpected<E>&& rhs) noexcept
    : err(std::move(rhs.error())), has_err(true) {}

template <typename T, typename E>
expected_ptr<T, E>& expected_ptr<T, E>::operator=(expected_ptr&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  this->destroy();

  if (rhs.has_ptr) {
    this->has_ptr = true;
    rhs.has_ptr = false;
    this->ptr = rhs.ptr;
  } else if (rhs.has_err) {
    this->has_err = true;
    rhs.has_err = false;
    this->err = std::move(rhs.err);
  }

  return *this;
}

// === Destructor === //
template <typename T, typename E> expected_ptr<T, E>::~expected_ptr() noexcept {
  this->destroy();
}

template <typename T, typename E> void expected_ptr<T, E>::destroy() noexcept {
  if (this->has_ptr) {
    this->has_ptr = false;
    this->ptr = nullptr;
  } else if (this->has_err) {
    this->has_err = false;
    this->err.~E();
  }
}

// === Observers === //
template <typename T, typename E>
T* expected_ptr<T, E>::operator->() const noexcept {
  return this->val;
}

template <typename T, typename E>
T& expected_ptr<T, E>::operator*() const& noexcept {
  return *this->val;
}

template <typename T, typename E>
expected_ptr<T, E>::operator bool() const noexcept {
  return this->has_val;
}

template <typename T, typename E>
bool expected_ptr<T, E>::is_null() const noexcept {
  return this->val == nullptr;
}

template <typename T, typename E> T& expected_ptr<T, E>::value() noexcept {
  return *this->val;
}

template <typename T, typename E>
const T& expected_ptr<T, E>::value() const noexcept {
  return *this->val;
}

template <typename T, typename E> T* expected_ptr<T, E>::data() noexcept {
  return this->val;
}

template <typename T, typename E>
const T* expected_ptr<T, E>::data() const noexcept {
  return this->val;
}

template <typename T, typename E> E& expected_ptr<T, E>::error() noexcept {
  return this->err;
}

template <typename T, typename E>
const E& expected_ptr<T, E>::error() const noexcept {
  return this->err;
}

template <typename T, typename E>
T* expected_ptr<T, E>::value_or(T* default_value) const noexcept {
  return this->has_ptr ? this->ptr : default_value;
}

// === Non-member function === //
template <typename T, typename E>
constexpr bool operator==(
    const expected_ptr<T, E>& lhs, const expected_ptr<T, E>& rhs
) noexcept {
  if (lhs.has_ptr == rhs.has_ptr) {
    return lhs.val == rhs.val;
  }

  if (lhs.has_err == rhs.has_err) {
    return lhs.err == rhs.err;
  }

  return !lhs.has_ptr && !lhs.has_err && !rhs.has_ptr && !rhs.has_err;
}

template <typename T, typename E>
constexpr bool
operator==(const expected_ptr<T, E>& lhs, const T& rhs) noexcept {
  return lhs.has_val ? *lhs.val == rhs : false;
}

template <typename T, typename E>
constexpr bool
operator==(const expected_ptr<T, E>& lhs, const T* rhs) noexcept {
  return lhs.has_val ? lhs.val == rhs : false;
}

template <typename T, typename E>
constexpr bool
operator==(const expected_ptr<T, E>& lhs, const unexpected<E>& rhs) noexcept {
  return lhs.has_err ? lhs.err = rhs.err : false;
}

} // namespace ds

#endif

