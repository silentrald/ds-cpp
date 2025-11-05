/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#include "ds/hash_set.hpp"
#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/prime.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include <cstdlib>
#include <cstring>

// For average collision
const ds::usize SIZE = 32;

// For bad collision
const ds::usize PRIME = ds::get_first_prime();
const ds::usize START = 1;
const ds::usize END = 10;
const ds::usize OFFSET = 5;

// === Inserting === //

template <typename Key, bool AverageCollision, bool Check = false>
ds::expected<ds::hash_set<Key>, ds::error_code> create_hash_set() {
  ds::hash_set<Key> set{};
  ds::error_code error_code{};

  if constexpr (AverageCollision) {
    for (Key i = 0, counter = 0; i < 10; ++i) {
      for (Key j = i; j < SIZE; j += 10) {
        if constexpr (Check) {
          error_code = set.insert(j);
          REQUIRE(ds_test::handle_error(error_code));
          REQUIRE(set.get_size() == ++counter);
          REQUIRE(set.get_capacity() >= counter);
        } else {
          DS_TRY(set.insert(j), ds::to_unexpected);
        }
      }
    }
  } else {
    for (Key i = START, counter = 0; i < END; ++i) {
      error_code = set.insert(i * PRIME + OFFSET);
      REQUIRE(ds_test::handle_error(error_code));
      REQUIRE(set.get_size() == ++counter);
    }
  }

  return std::move(set);
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> insert average collision", "[hash_set]", ds::i32,
    ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  create_hash_set<key_type, true, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> insert bad collision", "[hash_set]", ds::i32, ds::i64,
    ds::u32, ds::u64
) {
  using key_type = TestType;
  create_hash_set<key_type, false, true>();
}

// === Accessing Existing === //

template <typename Key, bool AverageCollision> void test_access_existing() {
  ds::hash_set<Key> set = ({
    auto expected_set = create_hash_set<Key, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });
  const ds::usize INIT_SIZE = set.get_size();

  Key key = 0;
  for (Key i = 0; i < INIT_SIZE; ++i) {
    if constexpr (AverageCollision) {
      key = i;
    } else {
      key = (i + START) * PRIME + OFFSET;
    }

    REQUIRE(set.contains(key));
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> access existing keys average collision", "[hash_set]",
    ds::i32, ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_access_existing<key_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> existing keys bad collision", "[hash_set]", ds::i32,
    ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_access_existing<key_type, false>();
}

// === Accessing Non-Existing === //

template <typename Key, bool AverageCollision> void test_access_non_existing() {
  ds::hash_set<Key> set = ({
    auto expected_set = create_hash_set<Key, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });
  const ds::usize INIT_SIZE = set.get_size();

  Key key = 0;
  for (Key i = 0; i < INIT_SIZE; ++i) {
    if constexpr (AverageCollision) {
      key = i + 100;
    } else {
      key = (i + START) * PRIME + OFFSET + 1;
    }

    REQUIRE_FALSE(set.contains(key));
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> access non-existing keys average collision",
    "[hash_set]", ds::i32, ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_access_non_existing<key_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> non-existing keys bad collision", "[hash_set]", ds::i32,
    ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_access_non_existing<key_type, false>();
}

// === Removing Existing === //

template <typename Key, bool AverageCollision> void test_remove_existing() {
  ds::hash_set<Key> set = ({
    auto expected_set = create_hash_set<Key, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });
  const ds::usize INIT_SIZE = set.get_size();

  Key key = 0;
  for (ds::usize i = 0, counter = INIT_SIZE; i < 10; ++i) {
    for (ds::usize j = i; j < INIT_SIZE; j += 10) {
      if constexpr (AverageCollision) {
        key = j;
      } else {
        key = (j + START) * PRIME + OFFSET;
      }
      set.remove(key);
      REQUIRE(set.get_size() == --counter);

      REQUIRE_FALSE(set.contains(key));
    }
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> remove existing keys average collision", "[hash_set]",
    ds::i32, ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_remove_existing<key_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> remove existing keys bad collision", "[hash_set]",
    ds::i32, ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_remove_existing<key_type, false>();
}

// === Remove Non Existing === //

template <typename Key, bool AverageCollision> void test_remove_non_existing() {
  ds::hash_set<Key> set = ({
    auto expected_set = create_hash_set<Key, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });
  const ds::usize INIT_SIZE = set.get_size();

  Key key = 0;
  for (ds::usize i = 0, counter = INIT_SIZE; i < 10; ++i) {
    for (ds::usize j = i; j < INIT_SIZE; j += 10) {
      if constexpr (AverageCollision) {
        key = j + 100;
      } else {
        key = (j + START) * PRIME + OFFSET + 1;
      }
      set.remove(key);
      REQUIRE(set.get_size() == counter);
    }
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> remove non-existing keys average collision",
    "[hash_set]", ds::i32, ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_remove_non_existing<key_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> remove non-existing keys bad collision", "[hash_set]",
    ds::i32, ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_remove_non_existing<key_type, false>();
}

// === Reinserting === //

template <typename Key, bool AverageCollision> void test_reinsert() {
  ds::error_code error_code{};
  ds::hash_set<Key> set = ({
    auto expected_set = create_hash_set<Key, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });
  const ds::usize INIT_SIZE = set.get_size();

  Key key = 0;
  for (ds::usize i = 0; i < 10; ++i) {
    for (ds::usize j = i; j < INIT_SIZE; j += 10) {
      if constexpr (AverageCollision) {
        key = j;
      } else {
        key = (j + START) * PRIME + OFFSET;
      }
      error_code = set.insert(key);
      REQUIRE(ds_test::handle_error(error_code));
      REQUIRE(set.get_size() == INIT_SIZE);

      REQUIRE(set.contains(key));
    }
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> reinserting keys average collision", "[hash_set]",
    ds::i32, ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_reinsert<key_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> reinserting keys bad collision", "[hash_set]", ds::i32,
    ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_reinsert<key_type, false>();
}

// === Clearing === //

template <typename Key, bool AverageCollision> void test_clear() {
  ds::hash_set<Key> set = ({
    auto expected_set = create_hash_set<Key, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });
  const ds::usize INIT_SIZE = set.get_size();

  set.clear();
  REQUIRE(set.get_size() == 0);
  REQUIRE(set.is_empty());

  REQUIRE(set.get_capacity() > 0);

  // Check for the old keys
  Key key = 0;
  for (Key i = 0; i < INIT_SIZE; ++i) {
    if constexpr (AverageCollision) {
      key = i;
    } else {
      key = (i + START) * PRIME + OFFSET;
    }

    REQUIRE_FALSE(set.contains(key));
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> clear average collision", "[hash_set]", ds::i32,
    ds::i64, ds::u32, ds::u64
) {
  using key_type = TestType;
  test_clear<key_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_set<int_type> clear bad collision", "[hash_set]", ds::i64, ds::u32,
    ds::u64
) {
  using key_type = TestType;
  test_clear<key_type, false>();
}

// === Iterating === //

TEST_CASE("hash_set iteration", "[hash_set]") {
  const ds::i32 MULTIPLIER = 7;
  ds::i32 array[] = {0, 0, 0, 0, 0};
  ds::hash_set<ds::i32> map{};

  REQUIRE(ds_test::handle_error(map.insert(0)));
  REQUIRE(ds_test::handle_error(map.insert(1 * MULTIPLIER)));
  REQUIRE(ds_test::handle_error(map.insert(2 * MULTIPLIER)));
  REQUIRE(ds_test::handle_error(map.insert(3 * MULTIPLIER)));
  REQUIRE(ds_test::handle_error(map.insert(4 * MULTIPLIER)));

  for (auto iterator = map.begin(); iterator != map.end(); ++iterator) {
    ++array[*iterator / MULTIPLIER];
  }

  for (ds::i32 i = 0; i < 5; ++i) {
    REQUIRE(array[i] == 1);
  }

  for (auto iterator = map.cbegin(); iterator != map.cend(); ++iterator) {
    ++array[*iterator / MULTIPLIER];
  }

  for (ds::i32 i = 0; i < 5; ++i) {
    REQUIRE(array[i] == 2);
  }
}

// TODO: Class Types Test

// === Key String Types === //

template <bool Check = false>
ds::expected<ds::hash_set<ds::string>, ds::error_code>
create_string_hash_set() {
  ds::hash_set<ds::string> set{};
  ds::i64* pointer = nullptr;
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::string string{};
  ds::c8 characters[32]; // NOLINT

  if constexpr (Check) {
    REQUIRE(ds_test::handle_error(set.insert("Hello")));

    REQUIRE(ds_test::handle_error(string.copy("My")));
    REQUIRE(ds_test::handle_error(set.insert(string)));

    REQUIRE(ds_test::handle_error(string.copy("Name")));
    REQUIRE(ds_test::handle_error(set.insert(std::move(string))));

    std::strcpy(characters, "Jeff");
    REQUIRE(ds_test::handle_error(set.insert(characters)));

    REQUIRE(set.get_size() == 4);
  } else {
    // NOLINTNEXTLINE
    const ds::c8* words[] = {"Hello", "My", "Name", "Jeff"};
    for (ds::i64 i = 0; i < 4; ++i) {
      DS_TRY(set.insert(words[i]), ds::to_unexpected);
    }
  }

  return std::move(set);
}

TEST_CASE("hash_set<string> insert", "[hash_set]") {
  create_string_hash_set<true>();
}

TEST_CASE("hash_set<string> access existing", "[hash_set]") {
  ds::string string{};
  ds::hash_set<ds::string> set = ({
    auto expected_set = create_string_hash_set<>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });

  REQUIRE(set.contains("Hello"));

  REQUIRE(ds_test::handle_error(string.copy("My")));
  REQUIRE(set.contains(string));

  REQUIRE(set.contains("Name"));

  REQUIRE(ds_test::handle_error(string.copy("Jeff")));
  REQUIRE(set.contains(string));
}

TEST_CASE("hash_set<string> access non-existing", "[hash_set]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;
  ds::string string{};
  ds::hash_set<ds::string> set = ({
    auto expected_set = create_string_hash_set<>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });

  REQUIRE_FALSE(set.contains("Hi"));

  REQUIRE(ds_test::handle_error(string.copy("Some")));
  REQUIRE_FALSE(set.contains(string));
}

TEST_CASE("hash_set<string> remove existing", "[hash_set]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;
  ds::string string{};
  ds::c8 characters[32]; // NOLINT
  ds::hash_set<ds::string> set = ({
    auto expected_set = create_string_hash_set<>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });

  set.remove("Hello");
  REQUIRE(set.get_size() == 3);

  REQUIRE(ds_test::handle_error(string.copy("My")));
  set.remove(string);
  REQUIRE(set.get_size() == 2);

  std::strcpy(characters, "Name");
  set.remove(characters);
  REQUIRE(set.get_size() == 1);

  REQUIRE(ds_test::handle_error(string.copy("Jeff")));
  set.remove(string);
  REQUIRE(set.get_size() == 0);
  REQUIRE(set.is_empty());
}

TEST_CASE("hash_set<string> remove non-existing", "[hash_set]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::hash_set<ds::string> set = ({
    auto expected_set = create_string_hash_set<>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });

  set.remove("asdf");
  REQUIRE(set.get_size() == 4);

  set.remove("something");
  REQUIRE(set.get_size() == 4);

  set.remove("else");
  REQUIRE(set.get_size() == 4);

  set.remove("asdfasdf");
  REQUIRE(set.get_size() == 4);
}

TEST_CASE("hash_set<string> reinserting", "[hash_set]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;
  ds::string string{};
  ds::c8 characters[32]; // NOLINT
  ds::hash_set<ds::string> set = ({
    auto expected_set = create_string_hash_set<>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });

  REQUIRE(ds_test::handle_error(set.insert("Hello")));
  REQUIRE(set.get_size() == 4);

  REQUIRE(ds_test::handle_error(string.copy("My")));
  REQUIRE(ds_test::handle_error(set.insert(string)));
  REQUIRE(set.get_size() == 4);

  REQUIRE(ds_test::handle_error(string.copy("Name")));
  REQUIRE(ds_test::handle_error(set.insert(std::move(string))));
  REQUIRE(set.get_size() == 4);

  std::strcpy(characters, "Jeff");
  REQUIRE(ds_test::handle_error(set.insert(characters)));
  REQUIRE(set.get_size() == 4);
}

TEST_CASE("hash_set<string> clearing", "[hash_set]") {
  ds::hash_set<ds::string> set = ({
    auto expected_set = create_string_hash_set<>();
    REQUIRE(ds_test::handle_error(expected_set));
    std::move(*expected_set);
  });

  set.clear();
  REQUIRE(set.get_size() == 0);
  REQUIRE(set.is_empty());

  REQUIRE(set.get_capacity() > 0);
}
