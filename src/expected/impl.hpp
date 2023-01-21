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
expected<T, E>::expected(expected&& rhs) noexcept : set(rhs.set) {
  if (this->set) {
    this->val = std::move(rhs.val);
  } else {
    this->err = std::move(rhs.err);
  }
}

template <typename T, typename E>
expected<T, E>& expected<T, E>::operator=(expected&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  this->destroy();

  this->set = rhs.set();
  if (this->set) {
    new (&this->val, std::move(rhs.val)) T;
  } else {
    new (&this->err, std::move(rhs.err)) E;
  }

  return *this;
}

// === Destructor === //
template <typename T, typename E> expected<T, E>::~expected() noexcept {
  this->destroy();
}

template <typename T, typename E> void expected<T, E>::destroy() noexcept {
  if (this->set) {
    this->set = false;
    this->val.~T();
  } else {
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
  return this->set;
}

template <typename T, typename E> T& expected<T, E>::value() const& noexcept {
  return this->val;
}

template <typename T, typename E> E& expected<T, E>::error() const& noexcept {
  return this->err;
}

template <typename T, typename E>
T& expected<T, E>::value_or(T&& default_value) const& noexcept {
  return this->set ? this->val : default_value;
}

// === Non-member function === //
template <typename T, typename E>
constexpr bool
operator==(const expected<T, E>& lhs, const expected<T, E>& rhs) noexcept {
  return lhs.set == rhs.set ? lhs.val == rhs.val : lhs.err == lhs.err;
}

template <typename T, typename E>
constexpr bool operator==(const expected<T, E>& lhs, const T& rhs) noexcept {
  return lhs.set ? lhs.val == rhs : false;
}

template <typename T, typename E>
constexpr bool
operator==(const expected<T, E>& lhs, const unexpected<E>& rhs) noexcept {
  return lhs.set ? false : lhs.err = rhs.err;
}

// * expected_ptr * //
// === Move === //
template <typename T, typename E>
expected_ptr<T, E>::expected_ptr(expected_ptr&& rhs) noexcept : set(rhs.set) {
  if (this->set) {
    this->ptr = rhs.ptr;
    rhs.ptr = nullptr;
  } else {
    this->err = std::move(rhs.err);
  }
}

template <typename T, typename E>
expected_ptr<T, E>& expected_ptr<T, E>::operator=(expected_ptr&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  this->destroy();

  this->set = rhs.set();
  if (this->set) {
    this->val = rhs.val;
  } else {
    new (&this->err, std::move(rhs.err)) E;
  }

  return *this;
}

// === Destructor === //
template <typename T, typename E> expected_ptr<T, E>::~expected_ptr() noexcept {
  this->destroy();
}

template <typename T, typename E> void expected_ptr<T, E>::destroy() noexcept {
  if (this->set) {
    this->set = false;
    this->ptr = nullptr;
  } else {
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
  return this->set;
}

template <typename T, typename E>
bool expected_ptr<T, E>::is_null() const noexcept {
  return this->val == nullptr;
}

template <typename T, typename E>
T& expected_ptr<T, E>::value() const& noexcept {
  return *this->val;
}

template <typename T, typename E> T* expected_ptr<T, E>::data() const noexcept {
  return this->val;
}

template <typename T, typename E>
E& expected_ptr<T, E>::error() const& noexcept {
  return this->err;
}

template <typename T, typename E>
T& expected_ptr<T, E>::value_or(T&& default_value) const& noexcept {
  return this->set ? this->val : default_value;
}

// === Non-member function === //
template <typename T, typename E>
constexpr bool operator==(
    const expected_ptr<T, E>& lhs, const expected_ptr<T, E>& rhs
) noexcept {
  return lhs.set == rhs.set ? lhs.val == rhs.val : lhs.err == lhs.err;
}

template <typename T, typename E>
constexpr bool
operator==(const expected_ptr<T, E>& lhs, const T& rhs) noexcept {
  return lhs.set ? lhs.val == &rhs : false;
}

template <typename T, typename E>
constexpr bool
operator==(const expected_ptr<T, E>& lhs, const T* rhs) noexcept {
  return lhs.set ? lhs.val == rhs : false;
}

template <typename T, typename E>
constexpr bool
operator==(const expected_ptr<T, E>& lhs, const unexpected<E>& rhs) noexcept {
  return lhs.set ? false : lhs.err = rhs.err;
}

} // namespace ds

#endif

