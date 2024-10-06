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
#include <cstdlib>
#include <map>

// NOTE: Test class, string class

template <typename T> inline void test_ascending(const ds::usize N) {
  ds::bptree_map<T, T> map{};
  T* pointer = nullptr;
  ds::error_code error_code{};
  ds::expected<T*, ds::error_code> expected{};

  for (T i = 1; i <= N; ++i) {
    error_code = map.insert(i, i);
    REQUIRE(ds_test::handle_error(error_code));
    REQUIRE(map.get_size() == i);

    pointer = map[i];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(i);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);
  }

  REQUIRE_FALSE(map.is_empty());

  auto iterator = map.begin();
  T previous = iterator.key();
  for (++iterator; iterator != map.end(); ++iterator) {
    REQUIRE(iterator.key() > previous);
    previous = iterator.key();
  }

  for (T i = 1; i <= N; ++i) {
    map.erase(i);
    REQUIRE(i + map.get_size() == N);

    pointer = map[i];
    REQUIRE(pointer == nullptr);

    expected = map.at(i);
    if (i == N) {
      REQUIRE(expected.error() == ds::error_code::CONTAINER_EMPTY);
    } else {
      REQUIRE(expected.error() == ds::error_code::NOT_FOUND);
    }
  }

  REQUIRE(map.is_empty());
}

template <typename T> inline void test_descending(const ds::usize N) {
  ds::bptree_map<T, T> map{};
  T* pointer = nullptr;
  ds::error_code error_code{};
  ds::expected<T*, ds::error_code> expected{};

  for (T i = N; i > 0; --i) {
    error_code = map.insert(i, i);
    REQUIRE(ds_test::handle_error(error_code));
    REQUIRE(map.get_size() + i == N + 1);

    pointer = map[i];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(i);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);
  }

  REQUIRE_FALSE(map.is_empty());

  auto iterator = map.begin();
  T previous = iterator.key();
  for (++iterator; iterator != map.end(); ++iterator) {
    REQUIRE(iterator.key() > previous);
    previous = iterator.key();
  }

  for (T i = N; i > 0; --i) {
    map.erase(i);
    REQUIRE(i == map.get_size() + 1);

    pointer = map[i];
    REQUIRE(pointer == nullptr);

    expected = map.at(i);
    if (i == 1) {
      REQUIRE(expected.error() == ds::error_code::CONTAINER_EMPTY);
    } else {
      REQUIRE(expected.error() == ds::error_code::NOT_FOUND);
    }
  }

  REQUIRE(map.is_empty());
}

// Try to start from N / 2 then outwards
// Eg. N = 1000 ; 499, 500, 498, 501, 497, ..., 0, 999
template <typename T> inline void test_centered(const ds::usize N) {
  ds::bptree_map<T, T> map{};
  T* pointer = nullptr;
  ds::error_code error_code{};
  ds::expected<T*, ds::error_code> expected{};

  T key = 0;
  for (T i = 0; i < N / 2; ++i) {
    key = N / 2 - i - 1;

    error_code = map.insert(key, i);
    REQUIRE(ds_test::handle_error(error_code));
    REQUIRE(map.get_size() == (i * 2) + 1);

    pointer = map[key];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(key);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);

    // --- //

    key = N / 2 + i;

    error_code = map.insert(key, i);
    REQUIRE(ds_test::handle_error(error_code));
    REQUIRE(map.get_size() == (i + 1) * 2);

    pointer = map[key];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i);

    expected = map.at(key);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i);
  }

  REQUIRE_FALSE(map.is_empty());

  auto iterator = map.begin();
  key = iterator.key();
  for (++iterator; iterator != map.end(); ++iterator) {
    REQUIRE(iterator.key() > key);
    key = iterator.key();
  }

  for (T i = 0; i < N / 2; ++i) {
    key = N / 2 - i - 1;
    map.erase(key);
    REQUIRE(map.get_size() + (2 * i) + 1 == N);

    pointer = map[key];
    REQUIRE(pointer == nullptr);

    expected = map.at(key);
    REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

    // --- //

    key = N / 2 + i;
    map.erase(key);
    REQUIRE(map.get_size() + 2 * (i + 1) == N);

    pointer = map[key];
    REQUIRE(pointer == nullptr);

    expected = map.at(key);
    if (i == N / 2 - 1) {
      REQUIRE(expected.error() == ds::error_code::CONTAINER_EMPTY);
    } else {
      REQUIRE(expected.error() == ds::error_code::NOT_FOUND);
    }
  }

  REQUIRE(map.is_empty());
}

template <typename T, ds::usize N> void benchmark() {
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

const ds::usize SIZE = 256;

TEST_CASE("Check if i32 keys", "[bptree_map][i32]") {
  test_ascending<ds::i32>(SIZE);
  test_descending<ds::i32>(SIZE);
  test_centered<ds::i32>(SIZE);
}

TEST_CASE("Check if i64 keys", "[bptree_map][i64]") {
  test_ascending<ds::i64>(SIZE);
  test_descending<ds::i64>(SIZE);
  test_centered<ds::i64>(SIZE);
}

TEST_CASE("Check if u32 keys", "[bptree_map][u32]") {
  test_ascending<ds::u32>(SIZE);
  test_descending<ds::u32>(SIZE);
  test_centered<ds::u32>(SIZE);
}

TEST_CASE("Check if u64", "[bptree_map][u64]") {
  test_ascending<ds::u64>(SIZE);
  test_descending<ds::u64>(SIZE);
  test_centered<ds::u64>(SIZE);
}

TEST_CASE("Benchmarks", "[bptree_map]") {
  SECTION("i32 ; 1,000") {
    benchmark<ds::i32, 1'000>();
  }

  SECTION("i64 ; 1,000") {
    benchmark<ds::i64, 1'000>();
  }

  SECTION("i32 ; 10,000") {
    benchmark<ds::i32, 10'000>();
  }

  SECTION("i64 ; 10,000") {
    benchmark<ds::i64, 10'000>();
  }
}
