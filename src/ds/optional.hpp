/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_OPTIONAL_HPP
#define DS_OPTIONAL_HPP

#include <type_traits>
#include <utility>

namespace ds {

class null_t {};
const null_t null{};

template <typename T> class optional {
public:
  optional() noexcept = default;
  optional(const optional&) noexcept = delete;
  optional& operator=(optional&) noexcept = delete;

  // === Move === //

  optional(optional&& other) noexcept : set(other.set) {
    if (!this->set) {
      return;
    }

    this->val = std::move(other.val);
  }

  optional& operator=(optional&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    if (!rhs.set) {
      this->destroy();
      return *this;
    }

    this->set = true;
    this->val = std::move(rhs.val);

    return *this;
  }

  explicit optional(T&& other) noexcept : val(std::move(other)), set(true) {}

  optional& operator=(T&& rhs) noexcept {
    this->val = std::move(rhs);
    this->set = true;

    return *this;
  }

  // === Destructor === //

  ~optional() noexcept {
    this->destroy();
  }

  void destroy() noexcept {
    if (this->set) {
      this->set = false;
      if constexpr (std::is_class_v<T>) {
        this->val.~T();
      }
    }
  }

  // === Null === //

  explicit optional(const null_t& rhs) noexcept {}
  explicit optional(null_t&& rhs) noexcept {};

  optional& operator=(const null_t& rhs) noexcept {
    this->destroy();
    return *this;
  }

  optional& operator=(null_t&& rhs) noexcept {
    this->destroy();
    return *this;
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

  [[nodiscard]] constexpr explicit operator bool() const noexcept {
    return this->set;
  }

private:
  T val{};
  bool set = false;
};

} // namespace ds

#endif
