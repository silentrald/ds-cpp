/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-10-07
 *===============================*/

#include "ds/bptree_map.hpp"
#include "catch2/benchmark/catch_benchmark.hpp"
#include "catch2/benchmark/catch_chronometer.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/types.hpp"
#include <array>
#include <map>

template <typename T, ds::usize N> void benchmark() {
  std::array<ds::i32, N> random{};
  for (ds::i32 i = 0; i < N; ++i) {
    random[i] = std::rand(); // NOLINT
  }

  BENCHMARK("insert ds::bptree_map") {
    ds::error_code error_code{};
    ds::bptree_map<ds::i32, ds::i32> map{};

    for (ds::i32 i = 0; i < N; ++i) {
      error_code = map.insert(random[i], i);
    }
  };

  BENCHMARK("insert std::map") {
    std::map<ds::i32, ds::i32> map{};
    for (ds::i32 i = 0; i < N; ++i) {
      map[random[i]] = i;
    }
  };

  BENCHMARK_ADVANCED("access ds::bptree_map")
  (Catch::Benchmark::Chronometer meter) {
    ds::bptree_map<ds::i32, ds::i32> map{};
    ds::error_code error_code{};
    for (ds::i32 i = 0; i < N; ++i) {
      error_code = map.insert(random[i], i);
    }

    meter.measure([&map, &random]() {
      for (ds::i32 i = 0; i < N; ++i) {
        static_cast<void>(map[random[i]]);
      }
    });
  };

  BENCHMARK_ADVANCED("access std::map")(Catch::Benchmark::Chronometer meter) {
    std::map<ds::i32, ds::i32> map{};
    for (ds::i32 i = 0; i < N; ++i) {
      map[random[i]] = i;
    }

    meter.measure([&map, &random]() {
      for (ds::i32 i = 0; i < N; ++i) {
        static_cast<void>(map[random[i]]);
      }
    });
  };

  BENCHMARK_ADVANCED("iterator ds::bptree_map")
  (Catch::Benchmark::Chronometer meter) {
    ds::bptree_map<ds::i32, ds::i32> map{};
    ds::error_code error_code{};
    for (ds::i32 i = 0; i < N; ++i) {
      error_code = map.insert(random[i], i);
    }

    meter.measure([&map]() {
      T key;
      for (auto it = map.cbegin(); it != map.cend(); ++it) {
        key = it.key();
      }
    });
  };

  BENCHMARK_ADVANCED("iterator std::map")(Catch::Benchmark::Chronometer meter) {
    std::map<ds::i32, ds::i32> map{};
    for (ds::i32 i = 0; i < N; ++i) {
      map[random[i]] = i;
    }

    meter.measure([&map, &random]() {
      T key;
      for (const auto pair : map) {
        key = pair.first;
      }
    });
  };

  BENCHMARK_ADVANCED("remove ds::bptree_map")
  (Catch::Benchmark::Chronometer meter) {
    ds::bptree_map<ds::i32, ds::i32> map{};
    ds::error_code error_code{};
    for (ds::i32 i = 0; i < N; ++i) {
      error_code = map.insert(random[i], i);
    }

    meter.measure([&map, &random]() {
      for (ds::i32 i = 0; i < N; ++i) {
        map.remove(random[i]);
      }
    });
  };

  BENCHMARK_ADVANCED("erase std::map")(Catch::Benchmark::Chronometer meter) {
    std::map<ds::i32, ds::i32> map{};
    for (ds::i32 i = 0; i < N; ++i) {
      map[random[i]] = i;
    }

    meter.measure([&map, &random]() {
      for (ds::i32 i = 0; i < N; ++i) {
        map.erase(random[i]);
      }
    });
  };
}

TEST_CASE("bptree_map benchmarks", "[!benchmark][bptree_map]") {
  SECTION("i32 ; 100") {
    benchmark<ds::i32, 100>();
  }

  SECTION("i64 ; 100") {
    benchmark<ds::i64, 100>();
  }

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
