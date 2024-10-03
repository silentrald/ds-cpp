/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_UNIQUE_PTR_HPP
#define DS_UNIQUE_PTR_HPP

#include "types.hpp"

namespace ds {

template <typename T> class unique_ptr {
public:
  using value = T;

  unique_ptr() noexcept = default;
  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator=(const unique_ptr&) = delete;

  // === Move === //

  unique_ptr(unique_ptr&& rhs) noexcept;
  unique_ptr& operator=(unique_ptr&& rhs) noexcept;

  // === Destructor === //

  ~unique_ptr() noexcept;

  // === Modifiers === //

  /**
   * @errors
   *  - error_code::ALREADY_SET
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code set(const T& data) noexcept;

  /**
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code set(T&& data) noexcept;

  [[nodiscard]] T release() noexcept;

  void reset() noexcept;

  // === Observers === //

  [[nodiscard]] T* get_ptr() const noexcept;
  [[nodiscard]] T* operator->() const noexcept;
  [[nodiscard]] T& operator*() const& noexcept;
  [[nodiscard]] constexpr explicit operator bool() const noexcept;

private:
  T* data = nullptr;
};

} // namespace ds

#ifndef DS_UNIQUE_PTR_TPP
#include "./unique_ptr.tpp"
#endif

#endif
