/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_UNIQUE_PTR_HPP
#define DS_UNIQUE_PTR_HPP

#include "ds/macro.hpp"
#include "ds/optional.hpp"
#include "ds/type_traits.hpp"
#include "ds/types.hpp"
#include <new>
#include <type_traits>

namespace ds {

const char* const UPTR_BAD_ALLOC = "unique_ptr:bad_alloc";
const char* const UPTR_SET = "unique_ptr:set";

template <typename T> class unique_ptr {
public:
  template <typename D> friend class unique_ptr;

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
  [[nodiscard]] err_code init() noexcept;
  [[nodiscard]] err_code set(ptr data) noexcept;
  [[nodiscard]] err_code set(cref data) noexcept;
  [[nodiscard]] err_code set(rref data) noexcept;

  [[nodiscard]] ptr release() noexcept;
  void reset() noexcept;

  // === Observers === //
  [[nodiscard]] T* get_ptr() const noexcept;
  [[nodiscard]] T* operator->() const noexcept;
  [[nodiscard]] T& operator*() const& noexcept;
  [[nodiscard]] constexpr explicit operator bool() const noexcept;

  // === Polymorphism === //
  template <typename D>
  unique_ptr(unique_ptr<D>&& rhs) noexcept : data(rhs.data) { // NOLINT
    rhs.data = nullptr;
  }

  template <typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
  unique_ptr& operator=(unique_ptr<D>&& rhs) noexcept {
    if ((unique_ptr*)&rhs == this) {
      return *this;
    }

    this->destroy();
    this->data = rhs.data;
    rhs.data = nullptr;

    return *this;
  }

  template <typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
  [[nodiscard]] err_code init() noexcept {
    if (this->data) {
      return ec::ALREADY_SET;
    }

    this->data = new (std::nothrow) D(); // NOLINT
    return ec::SUCCESS;
  }

  template <typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
  [[nodiscard]] err_code set(ptr data) {
    if (this->data) {
      return ec::ALREADY_SET;
    }

    this->data = data;
    return ec::SUCCESS;
  }

  template <typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
  [[nodiscard]] D& set(const D& data) noexcept {
    if (!this->data) {
      return error{UPTR_SET, def_err_vals};
    }

    D* data_ = new (std::nothrow) D(); // NOLINT
    if (data_ == nullptr) {
      return error{UPTR_BAD_ALLOC, def_err_vals};
    }

    if constexpr (has_copy_method<value>::value) {
      auto err = data_->copy(data);
      if (err) {
        delete data_; // NOLINT
        return err;
      }
    } else {
      *data_ = data;
    }

    this->data = data_;
    return ec::SUCCESS;
  }

  template <typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
  [[nodiscard]] err_code set(D&& data) noexcept {
    if (this->data) {
      return ec::ALREADY_SET;
    }

    D* data_ = new (std::nothrow) D(); // NOLINT
    if (data_ == nullptr) {
      return ec::BAD_ALLOC;
    }

    *data_ = std::move(data); // NOLINT
    this->data = data_;
    return ec::SUCCESS;
  }

  template <typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
  [[nodiscard]] D& get_ref_unsafe() noexcept {
    return *(D*)this->data;
  }

  template <typename D, typename = std::enable_if_t<std::is_base_of_v<T, D>>>
  [[nodiscard]] D* get_ptr() const noexcept {
    return (D*)this->data;
  }
};

} // namespace ds

#ifndef DS_UNIQUE_PTR_TPP
#include "./unique_ptr.tpp"
#endif

#endif

