/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 * Updated: 2022-12-29
 *===============================*/

#include "ds/hash_map.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/string.hpp"
#include "error.hpp"
#include "test.hpp"
#include "types.hpp"

using namespace ds_test;

TEST_CASE("hash_map", "ds") { // NOLINT
  ds::opt_err error{};

  SECTION("Primitive Key and Value") {
    ds::hash_map<ds::i32, ds::i32> map{};

    CHECK(handle_error(map.insert(100, 200)));
    CHECK(handle_error(map.insert(200, 500)));
    CHECK(handle_error(map.insert(100, 300)));

    for (auto it = map.cbegin(); it != map.cend(); ++it) {
      REQUIRE(map.contains(it.key()));
      REQUIRE(*map[it.key()] == it.value());
    }
  }

  SECTION("Key String and Value String") {
    ds::hash_map<ds::string, ds::string> map{};

    ds::string hi{};

    CHECK(handle_error(map.insert("hello", "hi")));
    CHECK(handle_error(map.insert("hi", "hello")));
    CHECK(handle_error(map.insert(hi, "SUCC")));
  }
}

