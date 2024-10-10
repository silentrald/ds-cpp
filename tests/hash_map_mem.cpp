/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-10-08
 *===============================*/

// NOTE: Mainly for valgrind checking

#include "ds/hash_map.hpp"
#include "ds/types.hpp"
#include <cstdio>
#include <cstdlib>

// Only even
const ds::i32 N_BIG = 1'000'000;
const ds::i32 N_SMALL = 16;

int main() {
  ds::hash_map<ds::i32, ds::i32> map{};
  ds::i32* pointer = nullptr;
  ds::error_code error_code{};
  ds::expected<ds::i32*, ds::error_code> expected{};

  for (ds::i32 i = 0; i < N_BIG; ++i) {
    error_code = map.insert(i, i * 2);
    if (ds::is_error(error_code)) {
      printf("Can't insert memory: %u\n", error_code);
      return -1;
    }
  }

  for (ds::i32 i = 0; i < N_BIG; ++i) {
    expected = map.at(i);
    if (ds::is_error(expected)) {
      printf("Can't find value: %u\n", error_code);
      return -1;
    }

    if (**expected != i * 2) {
      printf("Invalid expected value\n");
      return -1;
    }

    pointer = map[i];
    if (pointer == nullptr) {
      printf("Can't find value: %u\n", error_code);
      return -1;
    }

    if (*pointer != i * 2) {
      printf("Invalid pointer value\n");
      return -1;
    }
  }

  return 0;
}
