/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#include "main.hpp"

bool ds_test::handle_error(ds::error_code error) noexcept {
  if (!error) {
    return true;
  }

  UNSCOPED_INFO("Got error_code: " << error);
  return false;
}
