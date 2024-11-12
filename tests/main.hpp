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

} // namespace ds_test

#endif
