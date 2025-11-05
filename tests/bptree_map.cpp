/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-17
 *===============================*/

#include "ds/bptree_map.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include <cstdlib>

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
    map.remove(i);
    REQUIRE(i + map.get_size() == N);

    pointer = map[i];
    REQUIRE(pointer == nullptr);

    expected = map.at(i);
    if (i == N) {
      REQUIRE(expected.error() == ds::error_codes::CONTAINER_EMPTY);
    } else {
      REQUIRE(expected.error() == ds::error_codes::NOT_FOUND);
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
    map.remove(i);
    REQUIRE(i == map.get_size() + 1);

    pointer = map[i];
    REQUIRE(pointer == nullptr);

    expected = map.at(i);
    if (i == 1) {
      REQUIRE(expected.error() == ds::error_codes::CONTAINER_EMPTY);
    } else {
      REQUIRE(expected.error() == ds::error_codes::NOT_FOUND);
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
    map.remove(key);
    REQUIRE(map.get_size() + (2 * i) + 1 == N);

    pointer = map[key];
    REQUIRE(pointer == nullptr);

    expected = map.at(key);
    REQUIRE(expected.error() == ds::error_codes::NOT_FOUND);

    // --- //

    key = N / 2 + i;
    map.remove(key);
    REQUIRE(map.get_size() + 2 * (i + 1) == N);

    pointer = map[key];
    REQUIRE(pointer == nullptr);

    expected = map.at(key);
    if (i == N / 2 - 1) {
      REQUIRE(expected.error() == ds::error_codes::CONTAINER_EMPTY);
    } else {
      REQUIRE(expected.error() == ds::error_codes::NOT_FOUND);
    }
  }

  REQUIRE(map.is_empty());
}

const ds::usize N_SMALL = 16;
const ds::usize N_BIG = 512;

TEST_CASE("Check if i32 keys", "[bptree_map]") {
  SECTION("Small N") {
    test_ascending<ds::i32>(N_SMALL);
    test_descending<ds::i32>(N_SMALL);
    test_centered<ds::i32>(N_SMALL);
  }

  SECTION("Big N") {
    test_ascending<ds::i32>(N_BIG);
    test_descending<ds::i32>(N_BIG);
    test_centered<ds::i32>(N_BIG);
  }
}

TEST_CASE("Check if i64 keys", "[bptree_map]") {
  SECTION("Small N") {
    test_ascending<ds::i64>(N_SMALL);
    test_descending<ds::i64>(N_SMALL);
    test_centered<ds::i64>(N_SMALL);
  }

  SECTION("Big N") {
    test_ascending<ds::i64>(N_BIG);
    test_descending<ds::i64>(N_BIG);
    test_centered<ds::i64>(N_BIG);
  }
}

TEST_CASE("Check if u32 keys", "[bptree_map]") {
  SECTION("Small N") {
    test_ascending<ds::u32>(N_SMALL);
    test_descending<ds::u32>(N_SMALL);
    test_centered<ds::u32>(N_SMALL);
  }

  SECTION("Big N") {
    test_ascending<ds::u32>(N_BIG);
    test_descending<ds::u32>(N_BIG);
    test_centered<ds::u32>(N_BIG);
  }
}

TEST_CASE("Check if u64", "[bptree_map]") {
  SECTION("Small N") {
    test_ascending<ds::u64>(N_SMALL);
    test_descending<ds::u64>(N_SMALL);
    test_centered<ds::u64>(N_SMALL);
  }

  SECTION("Big N") {
    test_ascending<ds::u64>(N_BIG);
    test_descending<ds::u64>(N_BIG);
    test_centered<ds::u64>(N_BIG);
  }
}
