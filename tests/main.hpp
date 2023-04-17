/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_TEST_MAIN_HPP
#define DS_TEST_MAIN_HPP

#include "catch2/catch_message.hpp"
#include "ds-error/types.hpp"
#include "ds/types.hpp"

namespace ds_test {

extern ds::i32 counter; // NOLINT
extern void* free_ptr;  // NOLINT

[[nodiscard]] bool handle_error(ds::i32 err) noexcept;

template <typename T>
[[nodiscard]] bool handle_error(const ds::exp_err_code<T>& expected) noexcept {
  if (expected) {
    return true;
  }

  UNSCOPED_INFO("Error Code: " << expected.error());
  return false;
}

template <typename T>
[[nodiscard]] bool handle_error(const ds::exp_ptr_err_code<T>& expected
) noexcept {
  if (expected) {
    return true;
  }

  UNSCOPED_INFO("Error Code: " << expected.error());
  return false;
}

class Test {
private:
  ds::i32 i = 0;
  bool initialized = false;

public:
  Test() = default;
  Test(const Test& rhs) = delete;
  Test& operator=(const Test& rhs) = delete;

  // === Initializers === //
  explicit Test(ds::i32 i) : i(i), initialized(true) {
    ++ds_test::counter;
  }

  // === Copy === //
  [[nodiscard]] ds::i32 copy(const Test& other) noexcept;

  // === Move === //
  Test(Test&& rhs) noexcept;
  Test& operator=(Test&& rhs) noexcept;

  // === Destructor === //
  ~Test();

  // === Setter/Getter === //
  void set_int(ds::i32 i) noexcept;
  [[nodiscard]] ds::i32 get_int() const noexcept;

  // === Checkers === //
  [[nodiscard]] bool is_initialized() const noexcept;

  friend bool operator==(const Test& lhs, const Test& rhs) noexcept;
  friend bool operator==(const Test& lhs, ds::i32 rhs) noexcept;
  friend bool operator!=(const Test& lhs, const Test& rhs) noexcept;
  friend bool operator!=(const Test& lhs, ds::i32 rhs) noexcept;
};

} // namespace ds_test

#endif

