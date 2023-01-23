/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 * Updated: 2023-01-20
 *===============================*/

#ifndef DS_UNIQUE_PTR_HPP
#define DS_UNIQUE_PTR_HPP

#include "./error.hpp"
#include "./optional.hpp"

namespace ds {

const char* const UPTR_BAD_ALLOC = "unique_ptr:bad_alloc";

template <typename T> class unique_ptr {
public:
  using value = T;
  using ref = value&;
  using ptr = value*;
  using cref = const value&;
  using rref = value&&;

private:
  ptr data = nullptr;

  // === Destructor === //
  void destroy() noexcept;

public:
  unique_ptr() noexcept = default;
  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator=(const unique_ptr&) = delete;

  // === Move === //
  unique_ptr(unique_ptr&& rhs) noexcept;
  unique_ptr& operator=(unique_ptr&& rhs) noexcept;

  // === Destructor === //
  ~unique_ptr() noexcept;

  // === Modifiers === //
  [[nodiscard]] opt_err set(cref data) noexcept;
  [[nodiscard]] opt_err set(rref data) noexcept;

  [[nodiscard]] ptr release() noexcept;
  void reset() noexcept;

  // === Observers === //
  [[nodiscard]] T* operator->() const noexcept;
  [[nodiscard]] T& operator*() const& noexcept;
  [[nodiscard]] constexpr explicit operator bool() const noexcept;
};

} // namespace ds

#ifndef DS_UNIQUE_PTR_TPP
#include "./unique_ptr.tpp"
#endif

#endif

