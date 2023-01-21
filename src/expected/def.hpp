/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 * Updated: 2023-01-20
 *===============================*/

#ifndef DS_EXPECTED_DEF_HPP
#define DS_EXPECTED_DEF_HPP

namespace ds {

template <class E> class unexpected {
private:
  E err{};

public:
  unexpected() noexcept = default;
  unexpected(const unexpected&) = delete;
  unexpected& operator=(const unexpected&) = delete;

  // === Move === //
  unexpected(unexpected&& rhs) noexcept = default;
  unexpected& operator=(unexpected&& rhs) noexcept = default;

  // === Destructor === //
  ~unexpected() noexcept = default;

  E& error() const noexcept {
    return this->err;
  }
};

template <class T, class E> class expected {
public:
  using value_type = T;
  using error_type = T;

private:
  union {
    T val;
    E err;
  };
  bool set = false;

  // === Destructor === //
  void destroy() noexcept;

public:
  expected() noexcept = default;
  expected(const expected&) noexcept = delete;
  expected& operator=(const expected&) noexcept = delete;

  // === Move === //
  expected(expected&& rhs) noexcept;
  expected& operator=(expected&& rhs) noexcept;

  // === Destructor === //
  ~expected() noexcept;

  // === Observers === //
  [[nodiscard]] T* operator->() const noexcept;
  [[nodiscard]] T& operator*() const& noexcept;
  explicit operator bool() const noexcept;

  [[nodiscard]] T& value() const& noexcept;
  [[nodiscard]] E& error() const& noexcept;

  [[nodiscard]] T& value_or(T&& default_value) const& noexcept;

  // === Non-member function === //
  friend constexpr bool
  operator==(const expected& lhs, const expected& rhs) noexcept;
  friend constexpr bool operator==(const expected& lhs, const T& val) noexcept;
  friend constexpr bool
  operator==(const expected& lhs, const unexpected<E>& rhs) noexcept;
};

template <class T, class E> class expected_ptr {
public:
  using value_type = T;
  using value_ptr = T*;
  using error_type = T;

private:
  union {
    T* ptr;
    E err;
  };
  bool set = false;

  // === Destructor === //
  void destroy() noexcept;

public:
  expected_ptr() noexcept = default;
  expected_ptr(const expected_ptr&) noexcept = delete;
  expected_ptr& operator=(const expected_ptr&) noexcept = delete;

  // === Move === //
  expected_ptr(expected_ptr&& rhs) noexcept;
  expected_ptr& operator=(expected_ptr&& rhs) noexcept;

  // === Destructor === //
  ~expected_ptr() noexcept;

  // === Observers === //
  explicit operator bool() const noexcept;
  [[nodiscard]] bool is_null() const noexcept;

  [[nodiscard]] T* operator->() const noexcept;
  [[nodiscard]] T& operator*() const& noexcept;

  [[nodiscard]] T& value() const& noexcept;
  [[nodiscard]] T* data() const noexcept;
  [[nodiscard]] E& error() const& noexcept;

  [[nodiscard]] T& value_or(T&& default_value) const& noexcept;

  // === Non-member function === //
  friend constexpr bool
  operator==(const expected_ptr& lhs, const expected_ptr& rhs) noexcept;
  friend constexpr bool
  operator==(const expected_ptr& lhs, const T& val) noexcept;
  friend constexpr bool
  operator==(const expected_ptr& lhs, const T* val) noexcept;
  friend constexpr bool
  operator==(const expected_ptr& lhs, const unexpected<E>& rhs) noexcept;
};

} // namespace ds

#ifndef DS_EXPECTED_IMPL_HPP
#include "./impl.hpp"
#endif

#endif

