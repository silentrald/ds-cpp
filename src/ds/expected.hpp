/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_EXPECTED_HPP
#define DS_EXPECTED_HPP

#include <algorithm>
#include <type_traits>

namespace ds {

template <typename T, typename E> class expected;

template <class E> class unexpected {
public:
  unexpected() noexcept = default;
  unexpected(const unexpected&) noexcept = delete;
  unexpected& operator=(const unexpected&) noexcept = delete;

  // === Move === //
  unexpected(unexpected&& rhs) noexcept = default;
  unexpected& operator=(unexpected&& rhs) noexcept = default;

  // === Destructor === //
  ~unexpected() noexcept = default;

  unexpected(E&& rhs) noexcept : err(std::move(rhs)) {} // NOLINT

  template <typename = std::enable_if<std::is_fundamental_v<E>>>
  unexpected(const E& other) noexcept { // NOLINT
    this->err = other;
  }

  E& error() const noexcept {
    return this->err;
  }

  E&& error() noexcept {
    return std::move(this->err);
  }

private:
  E err{};
};

template <class T, class E> class expected {
public:
  using value_type = T;
  using error_type = T;

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

private:
  union {
    T val;
    E err;
  };
  bool has_value = false;
  bool has_error = false;

  // === Initializers === //

  inline void init_value() noexcept {
    if constexpr (std::is_class<T>::value) {
      new (&this->val) T;
    }
  }

  inline void init_error() noexcept {
    if constexpr (std::is_class<E>::value) {
      new (&this->err) E;
    }
  }

  // === Destructor === //

  void destroy() noexcept;

  inline void destroy_value() noexcept {
    this->has_value = false;
    if constexpr (std::is_class<T>::value) {
      this->val.~T();
    }
  }

  inline void destroy_error() noexcept {
    this->has_error = false;
    if constexpr (std::is_class<E>::value) {
      this->err.~T();
    }
  }
};

} // namespace ds

#ifndef DS_EXPECTED_TPP
#include "./expected.tpp"
#endif

#endif
