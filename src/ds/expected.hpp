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

  template <typename = std::enable_if<!std::is_class_v<E>>>
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

  expected(expected&& other) noexcept
      : has_value(other.has_value), has_error(other.has_error) {
    if (this->has_value) {
      this->init_value();
      this->val = std::move(other.val);
      other.has_value = false;
    } else if (this->has_error) {
      this->init_error();
      this->err = std::move(other.err);
      other.has_error = false;
    }
  }

  expected(T&& other) noexcept : has_value(true) { // NOLINT
    this->init_value();
    this->val = std::move(other);
  }

  template <typename = std::enable_if<!std::is_class_v<T>>>
  expected(const T& other) noexcept : has_value(true) { // NOLINT
    this->init_value();
    this->val = other;
  }

  expected(unexpected<E>&& other) noexcept : has_error(true) { // NOLINT
    this->init_error();
    this->err = std::move(other.error());
  }

  expected& operator=(expected&& rhs) noexcept {
    if (this == &rhs) {
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

  expected& operator=(T&& rhs) noexcept {
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

  expected& operator=(unexpected<E>&& rhs) noexcept {
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

  ~expected() noexcept {
    this->destroy();
  }

  void destroy() noexcept {
    if (this->has_value) {
      this->destroy_value();
    } else if (this->has_error) {
      this->destroy_error();
    }
  }

  // === Observers === //

  [[nodiscard]] T* operator->() noexcept {
    return &this->val;
  }

  [[nodiscard]] const T* operator->() const noexcept {
    return &this->val;
  }

  [[nodiscard]] T& operator*() & noexcept {
    return this->val;
  }

  [[nodiscard]] const T& operator*() const& noexcept {
    return this->val;
  }

  explicit operator bool() const noexcept {
    return this->has_value;
  }

  [[nodiscard]] T& value() noexcept {
    return this->val;
  }

  [[nodiscard]] const T& value() const noexcept {
    return this->val;
  }

  [[nodiscard]] E& error() noexcept {
    return this->err;
  }

  [[nodiscard]] const E& error() const noexcept {
    return this->err;
  }

  [[nodiscard]] T& value_or(T& default_value) noexcept {
    return this->has_value ? this->val : default_value;
  }

  [[nodiscard]] const T& value_or(const T& default_value) const& noexcept {
    return this->has_value ? this->val : default_value;
  }

  // === Comparators === //

  [[nodiscard]] bool operator==(const expected<T, E>& rhs) const noexcept {
    if (this->has_value && rhs.has_value) {
      return this->val == rhs.val;
    }

    if (this->has_error && this->has_error) {
      return this->err == this->err;
    }

    return !this->has_value && !this->has_error && !rhs.has_value &&
           !rhs.has_error;
  }

  [[nodiscard]] bool operator==(const T& rhs) const noexcept {
    return this->has_value && this->val == rhs;
  }

  [[nodiscard]] bool operator==(const unexpected<E>& rhs) const noexcept {
    return this->has_error && this->err == rhs.error();
  }

private:
  union {
    T val;
    E err;
  };
  bool has_value = false;
  bool has_error = false;

  // === Initializers === //

  inline void init_value() noexcept {
    if constexpr (std::is_class_v<T>) {
      new (&this->val) T;
    }
  }

  inline void init_error() noexcept {
    if constexpr (std::is_class_v<E>) {
      new (&this->err) E;
    }
  }

  inline void destroy_value() noexcept {
    this->has_value = false;
    if constexpr (std::is_class_v<T>) {
      this->val.~T();
    }
  }

  inline void destroy_error() noexcept {
    this->has_error = false;
    if constexpr (std::is_class_v<E>) {
      this->err.~T();
    }
  }
};

} // namespace ds

#endif
