/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-17
 *===============================*/

#include "ds/bptree_map.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "main.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>

TEST_CASE("Check if elements are sorted", "[bptree_map]") {
  ds::error_code error_code{};
  ds::i32* pointer = nullptr;
  ds::expected<ds::i32*, ds::error_code> expected{};

  SECTION("using i32 primitive") {
    const ds::i32 N = 32;
    ds::bptree_map<ds::i32, ds::i32> map{};

    ds::i32 tmp = 0;
    for (ds::i32 i = 0; i < N; ++i) {
      printf("%d > Inserting: %d\n", i, tmp);

      tmp = std::rand();
      error_code = map.insert(tmp, tmp);
      REQUIRE(ds_test::handle_error(error_code));

      pointer = map[tmp];
      REQUIRE(pointer != nullptr);
      REQUIRE(*pointer == tmp);

      expected = map.at(tmp);
      REQUIRE(ds_test::handle_error(expected));
      REQUIRE(**expected == tmp);
    }

    ds::i32 previous = INT32_MIN;
    error_code = map.insert(previous, 0);
    REQUIRE(ds_test::handle_error(error_code));

    SECTION("Check if sorted (iterators)") {
      for (auto iterator = map.begin()++; iterator != map.end(); ++iterator) {
        REQUIRE(iterator.key() > previous);
        previous = iterator.key();
      }
    }

  }

}

// TEST_CASE("Benchmarks", "[bptree_map]") { // NOLINT
//
//   SECTION("10,000") {
//     const ds::i32 N = 10'000;
//     std::array<ds::i32, N> random{};
//
//     for (ds::i32 i = 0; i < N; ++i) {
//       random[i] = std::rand();
//     }
//
//     BENCHMARK("ds::bptree_map") {
//       ds::error_code error_code{};
//       ds::bptree_map<ds::i32, ds::i32> map{};
//
//       for (ds::i32 i = 0; i < N; ++i) {
//         error_code = map.insert(random[i], i);
//       }
//
//       for (ds::i32 i = 0; i < N; ++i) {
//         static_cast<void>(map[i]);
//       }
//     };
//
//     BENCHMARK("std::map") {
//       std::map<ds::i32, ds::i32> map{};
//       for (ds::i32 i = 0; i < N; ++i) {
//         map[random[i]] = i;
//       }
//
//       for (ds::i32 i = 0; i < N; ++i) {
//         static_cast<void>(map[i]);
//       }
//     };
//   }
//
//   SECTION("100,000") {
//     const ds::i32 N = 100'000;
//     std::array<ds::i32, N> random{};
//
//     for (ds::i32 i = 0; i < N; ++i) {
//       random[i] = std::rand();
//     }
//
//     BENCHMARK("ds::bptree_map") {
//       ds::error_code error_code{};
//       ds::bptree_map<ds::i32, ds::i32> map{};
//
//       for (ds::i32 i = 0; i < N; ++i) {
//         error_code = map.insert(random[i], i);
//       }
//
//       for (ds::i32 i = 0; i < N; ++i) {
//         static_cast<void>(map[i]);
//       }
//     };
//
//     BENCHMARK("std::map") {
//       std::map<ds::i32, ds::i32> map{};
//       for (ds::i32 i = 0; i < N; ++i) {
//         map[random[i]] = i;
//       }
//
//       for (ds::i32 i = 0; i < N; ++i) {
//         static_cast<void>(map[i]);
//       }
//     };
//   }
// };

