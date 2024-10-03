/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-17
 *===============================*/

#include "ds/bptree_map.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"
#include "catch2/catch_test_macros.hpp"
#include "main.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>

// NOTE: Test class, string class

TEST_CASE("Check if i32 keys are sorted", "[bptree_map][i32]") {
  ds::error_code error_code{};
  ds::usize* pointer = nullptr;
  ds::expected<ds::usize*, ds::error_code> expected{};

  const ds::usize N = 64;
  ds::bptree_map<ds::i32, ds::usize> map{};

  ds::i32 tmp = 0;
  for (ds::usize i = 0; i < N; ++i) {
    tmp = std::rand();
    error_code = map.insert(tmp, i);
    REQUIRE(ds_test::handle_error(error_code));

    pointer = map[tmp];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(tmp);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);
  }

  ds::i32 previous = INT32_MIN;
  error_code = map.insert(previous, 0);
  REQUIRE(ds_test::handle_error(error_code));

  SECTION("Check if sorted (iterators)") {
    auto iterator = map.begin();
    for (++iterator; iterator != map.end(); ++iterator) {
      REQUIRE(iterator.key() > previous);
      previous = iterator.key();
    }
  }
}

TEST_CASE("Check if i64 keys are sorted", "[bptree_map][i64]") {
  ds::error_code error_code{};
  ds::usize* pointer = nullptr;
  ds::expected<ds::usize*, ds::error_code> expected{};

  const ds::usize N = 64;
  ds::bptree_map<ds::i64, ds::usize> map{};

  ds::i64 tmp = 0;
  for (ds::usize i = 0; i < N; ++i) {
    tmp = std::rand();
    error_code = map.insert(tmp, i);
    REQUIRE(ds_test::handle_error(error_code));

    pointer = map[tmp];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(tmp);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);
  }

  ds::i64 previous = INT64_MIN;
  error_code = map.insert(previous, 0);
  REQUIRE(ds_test::handle_error(error_code));

  SECTION("Check if sorted (iterators)") {
    auto iterator = map.begin();
    for (++iterator; iterator != map.end(); ++iterator) {
      REQUIRE(iterator.key() > previous);
      previous = iterator.key();
    }
  }
}

TEST_CASE("Check if u32 keys are sorted", "[bptree_map][u32]") {
  ds::error_code error_code{};
  ds::usize* pointer = nullptr;
  ds::expected<ds::usize*, ds::error_code> expected{};

  const ds::usize N = 64;
  ds::bptree_map<ds::u32, ds::usize> map{};

  ds::u32 tmp = 0;
  for (ds::usize i = 0; i < N; ++i) {
    tmp = std::rand();
    error_code = map.insert(tmp, i);
    REQUIRE(ds_test::handle_error(error_code));

    pointer = map[tmp];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(tmp);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);
  }

  ds::u32 previous = 0U;
  error_code = map.insert(previous, 0);
  REQUIRE(ds_test::handle_error(error_code));

  SECTION("Check if sorted (iterators)") {
    auto iterator = map.begin();
    for (++iterator; iterator != map.end(); ++iterator) {
      REQUIRE(iterator.key() > previous);
      previous = iterator.key();
    }
  }
}

TEST_CASE("Check if u64 elements are sorted", "[bptree_map][u64]") {
  ds::error_code error_code{};
  ds::u64* pointer = nullptr;
  ds::expected<ds::u64*, ds::error_code> expected{};

  const ds::usize N = 64;
  ds::bptree_map<ds::u64, ds::usize> map{};

  ds::u64 tmp = 0;
  for (ds::usize i = 0; i < N; ++i) {
    tmp = std::rand();
    error_code = map.insert(tmp, i);
    REQUIRE(ds_test::handle_error(error_code));

    pointer = map[tmp];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(tmp);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);
  }

  ds::u64 previous = 0U;
  error_code = map.insert(previous, 0);
  REQUIRE(ds_test::handle_error(error_code));

  SECTION("Check if sorted (iterators)") {
    auto iterator = map.begin();
    for (++iterator; iterator != map.end(); ++iterator) {
      REQUIRE(iterator.key() > previous);
      previous = iterator.key();
    }
  }
}

TEST_CASE("Benchmarks", "[bptree_map]"){
    // NOLINT

    SECTION("1,000"){const ds::i32 N = 1'000;
std::array<ds::i32, N> random{};

for (ds::i32 i = 0; i < N; ++i) {
  random[i] = std::rand();
}

BENCHMARK("ds::bptree_map") {
  ds::error_code error_code{};
  ds::bptree_map<ds::i32, ds::i32> map{};

  for (ds::i32 i = 0; i < N; ++i) {
    error_code = map.insert(random[i], i);
  }

  for (ds::i32 i = 0; i < N; ++i) {
    static_cast<void>(map[i]);
  }
};

BENCHMARK("std::map") {
  std::map<ds::i32, ds::i32> map{};
  for (ds::i32 i = 0; i < N; ++i) {
    map[random[i]] = i;
  }

  for (ds::i32 i = 0; i < N; ++i) {
    static_cast<void>(map[i]);
  }
};
}

SECTION("10,000") {
  const ds::usize N = 10'000;
  std::array<ds::i32, N> random{};

  for (ds::usize i = 0; i < N; ++i) {
    random[i] = std::rand();
  }

  BENCHMARK("ds::bptree_map") {
    ds::error_code error_code{};
    ds::bptree_map<ds::i32, ds::i32> map{};

    for (ds::i32 i = 0; i < N; ++i) {
      error_code = map.insert(random[i], i);
    }

    for (ds::i32 i = 0; i < N; ++i) {
      static_cast<void>(map[i]);
    }
  };

  BENCHMARK("std::map") {
    std::map<ds::i32, ds::i32> map{};
    for (ds::i32 i = 0; i < N; ++i) {
      map[random[i]] = i;
    }

    for (ds::i32 i = 0; i < N; ++i) {
      static_cast<void>(map[i]);
    }
  };
}
}
;
