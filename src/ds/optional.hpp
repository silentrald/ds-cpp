/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_OPTIONAL_HPP
#define DS_OPTIONAL_HPP

#include <utility>

namespace ds {

class null_t {};
const null_t null{};

template <typename T> class optional {
private:
  T val{};
  bool set = false;

  // === Destructor === //
  void destroy() noexcept;

public:
  optional() noexcept = default;
  optional(const optional&) noexcept = delete;
  optional& operator=(optional&) noexcept = delete;

  // === Move === //
  optional(optional&& rhs) noexcept;
  optional& operator=(optional&& rhs) noexcept;
  optional(T&& rhs) noexcept; // NOLINT
  optional& operator=(T&& rhs) noexcept;

  // === Destructor === //
  ~optional() noexcept;

  // === Null === //
  optional(const null_t& rhs) noexcept; // NOLINT
  optional(null_t&& rhs) noexcept;      // NOLINT
  optional& operator=(const null_t& rhs) noexcept;
  optional& operator=(null_t&& rhs) noexcept;

  // === Observers === //

  [[nodiscard]] T* operator->() noexcept;
  [[nodiscard]] const T* operator->() const noexcept;
  [[nodiscard]] T& operator*() & noexcept;
  [[nodiscard]] const T& operator*() const& noexcept;

  [[nodiscard]] constexpr explicit operator bool() const noexcept;
};

} // namespace ds

#ifndef DS_OPTIONAL_TPP
#include "./optional.tpp"
#endif

#endif

