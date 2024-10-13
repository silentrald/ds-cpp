/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_TEST_MAIN_HPP
#define DS_TEST_MAIN_HPP

#include "catch2/catch_message.hpp"
#include "ds/types.hpp"

namespace ds_test {

[[nodiscard]] bool handle_error(ds::error_code error) noexcept;

template <typename T>
bool handle_error(const ds::expected<T, ds::error_code>& exp) noexcept {
  if (exp) {
    return true;
  }

  UNSCOPED_INFO("Got error_code: " << exp.error());
  return false;
}

class Test {
public:
  Test(const Test& rhs) = delete;
  Test& operator=(const Test& rhs) = delete;

  // === Initializers === //

  Test() noexcept = default;
  explicit Test(ds::isize* pointer) noexcept;
  Test(ds::isize* pointer, ds::isize value) noexcept;

  // Only call when destroy is called
  [[nodiscard]] bool reinit(ds::isize* pointer) noexcept;

  // Only call when destroy is called
  [[nodiscard]] bool reinit(ds::isize* pointer, ds::isize value) noexcept;

  // === Move/Copy === //

  Test(Test&& rhs) noexcept;
  Test& operator=(Test&& other) noexcept;
  ds::error_code copy(const Test& other) noexcept;

  // === Destructor === //

  ~Test() noexcept;
  void destroy() noexcept;

  // === Setter/Getter === //

  void set_value(ds::isize value) noexcept;
  [[nodiscard]] ds::isize get_value() const noexcept;

  // === Checkers === //

  [[nodiscard]] bool operator==(const Test& rhs) const noexcept;
  [[nodiscard]] bool operator==(ds::isize rhs) const noexcept;
  [[nodiscard]] bool operator!=(const Test& rhs) const noexcept;
  [[nodiscard]] bool operator!=(ds::isize rhs) const noexcept;
  friend bool operator==(ds::isize lhs, const Test& rhs) noexcept;
  friend bool operator!=(ds::isize lhs, const Test& rhs) noexcept;

private:
  ds::isize* pointer = nullptr;
  ds::isize value = 0;
};

} // namespace ds_test

#endif
