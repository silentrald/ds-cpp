/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 * Updated: 2023-01-20
 *===============================*/

#ifndef DS_EXPECTED_DEF_HPP
#define DS_EXPECTED_DEF_HPP

#include <algorithm>

namespace ds {

template <typename T, typename E> class expected;

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
  unexpected(E&& rhs) noexcept : err(std::move(rhs)) {} // NOLINT

  // === Destructor === //
  ~unexpected() noexcept = default;

  E& error() const noexcept {
    return this->err;
  }

  E&& error() noexcept {
    return std::move(this->err);
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
  bool has_val = false;
  bool has_err = false;

  // === Destructor === //
  void destroy() noexcept;

public:
  expected() noexcept {}; // NOLINT
  expected(const expected&) noexcept = delete;
  expected& operator=(const expected&) noexcept = delete;

  // === Move === //
  expected(expected&& rhs) noexcept;
  expected(T&& rhs) noexcept;             // NOLINT
  expected(const T& rhs) noexcept;        // NOLINT
  expected(unexpected<E>&& rhs) noexcept; // NOLINT
  expected& operator=(expected&& rhs) noexcept;
  expected& operator=(T&& rhs) noexcept;
  expected& operator=(unexpected<E>&& rhs) noexcept;

  // === Destructor === //
  ~expected() noexcept;

  // === Observers === //
  [[nodiscard]] T* operator->() noexcept;
  [[nodiscard]] const T* operator->() const noexcept;
  [[nodiscard]] T& operator*() & noexcept;
  [[nodiscard]] const T& operator*() const& noexcept;
  explicit operator bool() const noexcept;

  [[nodiscard]] T& value() noexcept;
  [[nodiscard]] const T& value() const noexcept;
  [[nodiscard]] E& error() noexcept;
  [[nodiscard]] const E& error() const noexcept;

  [[nodiscard]] T& value_or(T&& default_value) const& noexcept;

  // === Non-member function === //
  template <typename T2, typename E2>
  friend constexpr bool
  operator==(const expected<T2, E2>& lhs, const expected<T2, E2>& rhs) noexcept;
  template <typename T2, typename E2>
  friend constexpr bool
  operator==(const expected<T2, E2>& lhs, const T2& val) noexcept;
  template <typename T2, typename E2>
  friend constexpr bool
  operator==(const expected<T2, E2>& lhs, const unexpected<E2>& rhs) noexcept;
};

template <class T, class E> class expected_ptr {
public:
  using value_type = T;
  using error_type = T;

private:
  T* ptr = nullptr;
  E err{};
  bool has_ptr = false;
  bool has_err = false;

  // === Destructor === //
  void destroy() noexcept;

public:
  expected_ptr() noexcept = default;
  expected_ptr(const expected_ptr&) noexcept = delete;
  expected_ptr& operator=(const expected_ptr&) noexcept = delete;

  // === Move === //
  expected_ptr(expected_ptr&& rhs) noexcept;
  expected_ptr(T* rhs) noexcept;              // NOLINT
  expected_ptr(unexpected<E>&& rhs) noexcept; // NOLINT
  expected_ptr& operator=(expected_ptr&& rhs) noexcept;
  expected_ptr& operator=(T* rhs) noexcept;
  expected_ptr& operator=(unexpected<E>&& rhs) noexcept;

  // === Destructor === //
  ~expected_ptr() noexcept;

  // === Observers === //
  explicit operator bool() const noexcept;
  [[nodiscard]] bool is_null() const noexcept;

  [[nodiscard]] T* operator->() noexcept;
  [[nodiscard]] const T* operator->() const noexcept;
  [[nodiscard]] T& operator*() & noexcept;
  [[nodiscard]] const T& operator*() const& noexcept;

  [[nodiscard]] T& value() noexcept;
  [[nodiscard]] const T& value() const noexcept;
  [[nodiscard]] T* data() noexcept;
  [[nodiscard]] const T* data() const noexcept;
  [[nodiscard]] E& error() noexcept;
  [[nodiscard]] const E& error() const noexcept;

  [[nodiscard]] T* value_or(T* default_value) const noexcept;

  // === Non-member function === //
  template <typename T2, typename E2>
  friend constexpr bool operator==(
      const expected_ptr<T2, E2>& lhs, const expected_ptr<T2, E2>& rhs
  ) noexcept;
  template <typename T2, typename E2>
  friend constexpr bool
  operator==(const expected_ptr<T2, E2>& lhs, const T2& val) noexcept;
  template <typename T2, typename E2>
  friend constexpr bool
  operator==(const expected_ptr<T2, E2>& lhs, const T2* val) noexcept;
  template <typename T2, typename E2>
  friend constexpr bool operator==(
      const expected_ptr<T2, E2>& lhs, const unexpected<E2>& rhs
  ) noexcept;
};

} // namespace ds

#ifndef DS_EXPECTED_IMPL_HPP
#include "./impl.hpp"
#endif

#endif

