/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-06
 *===============================*/

#include "ds/unique_ptr.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include <cstdlib>

TEST_CASE("primitive test", "[unique_ptr]") {
  ds::unique_ptr<ds::i32> smart{};
  ds::error_code error_code{};

  ds::i32 tmp = 0;

  for (ds::i32 i = 0; i < 16; ++i) {
    tmp = std::rand();
    error_code = smart.set(tmp);
    REQUIRE(ds_test::handle_error(error_code));
    REQUIRE(*smart == tmp);

    if (i & 1) {
      smart.destroy();
    } else {
      REQUIRE(smart.release() == tmp);
    }
    REQUIRE_FALSE(smart);
  }
}

TEST_CASE("class test", "[unique_ptr]") {
  ds::isize counter = 0;
  ds::unique_ptr<ds_test::Test> smart{};
  ds::error_code error_code{};

  ds::i32 tmp = 0;

  for (ds::i32 i = 0; i < 16; ++i) {
    tmp = std::rand();
    error_code = smart.set(ds_test::Test{&counter, tmp});
    REQUIRE(ds_test::handle_error(error_code));

    REQUIRE(counter == 1);
    REQUIRE(*smart == tmp);

    if (i & 1) {
      smart.destroy();
    } else {
      REQUIRE(smart.release() == tmp);
    }

    REQUIRE_FALSE(smart);
    REQUIRE(counter == 0);
  }
}
