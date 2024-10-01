/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#include "ds/hash_map.hpp"
#include "./main.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds-error/types.hpp"
#include "ds/hash.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <unordered_map>

using namespace ds_test;

TEST_CASE("hash_map", "ds") { // NOLINT
  ds::opt_err err{};

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

  SECTION("10,000") {
    const ds::i32 N = 10'000;
    std::array<ds::i32, N> numbers{};
    for (int i = 0; i < N; ++i) {
      numbers[i] = std::rand();
    }

    BENCHMARK("ds::hash_map ds::hash 10,000") {
      ds::hash_map<ds::i32, ds::i32, ds::hash<ds::i32>> map{};
      for (ds::i32 i = 0; i < N; ++i) {
        err = map.insert(numbers[i], i);
      }

      for (ds::i32 i = 0; i < N; ++i) {
        static_cast<void>(map[i]);
      }
    };

    BENCHMARK("ds::hash_map std::hash 10,000") {
      ds::hash_map<ds::i32, ds::i32, std::hash<ds::i32>> map{};
      for (ds::i32 i = 0; i < N; ++i) {
        err = map.insert(numbers[i], i);
      }

      for (ds::i32 i = 0; i < N; ++i) {
        static_cast<void>(map[i]);
      }
    };

    BENCHMARK("std::unordered_map ds::hash 10,000") {
      std::unordered_map<ds::i32, ds::i32, ds::hash<ds::i32>> map{};
      for (ds::i32 i = 0; i < N; ++i) {
        map.insert({numbers[i], i});
      }

      for (ds::i32 i = 0; i < N; ++i) {
        static_cast<void>(map[i]);
      }
    };

    BENCHMARK("std::unordered_map std::hash 10,000") {
      std::unordered_map<ds::i32, ds::i32> map{};
      for (ds::i32 i = 0; i < N; ++i) {
        map.insert({numbers[i], i});
      }

      for (ds::i32 i = 0; i < N; ++i) {
        static_cast<void>(map[i]);
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

