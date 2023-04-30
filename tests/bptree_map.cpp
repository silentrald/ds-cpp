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
#include "ds/types.hpp"
#include <map>

TEST_CASE("bptree_map", "ds") { // NOLINT
  ds::i32 n = 10000;
  ds::opt_err err{};

  BENCHMARK("ds::bptree_map") {
    ds::bptree_map<ds::i32, ds::i32> map{};
    for (ds::i32 i = 0; i < n; ++i) {
      err = map.insert(i, i);
    }

    for (ds::i32 i = 0; i < n; ++i) {
      static_cast<void>(map[i]);
    }
  };

  BENCHMARK("std::map") {
    std::map<ds::i32, ds::i32> map{};
    for (ds::i32 i = 0; i < n; ++i) {
      map[i] = i;
    }

    for (ds::i32 i = 0; i < n; ++i) {
      static_cast<void>(map[i]);
    }
  };
};

