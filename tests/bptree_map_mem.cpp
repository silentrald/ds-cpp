/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-10-06
 *===============================*/

// NOTE: Mainly for valgrind checking

#include "ds/bptree_map.hpp"
#include "types.hpp"
#include <cstdio>
#include <cstdlib>

// Only even
const ds::i32 N = 100'000'000;

template <typename T> bool test_ascending() noexcept {
  printf("Testing ascending to %lu bytes\n", sizeof(T));

  ds::bptree_map<T, ds::i32> map{};
  ds::error_code error_code{};
  ds::expected<ds::i32*, ds::error_code> expected{};

  for (T i = 0; i < N; ++i) {
    error_code = map.insert(i, i);
    if (ds::is_error(error_code)) {
      printf("Can't insert memory: %u\n", error_code);
      return false;
    }

    expected = map.at(i);
    if (ds::is_error(expected)) {
      printf("Invalid implementation of bptree_map: %u\n", error_code);
      return false;
    }

    if (*expected == nullptr) {
      printf("Nullptr value\n");
      return false;
    }

    if (**expected != i) {
      printf("Not equal %x != %x\n", **expected, i); // NOLINT
      return false;
    }
  }

  for (T i = 0; i < N; ++i) {
    map.erase(i);

    if (i + map.get_size() != N - 1) {
      printf("Couldn't delete key\n");
      return false;
    }
  }

  return true;
}

template <typename T> bool test_descending() noexcept {
  printf("Testing descending to %lu bytes\n", sizeof(T));

  ds::bptree_map<T, ds::i32> map{};
  ds::error_code error_code{};
  ds::expected<ds::i32*, ds::error_code> expected{};

  for (T i = N - 1; i > -1; --i) {
    error_code = map.insert(i, i);
    if (ds::is_error(error_code)) {
      printf("Can't insert memory: %u\n", error_code);
      return false;
    }

    expected = map.at(i);
    if (ds::is_error(expected)) {
      printf("Invalid implementation of bptree_map: %u\n", error_code);
      return false;
    }

    if (*expected == nullptr) {
      printf("Nullptr value\n");
      return false;
    }

    if (**expected != i) {
      printf("Not equal %x != %x\n", **expected, i); // NOLINT
      return false;
    }
  }

  for (T i = N - 1; i > -1; --i) {
    map.erase(i);

    if (i != map.get_size()) {
      printf("Couldn't delete key\n");
      return false;
    }
  }

  return true;
}

// Try to start from N / 2 then outwards
// Eg. N = 1000 ; 499, 500, 498, 501, 497, ..., 0, 999
template <typename T> bool test_centered() noexcept {
  printf("Testing centered to %lu bytes\n", sizeof(T));

  ds::bptree_map<T, ds::i32> map{};
  ds::error_code error_code{};
  ds::expected<ds::i32*, ds::error_code> expected{};

  for (T i = 0; i < N; ++i) {
    error_code = map.insert(i, i);
    if (ds::is_error(error_code)) {
      printf("Can't insert memory: %u\n", error_code);
      return false;
    }

    expected = map.at(i);
    if (ds::is_error(expected)) {
      printf("Invalid implementation of bptree_map: %u\n", error_code);
      return false;
    }

    if (*expected == nullptr) {
      printf("Nullptr value\n");
      return false;
    }

    if (**expected != i) {
      printf("Not equal %x != %x\n", **expected, i); // NOLINT
      return false;
    }
  }

  for (T i = 0; i < N / 2; ++i) {
    map.erase(N / 2 - i - 1);
    map.erase(N / 2 + i);

    if ((i * 2) + map.get_size() != N - 2) {
      printf("Couldn't delete key \n");
      return false;
    }
  }

  return true;
}

int main() {
  if (!test_ascending<ds::i32>() || !test_descending<ds::i32>() ||
      !test_centered<ds::i32>() || !test_ascending<ds::i64>() ||
      !test_descending<ds::i64>() || !test_centered<ds::i64>()) {
    return -1;
  }

  return 0;
}
