/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-12-02
 *===============================*/

#include "ds/sort.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/vector.hpp"
#include "main.hpp"
#include <cstdlib>

TEST_CASE("bubble sort", "[sort]") {
  ds::vector<ds::i32> numbers{};
  const ds::usize SIZE = 1'000U;
  const ds::usize MAX_NUMBER = 100'000U;
  REQUIRE(ds_test::handle_error(numbers.resize(SIZE)));

  // First iteration should contain zero
  while (ds::is_sorted(numbers.get_data(), numbers.get_size())) {
    for (ds::usize i = 0; i < SIZE; ++i) {
      numbers[i] = std::rand() % MAX_NUMBER; // NOLINT
    }
  }

  ds::bubble_sort(numbers.get_data(), numbers.get_size());
  REQUIRE(ds::is_sorted(numbers.get_data(), numbers.get_size()));
}

TEST_CASE("merge sort", "[sort]") {
  ds::vector<ds::i32> numbers{};
  const ds::usize SIZE_START = 16U;
  const ds::usize SIZE_END = 2'049U;
  const ds::usize MAX_NUMBER = 100'000U;

  REQUIRE(ds_test::handle_error(numbers.resize(SIZE_END)));

  // First iteration should contain zero
  for (ds::usize size = SIZE_START; size <= SIZE_END; ++size) {
    for (ds::usize i = 0; i < size; ++i) {
      numbers[i] = std::rand() % MAX_NUMBER; // NOLINT
    }

    ds::merge_sort(numbers.get_data(), numbers.get_size());
    REQUIRE(ds::is_sorted(numbers.get_data(), numbers.get_size()));
  }
}
