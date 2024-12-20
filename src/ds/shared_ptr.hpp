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

namespace { // NOLINT

template <typename T> struct _data {
  T value{};
  usize count = 0U;
};

} // namespace

// NOTE: Not thread safe
template <typename T, typename Allocator = allocator<_data<T>>>
class shared_ptr {
public:
  using value_type = T;
  using data_type = _data<T>;

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
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const shared_ptr& other) noexcept {
    if (&other == this || this->data == other.data) {
      return error::OK;
    }

    this->destroy();
    if (other.data == nullptr) {
      return error::OK;
    }

    this->data = other.data;
    ++(this->data->count);

    return error::OK;
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
   * Initializes an object with an empty constructor
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code init() noexcept {
    if (this->data == nullptr) {
      TRY(this->allocate());
    }

    return error::OK;
  }

  /**
   * Set the value of the shared_ptr
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code set(const T& data) noexcept {
    return this->set_impl<const T&>(data);
  }

  /**
   * Set the value of the shared_ptr
   *
   * @errors
   *  - error::BAD_ALLOCATION
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
      return error::BAD_ALLOCATION;
    }

    if constexpr (std::is_class<T>::value) {
      new (&this->data->value) T;
    }

    this->data->count = 1U;
    return error::OK;
  }

  // === Initializers === //

  /**
   * @errors
   *  - error::BAD_ALLOCATION
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

        return error::BAD_ALLOCATION;
      }
    }

    return error::OK;
  }
};

} // namespace ds

#endif
