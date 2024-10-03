/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_SHARED_PTR_HPP
#define DS_SHARED_PTR_HPP

#include "types.hpp"

#if DS_TEST
#include "../../tests/main.hpp"
#endif

namespace ds {

// NOTE: Not thread safe
template <typename T> class shared_ptr {
public:
  using value = T;

  shared_ptr() noexcept = default;
  shared_ptr(const shared_ptr&) = delete;
  shared_ptr& operator=(const shared_ptr&) = delete;

  // === Copy === //

  /**
   * Copies another shared_ptr to this one and incrementing the counter by one
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const shared_ptr& other) noexcept;

  // === Move === //

  shared_ptr(shared_ptr&& rhs) noexcept;
  shared_ptr& operator=(shared_ptr&& rhs) noexcept;

  // === Destructor === //

  ~shared_ptr() noexcept;

  // === Modifiers === //

  /**
   * Set the value of the shared_ptr
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code set(const T& data) noexcept {
    return this->set_impl<const T&>(data);
  }

  /**
   * Set the value of the shared_ptr
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code set(T&& data) noexcept {
    return this->set_impl<T&&>(std::move(data));
  }

  void reset() noexcept;

  // === Observers === //

  [[nodiscard]] T* get() const noexcept;
  [[nodiscard]] T& operator*() const noexcept;
  T* operator->() const noexcept;
  [[nodiscard]] usize get_count() const noexcept;
  explicit operator bool() const noexcept;

  // === Non-member Operators === //

  template <typename T_>
  friend bool
  operator==(const shared_ptr<T_>& lhs, const shared_ptr<T_>& rhs) noexcept;
  template <typename T_>
  friend bool
  operator!=(const shared_ptr<T_>& lhs, const shared_ptr<T_>& rhs) noexcept;

private:
  struct Data {
    T val = nullptr;
    usize count = 0U;
  };

  Data* data = nullptr;

  // === Memory === //

  [[nodiscard]] error_code allocate() noexcept;

  // === Initializers === //

  /**
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  template <typename Data_>
  [[nodiscard]] error_code set_impl(Data_ data) noexcept;
};

} // namespace ds

#ifndef DS_SHARED_PTR_TPP
#include "./shared_ptr.tpp"
#endif

#endif
