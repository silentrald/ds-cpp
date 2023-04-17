/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#include "ds/hash_map.hpp"
#include "./main.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds-error/types.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include <cstdio>
#include <unordered_map>

using namespace ds_test;

TEST_CASE("hash_map", "ds") { // NOLINT
  ds::err_code error{};

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

  SECTION("1000") {
    ds::i32 n = 10000;
    ds::err_code err = 0;
    BENCHMARK("ds::hash_map 1000") {
      ds::hash_map<ds::i32, ds::i32> map{};
      for (ds::i32 i = 0; i < n; ++i) {
        err = map.insert(i, i);
      }

      for (ds::i32 i = 0; i < n; ++i) {
        err = *map[i];
      }
    };

    BENCHMARK("std::unordered_map 1000") {
      std::unordered_map<ds::i32, ds::i32> map{};
      for (ds::i32 i = 0; i < n; ++i) {
        map.insert({i, i});
      }

      for (ds::i32 i = 0; i < n; ++i) {
        err = map[i];
      }
    };
  }

  SECTION("Key String and Value String") {
    ds::hash_map<ds::string, ds::string> map{};

    ds::string hi{};

    CHECK(handle_error(map.insert("hello", "hi")));
    CHECK(handle_error(map.insert("hi", "hello")));
    CHECK(handle_error(map.insert(hi, "SUCC")));
  }
}

