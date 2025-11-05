/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-20
 *===============================*/

#ifndef DS_UNIQUE_PTR_HPP
#define DS_UNIQUE_PTR_HPP

#include "./allocator.hpp"
#include "types.hpp"

namespace ds {

template <typename T, typename Allocator = allocator<T>> class unique_ptr {
public:
  using value = T;

  unique_ptr() noexcept = default;
  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator=(const unique_ptr&) = delete;

  // === Move === //

  unique_ptr(unique_ptr&& other) noexcept : data(other.data) {
    other.data = nullptr;
  }

  unique_ptr& operator=(unique_ptr&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->destroy();
    this->data = rhs.data;
    rhs.data = nullptr;

    return *this;
  }

  // === Destructor === //

  ~unique_ptr() noexcept {
    this->destroy();
  }

  void destroy() noexcept {
    if (this->data == nullptr) {
      return;
    }

    if constexpr (std::is_class_v<T>) {
      this->data->~T();
    }
    Allocator{}.deallocate(this->data);
    this->data = nullptr;
  }

  [[nodiscard]] T release() noexcept {
    auto* tmp = this->data;
    this->data = nullptr;
    return std::move(*tmp);
  }

  // === Modifiers === //

  /**
   * @errors
   *  - error_codes::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code set(const T& data) noexcept {
    if (this->data) {
      if constexpr (std::is_class_v<T>) {
        DS_TRY(this->data->copy(data));
      } else {
        *this->data = data;
      }

      return error_codes::OK;
    }

    this->data = static_cast<T*>(Allocator{}.allocate(sizeof(T)));
    if (this->data == nullptr) {
      return error_codes::BAD_ALLOCATION;
    }

    if constexpr (std::is_class_v<T>) {
      new (this->data) T();

      error_code error = this->data->copy(data);
      if (is_error(error)) {
        this->data.~T();
        std::free(this->data); // NOLINT
        this->data = nullptr;
        return error;
      }
    } else {
      *this->data = data;
    }

    return error_codes::OK;
  }

  /**
   * @errors
   *  - error_codes::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code set(T&& data) noexcept {
    if (this->data == nullptr) {
      this->data = (T*)std::malloc(sizeof(T)); // NOLINT
      if (this->data == nullptr) {
        return error_codes::BAD_ALLOCATION;
      }

      if constexpr (std::is_class_v<T>) {
        new (this->data) T();
      }
    }

    *this->data = std::move(data);
    return error_codes::OK;
  }

  // === Observers === //

  [[nodiscard]] T* get_pointer() const noexcept {
    return this->data;
  }

  [[nodiscard]] T* operator->() const noexcept {
    return this->data;
  }

  [[nodiscard]] T& operator*() const& noexcept {
    return *this->data;
  }

  [[nodiscard]] constexpr explicit operator bool() const noexcept {
    return this->data != nullptr;
  }

private:
  T* data = nullptr;
};

} // namespace ds

#endif
