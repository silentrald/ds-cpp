/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#ifndef DS_SHARED_PTR_HPP
#define DS_SHARED_PTR_HPP

#include "./allocator.hpp"
#include "types.hpp"
#include <type_traits>

namespace ds {

namespace _shared_ptr {

template <typename T> struct data {
  T value = nullptr;
  usize count = 0U;
};

} // namespace _shared_ptr

// NOTE: Not thread safe
template <typename T, typename Allocator = allocator<_shared_ptr::data<T>>>
class shared_ptr {
public:
  using value_type = T;
  using data_type = _shared_ptr::data<T>;

  shared_ptr() noexcept = default;
  shared_ptr(const shared_ptr&) = delete;
  shared_ptr& operator=(const shared_ptr&) = delete;

  // === Move === //

  shared_ptr(shared_ptr&& other) noexcept : data(other.data) {
    other.data = nullptr;
  }

  shared_ptr& operator=(shared_ptr&& rhs) noexcept {
    if (this == &rhs) {
      return *this;
    }

    this->data = rhs.data;
    rhs.data = nullptr;

    return *this;
  }

  // === Copy === //

  /**
   * Copies another shared_ptr to this one and incrementing the counter by one
   *
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const shared_ptr& other) noexcept {
    if (&other == this || this->data == other.data) {
      return error_code::OK;
    }

    this->destroy();
    if (other.data == nullptr) {
      return error_code::OK;
    }

    this->data = other.data;
    ++(this->data->count);

    return error_code::OK;
  }

  // === Destructor === //

  ~shared_ptr() noexcept {
    this->destroy();
  }

  void destroy() noexcept {
    if (this->data == nullptr) {
      return;
    }

    if (--(this->data->count) == 0) {
      if constexpr (std::is_class<T>::value) {
        this->data->value.~T();
      }
      Allocator{}.deallocate(this->data);
    }
    this->data = nullptr;
  }

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

  // === Observers === //

  [[nodiscard]] T* get() const noexcept {
    return &this->data->value;
  }

  [[nodiscard]] T& operator*() const noexcept {
    return this->data->value;
  }

  T* operator->() noexcept {
    return &this->data->value;
  }

  const T* operator->() const noexcept {
    return &this->data->value;
  }

  [[nodiscard]] usize get_count() const noexcept {
    return this->data == nullptr ? 0U : this->data->count;
  }

  explicit operator bool() const noexcept {
    return this->data != nullptr;
  }

  // === Non-member Operators === //

  bool operator==(const shared_ptr<T>& rhs) noexcept {
    return this->data == rhs.data;
  }

  bool operator!=(const shared_ptr<T>& rhs) noexcept {
    return this->data != rhs.data;
  }

private:
  data_type* data = nullptr;

  // === Memory === //

  [[nodiscard]] error_code allocate() noexcept {
    this->data =
        static_cast<data_type*>(Allocator{}.allocate(sizeof(data_type)));
    if (this->data == nullptr) {
      return error_code::BAD_ALLOCATION;
    }

    if constexpr (std::is_class<T>::value) {
      new (&this->data->value) T;
    }

    this->data->count = 1U;
    return error_code::OK;
  }

  // === Initializers === //

  /**
   * @errors
   *  - error_code::BAD_ALLOCATION
   **/
  template <typename Data_>
  [[nodiscard]] error_code set_impl(Data_ data) noexcept {
    if (this->data != nullptr) {
      this->destroy();
    }

    TRY(this->allocate());

    if constexpr (std::is_rvalue_reference<Data_>::value) {
      this->data->value = std::move(data);
    } else if constexpr (!std::is_class<T>::value) {
      this->data->value = data;
    } else {
      if (is_error(this->data->value.copy(data))) {
        this->data->value.~T();
        Allocator{}.deallocate(this->data);
        this->data = nullptr;

        return error_code::BAD_ALLOCATION;
      }
    }

    return error_code::OK;
  }
};

} // namespace ds

#endif
