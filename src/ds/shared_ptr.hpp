/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_SHARED_PTR_HPP
#define DS_SHARED_PTR_HPP

#include "ds-error/types.hpp"
#include "ds/types.hpp"

#if DS_TEST
#include "../../tests/main.hpp"
#endif

namespace ds {

// NOTE: Not thread safe
template <typename T> class shared_ptr {
public:
  using value = T;
  using ref = value&;
  using ptr = value*;
  using cref = const value&;
  using rref = value&&;

private:
  ptr data = nullptr;
  int* counter = nullptr;

  // === Memory === //
  [[nodiscard]] opt_err allocate() noexcept;

  // === Destructor === //
  void destroy() noexcept;
  void destroy_data_ptr() noexcept;
  void destroy_counter_ptr() noexcept;
  void destroy_all_ptrs() noexcept;

  // === Initializers === //
  template <typename Data_> [[nodiscard]] opt_err set_impl(Data_ data) noexcept;

public:
  shared_ptr() noexcept = default;
  shared_ptr(const shared_ptr&) = delete;
  shared_ptr& operator=(const shared_ptr&) = delete;

  // === Copy === //
  [[nodiscard]] opt_err copy(const shared_ptr& other) noexcept;

  // === Move === //
  shared_ptr(shared_ptr&& rhs) noexcept;
  shared_ptr& operator=(shared_ptr&& rhs) noexcept;

  // === Destructor === //
  ~shared_ptr() noexcept;

  // === Modifiers === //
  [[nodiscard]] opt_err init() noexcept;

  [[nodiscard]] opt_err set(cref data) noexcept {
    return this->set_impl<cref>(data);
  }

  [[nodiscard]] opt_err set(rref data) noexcept {
    return this->set_impl<rref>(std::move(data));
  }

  void release() noexcept;

  // === Observers === //
  [[nodiscard]] ptr get() const noexcept;
  [[nodiscard]] ref operator*() const noexcept;
  ptr operator->() const noexcept;
  [[nodiscard]] i32 count() const noexcept;
  explicit operator bool() const noexcept;

  // === Non-member Operators === //
  template <typename T_>
  friend bool
  operator==(const shared_ptr<T_>& lhs, const shared_ptr<T_>& rhs) noexcept;
  template <typename T_>
  friend bool
  operator!=(const shared_ptr<T_>& lhs, const shared_ptr<T_>& rhs) noexcept;
};

} // namespace ds

#ifndef DS_SHARED_PTR_TPP
#include "./shared_ptr.tpp"
#endif

#endif

