/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#include "ds/hash_map.hpp"
#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/expected.hpp"
#include "ds/prime.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include <cstring>
#include <tuple>

// For average collision
const ds::usize SIZE = 32;

// For bad collision
const ds::usize PRIME = ds::get_first_prime();
const ds::usize START = 1;
const ds::usize END = 10;
const ds::usize OFFSET = 5;

// === Inserting === //

template <
    typename Key, typename Value, bool AverageCollision, bool Check = false>
ds::expected<ds::hash_map<Key, Value>, ds::error_code> create_hash_map() {
  ds::hash_map<Key, Value> map{};
  ds::error_code error_code{};

  if constexpr (AverageCollision) {
    for (Key i = 0, counter = 0; i < 10; ++i) {
      for (Key j = i; j < SIZE; j += 10) {
        if constexpr (Check) {
          error_code = map.insert(j, j * 2);
          REQUIRE(ds_test::handle_error(error_code));
          REQUIRE(map.get_size() == ++counter);
          REQUIRE(map.get_capacity() >= counter);
        } else {
          TRY(map.insert(j, j * 2), ds::to_unexpected);
        }
      }
    }
  } else {
    for (Key i = START, counter = 0; i < END; ++i) {
      error_code = map.insert(i * PRIME + OFFSET, (i - START) * 2);
      REQUIRE(ds_test::handle_error(error_code));
      REQUIRE(map.get_size() == ++counter);
    }
  }

  return std::move(map);
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> insert average collision", "[hash_map]",
    (std::tuple<ds::i32, ds::i32>), (std::tuple<ds::i64, ds::i64>),
    (std::tuple<ds::u32, ds::u32>), (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  create_hash_map<key_type, value_type, true, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> insert bad collision", "[hash_map]",
    (std::tuple<ds::i32, ds::i32>), (std::tuple<ds::i64, ds::i64>),
    (std::tuple<ds::u32, ds::u32>), (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  create_hash_map<key_type, value_type, false, true>();
}

// === Accessing Existing === //

template <typename Key, typename Value, bool AverageCollision>
void test_access_existing() {
  ds::expected<Value*, ds::error_code> expected{};
  Value* pointer = nullptr;
  ds::hash_map<Key, Value> map = ({
    auto expected_map = create_hash_map<Key, Value, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });
  const ds::usize INIT_SIZE = map.get_size();

  Key key = 0;
  for (Key i = 0; i < INIT_SIZE; ++i) {
    if constexpr (AverageCollision) {
      key = i;
    } else {
      key = (i + START) * PRIME + OFFSET;
    }

    expected = map.at(key);
    REQUIRE(ds_test::handle_error(expected));
    REQUIRE(**expected == i * 2);

    pointer = map[key];
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == i * 2);

    REQUIRE(map.contains(key));
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> access existing keys average collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_access_existing<key_type, value_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> existing keys bad collision", "[hash_map]",
    (std::tuple<ds::i32, ds::i32>), (std::tuple<ds::i64, ds::i64>),
    (std::tuple<ds::u32, ds::u32>), (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_access_existing<key_type, value_type, false>();
}

// === Accessing Non-Existing === //

template <typename Key, typename Value, bool AverageCollision>
void test_access_non_existing() {
  ds::expected<Value*, ds::error_code> expected{};
  ds::hash_map<Key, Value> map = ({
    auto expected_map = create_hash_map<Key, Value, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });
  const ds::usize INIT_SIZE = map.get_size();

  Key key = 0;
  for (Key i = 0; i < INIT_SIZE; ++i) {
    if constexpr (AverageCollision) {
      key = i + 100;
    } else {
      key = (i + START) * PRIME + OFFSET + 1;
    }

    expected = map.at(key);
    REQUIRE_FALSE(expected);
    REQUIRE(expected.error() == ds::error::NOT_FOUND);

    REQUIRE(map[key] == nullptr);

    REQUIRE_FALSE(map.contains(key));
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> access non-existing keys average collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_access_non_existing<key_type, value_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> non-existing keys bad collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_access_non_existing<key_type, value_type, false>();
}

// === Removing Existing === //

template <typename Key, typename Value, bool AverageCollision>
void test_remove_existing() {
  ds::expected<Value*, ds::error_code> expected{};
  ds::hash_map<Key, Value> map = ({
    auto expected_map = create_hash_map<Key, Value, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });
  const ds::usize INIT_SIZE = map.get_size();

  Key key = 0;
  for (ds::usize i = 0, counter = INIT_SIZE; i < 10; ++i) {
    for (ds::usize j = i; j < INIT_SIZE; j += 10) {
      if constexpr (AverageCollision) {
        key = j;
      } else {
        key = (j + START) * PRIME + OFFSET;
      }
      map.remove(key);
      REQUIRE(map.get_size() == --counter);

      REQUIRE_FALSE(map.contains(key));
    }
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> remove existing keys average collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_remove_existing<key_type, value_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> remove existing keys bad collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_remove_existing<key_type, value_type, false>();
}

// === Remove Non Existing === //

template <typename Key, typename Value, bool AverageCollision>
void test_remove_non_existing() {
  ds::expected<Value*, ds::error_code> expected{};
  ds::hash_map<Key, Value> map = ({
    auto expected_map = create_hash_map<Key, Value, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });
  const ds::usize INIT_SIZE = map.get_size();

  Key key = 0;
  for (ds::usize i = 0, counter = INIT_SIZE; i < 10; ++i) {
    for (ds::usize j = i; j < INIT_SIZE; j += 10) {
      if constexpr (AverageCollision) {
        key = j + 100;
      } else {
        key = (j + START) * PRIME + OFFSET + 1;
      }
      map.remove(key);
      REQUIRE(map.get_size() == counter);
    }
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> remove non-existing keys average collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_remove_non_existing<key_type, value_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> remove non-existing keys bad collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_remove_non_existing<key_type, value_type, false>();
}

// === Reinserting === //

template <typename Key, typename Value, bool AverageCollision>
void test_reinsert() {
  ds::error_code error_code{};
  ds::expected<Value*, ds::error_code> expected{};
  Value* pointer = nullptr;

  ds::hash_map<Key, Value> map = ({
    auto expected_map = create_hash_map<Key, Value, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });
  const ds::usize INIT_SIZE = map.get_size();

  Key key = 0;
  for (ds::usize i = 0; i < 10; ++i) {
    for (ds::usize j = i; j < INIT_SIZE; j += 10) {
      if constexpr (AverageCollision) {
        key = j;
      } else {
        key = (j + START) * PRIME + OFFSET;
      }
      error_code = map.insert(key, j * 3);
      REQUIRE(ds_test::handle_error(error_code));
      REQUIRE(map.get_size() == INIT_SIZE);

      pointer = map[key];
      REQUIRE(pointer != nullptr);
      REQUIRE(*pointer == j * 3);

      REQUIRE(map.contains(key));
    }
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> reinserting keys average collision",
    "[hash_map]", (std::tuple<ds::i32, ds::i32>),
    (std::tuple<ds::i64, ds::i64>), (std::tuple<ds::u32, ds::u32>),
    (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_reinsert<key_type, value_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> reinserting keys bad collision", "[hash_map]",
    (std::tuple<ds::i32, ds::i32>), (std::tuple<ds::i64, ds::i64>),
    (std::tuple<ds::u32, ds::u32>), (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_reinsert<key_type, value_type, false>();
}

// === Clearing === //

template <typename Key, typename Value, bool AverageCollision>
void test_clear() {
  ds::error_code error_code{};
  ds::expected<Value*, ds::error_code> expected{};
  Value* pointer = nullptr;

  ds::hash_map<Key, Value> map = ({
    auto expected_map = create_hash_map<Key, Value, AverageCollision>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });
  const ds::usize INIT_SIZE = map.get_size();

  map.clear();
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.is_empty());

  REQUIRE(map.get_capacity() > 0);

  // Check for the old keys
  Key key = 0;
  for (Key i = 0; i < INIT_SIZE; ++i) {
    if constexpr (AverageCollision) {
      key = i;
    } else {
      key = (i + START) * PRIME + OFFSET;
    }

    REQUIRE_FALSE(map.contains(key));
  }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> clear average collision", "[hash_map]",
    (std::tuple<ds::i32, ds::i32>), (std::tuple<ds::i64, ds::i64>),
    (std::tuple<ds::u32, ds::u32>), (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_clear<key_type, value_type, true>();
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> clear bad collision", "[hash_map]",
    (std::tuple<ds::i32, ds::i32>), (std::tuple<ds::i64, ds::i64>),
    (std::tuple<ds::u32, ds::u32>), (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;
  test_clear<key_type, value_type, false>();
}

// === Edge Case: empty hash_maps === //

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "hash_map<int_type, int_type> is empty", "[hash_map]",
    (std::tuple<ds::i32, ds::i32>), (std::tuple<ds::i64, ds::i64>),
    (std::tuple<ds::u32, ds::u32>), (std::tuple<ds::u64, ds::u64>)
) {
  using key_type = typename std::tuple_element<0, TestType>::type;
  using value_type = typename std::tuple_element<1, TestType>::type;

  ds::hash_map<key_type, value_type> map{};

  REQUIRE(map.is_empty());
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.get_capacity() == 0);

  REQUIRE_FALSE(map.contains(0));

  ds::expected<value_type*, ds::error_code> expected = map.at(0);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  value_type* pointer = map[0];
  REQUIRE(pointer == nullptr);
}

// === Iterating === //

TEST_CASE("hash_map iteration", "[hash_map]") {
  const ds::i32 MULTIPLIER = 7;
  ds::i32 array[] = {0, 0, 0, 0, 0};
  ds::hash_map<ds::i32, ds::i32> map{};

  REQUIRE(ds_test::handle_error(map.insert(0, 0)));
  REQUIRE(ds_test::handle_error(map.insert(1 * MULTIPLIER, 1)));
  REQUIRE(ds_test::handle_error(map.insert(2 * MULTIPLIER, 2)));
  REQUIRE(ds_test::handle_error(map.insert(3 * MULTIPLIER, 3)));
  REQUIRE(ds_test::handle_error(map.insert(4 * MULTIPLIER, 4)));

  for (auto iterator = map.begin(); iterator != map.end(); ++iterator) {
    ++array[iterator.key() / MULTIPLIER];
    ++array[iterator.value()];
  }

  for (ds::i32 i = 0; i < 5; ++i) {
    REQUIRE(array[i] == 2);
  }

  for (auto iterator = map.cbegin(); iterator != map.cend(); ++iterator) {
    ++array[iterator.key() / MULTIPLIER];
    ++array[iterator.value()];
  }

  for (ds::i32 i = 0; i < 5; ++i) {
    REQUIRE(array[i] == 4);
  }
}

// TODO: Class Types Test

// === Key String Types === //

template <bool Check = false>
ds::expected<ds::hash_map<ds::string, ds::i64>, ds::error_code>
create_key_string_hash_map() {
  ds::hash_map<ds::string, ds::i64> map{};
  ds::i64* pointer = nullptr;
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::string string{};
  ds::c8 characters[32]; // NOLINT

  if constexpr (Check) {
    REQUIRE(ds_test::handle_error(map.insert("Hello", 1)));

    REQUIRE(ds_test::handle_error(string.copy("My")));
    REQUIRE(ds_test::handle_error(map.insert(string, 2)));

    REQUIRE(ds_test::handle_error(string.copy("Name")));
    REQUIRE(ds_test::handle_error(map.insert(std::move(string), 3)));

    std::strcpy(characters, "Jeff");
    REQUIRE(ds_test::handle_error(map.insert(characters, 4)));

    REQUIRE(map.get_size() == 4);
  } else {
    // NOLINTNEXTLINE
    const ds::c8* words[] = {"Hello", "My", "Name", "Jeff"};
    for (ds::i64 i = 0; i < 4; ++i) {
      TRY(map.insert(words[i], i + 1), ds::to_unexpected);
    }
  }

  return std::move(map);
}

TEST_CASE("hash_map<string, i64> insert", "[hash_map]") {
  create_key_string_hash_map<true>();
}

TEST_CASE("hash_map<string, i64> access existing", "[hash_map]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;
  ds::string string{};
  ds::hash_map<ds::string, ds::i64> map = ({
    auto expected_map = create_key_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  expected = map.at("Hello");
  REQUIRE(ds_test::handle_error(expected));
  REQUIRE(**expected == 1);

  REQUIRE(ds_test::handle_error(string.copy("My")));
  expected = map.at(string);
  REQUIRE(ds_test::handle_error(expected));
  REQUIRE(**expected == 2);

  pointer = map["Name"];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == 3);

  REQUIRE(map.contains("Jeff"));

  REQUIRE(ds_test::handle_error(string.copy("Jeff")));
  REQUIRE(map.contains(string));
}

TEST_CASE("hash_map<string, i64> access non-existing", "[hash_map]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;
  ds::string string{};
  ds::hash_map<ds::string, ds::i64> map = ({
    auto expected_map = create_key_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  expected = map.at("Hi");
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  pointer = map["There"];
  REQUIRE(pointer == nullptr);

  REQUIRE(ds_test::handle_error(string.copy("Some")));
  expected = map.at(string);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  REQUIRE(ds_test::handle_error(string.copy("World")));
  pointer = map[string];
  REQUIRE(pointer == nullptr);

  REQUIRE_FALSE(map.contains("asdf"));

  REQUIRE(ds_test::handle_error(string.copy("blahhh")));
  REQUIRE_FALSE(map.contains(string));
}

TEST_CASE("hash_map<string, i64> remove existing", "[hash_map]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;
  ds::string string{};
  ds::c8 characters[32]; // NOLINT
  ds::hash_map<ds::string, ds::i64> map = ({
    auto expected_map = create_key_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.remove("Hello");
  REQUIRE(map.get_size() == 3);

  REQUIRE(ds_test::handle_error(string.copy("My")));
  map.remove(string);
  REQUIRE(map.get_size() == 2);

  std::strcpy(characters, "Name");
  map.remove(characters);
  REQUIRE(map.get_size() == 1);

  REQUIRE(ds_test::handle_error(string.copy("Jeff")));
  map.remove(string);
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.is_empty());
}

TEST_CASE("hash_map<string, i64> remove non-existing", "[hash_map]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::hash_map<ds::string, ds::i64> map = ({
    auto expected_map = create_key_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.remove("asdf");
  REQUIRE(map.get_size() == 4);

  map.remove("something");
  REQUIRE(map.get_size() == 4);

  map.remove("else");
  REQUIRE(map.get_size() == 4);

  map.remove("asdfasdf");
  REQUIRE(map.get_size() == 4);
}

TEST_CASE("hash_map<string, i64> reinserting", "[hash_map]") {
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;
  ds::string string{};
  ds::c8 characters[32]; // NOLINT
  ds::hash_map<ds::string, ds::i64> map = ({
    auto expected_map = create_key_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  REQUIRE(ds_test::handle_error(map.insert("Hello", 100)));
  REQUIRE(map.get_size() == 4);
  pointer = map["Hello"];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == 100);

  REQUIRE(ds_test::handle_error(string.copy("My")));
  REQUIRE(ds_test::handle_error(map.insert(string, 200)));
  REQUIRE(map.get_size() == 4);
  pointer = map[string];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == 200);

  REQUIRE(ds_test::handle_error(string.copy("Name")));
  REQUIRE(ds_test::handle_error(map.insert(std::move(string), 300)));
  REQUIRE(map.get_size() == 4);
  pointer = map["Name"];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == 300);

  std::strcpy(characters, "Jeff");
  REQUIRE(ds_test::handle_error(map.insert(characters, 400)));
  REQUIRE(map.get_size() == 4);
  pointer = map[characters];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == 400);
}

TEST_CASE("hash_map<string, i64> clearing", "[hash_map]") {
  ds::hash_map<ds::string, ds::i64> map = ({
    auto expected_map = create_key_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.clear();
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.is_empty());

  REQUIRE(map.get_capacity() > 0);
}

TEST_CASE("hash_map<string, i64> is empty", "[hash_map]") {
  ds::hash_map<ds::string, ds::i64> map{};
  ds::string string{};
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::i64* pointer = nullptr;

  REQUIRE(ds_test::handle_error(string.copy("test")));

  REQUIRE(map.is_empty());
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.get_capacity() == 0);

  REQUIRE_FALSE(map.contains("test"));

  expected = map.at("test");
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  expected = map.at(string);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  pointer = map["test"];
  REQUIRE(pointer == nullptr);

  pointer = map[string];
  REQUIRE(pointer == nullptr);
}

// === Value String Map === //

// NOLINTNEXTLINE
const ds::i64 VS_INDICES[] = {
    OFFSET, (ds::i64)(OFFSET + PRIME), (ds::i64)(OFFSET + PRIME * 2),
    (ds::i64)(OFFSET + PRIME * 3)
};

template <bool Check = false>
ds::expected<ds::hash_map<ds::i64, ds::string>, ds::error_code>
create_value_string_hash_map() {
  ds::hash_map<ds::i64, ds::string> map{};
  ds::string* pointer = nullptr;
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::string string{};
  ds::c8 characters[32]; // NOLINT

  if constexpr (Check) {
    REQUIRE(ds_test::handle_error(map.insert(VS_INDICES[0], "Hello")));

    REQUIRE(ds_test::handle_error(string.copy("My")));
    REQUIRE(ds_test::handle_error(map.insert(VS_INDICES[1], string)));

    REQUIRE(ds_test::handle_error(string.copy("Name")));
    REQUIRE(
        ds_test::handle_error(map.insert(VS_INDICES[2], std::move(string)))
    );

    std::strcpy(characters, "Jeff");
    REQUIRE(ds_test::handle_error(map.insert(VS_INDICES[3], characters)));

    REQUIRE(map.get_size() == 4);
  } else {
    TRY(map.insert(VS_INDICES[0], "Hello"), ds::to_unexpected);
    TRY(map.insert(VS_INDICES[1], "My"), ds::to_unexpected);
    TRY(map.insert(VS_INDICES[2], "Name"), ds::to_unexpected);
    TRY(map.insert(VS_INDICES[3], "Jeff"), ds::to_unexpected);
  }

  return std::move(map);
}

TEST_CASE("hash_map<i64, string> inserting", "[hash_map]") {
  create_value_string_hash_map<true>();
}

TEST_CASE("hash_map<i64, string> access existing", "[hash_map]") {
  ds::string* pointer = nullptr;
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::i64, ds::string> map = ({
    auto expected_map = create_value_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  expected = map.at(VS_INDICES[0]);
  REQUIRE(ds_test::handle_error(expected));
  REQUIRE(**expected == "Hello");

  expected = map.at(VS_INDICES[1]);
  REQUIRE(ds_test::handle_error(expected));
  REQUIRE(**expected == "My");

  pointer = map[VS_INDICES[2]];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == "Name");

  pointer = map[VS_INDICES[3]];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == "Jeff");

  REQUIRE(map.contains(VS_INDICES[0]));
  REQUIRE(map.contains(VS_INDICES[1]));
  REQUIRE(map.contains(VS_INDICES[2]));
  REQUIRE(map.contains(VS_INDICES[3]));
}

TEST_CASE("hash_map<i64, string> access non-existing", "[hash_map]") {
  ds::string* pointer = nullptr;
  ds::string string{};
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::i64, ds::string> map = ({
    auto expected_map = create_value_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  expected = map.at(-1);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  pointer = map[-100];
  REQUIRE(pointer == nullptr);

  REQUIRE(ds_test::handle_error(string.copy("Some")));
  expected = map.at(-200);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  REQUIRE(ds_test::handle_error(string.copy("World")));
  pointer = map[0];
  REQUIRE(pointer == nullptr);
}

TEST_CASE("hash_map<i64, string> remove existing", "[hash_map]") {
  ds::string* pointer = nullptr;
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::i64, ds::string> map = ({
    auto expected_map = create_value_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.remove(VS_INDICES[0]);
  REQUIRE(map.get_size() == 3);
  REQUIRE_FALSE(map.contains(VS_INDICES[0]));

  map.remove(VS_INDICES[1]);
  REQUIRE(map.get_size() == 2);
  REQUIRE_FALSE(map.contains(VS_INDICES[1]));

  map.remove(VS_INDICES[2]);
  REQUIRE(map.get_size() == 1);
  REQUIRE_FALSE(map.contains(VS_INDICES[2]));

  map.remove(VS_INDICES[3]);
  REQUIRE(map.get_size() == 0);
  REQUIRE_FALSE(map.contains(VS_INDICES[3]));

  REQUIRE(map.is_empty());
}

TEST_CASE("hash_map<i64, string> remove non-existing", "[hash_map]") {
  ds::string* pointer = nullptr;
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::i64, ds::string> map = ({
    auto expected_map = create_value_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.remove(-1);
  REQUIRE(map.get_size() == 4);

  map.remove(-100);
  REQUIRE(map.get_size() == 4);

  map.remove(-200);
  REQUIRE(map.get_size() == 4);

  map.remove(0);
  REQUIRE(map.get_size() == 4);
}

TEST_CASE("hash_map<i64, string> reinserting", "[hash_map]") {
  ds::string* pointer = nullptr;
  ds::string string{};
  ds::c8 characters[32]; // NOLINT
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::i64, ds::string> map = ({
    auto expected_map = create_value_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  REQUIRE(ds_test::handle_error(map.insert(VS_INDICES[0], "Hello")));
  REQUIRE(map.get_size() == 4);

  REQUIRE(ds_test::handle_error(string.copy("My")));
  REQUIRE(ds_test::handle_error(map.insert(VS_INDICES[1], string)));
  REQUIRE(map.get_size() == 4);

  REQUIRE(ds_test::handle_error(string.copy("Name")));
  REQUIRE(ds_test::handle_error(map.insert(VS_INDICES[2], std::move(string))));
  REQUIRE(map.get_size() == 4);

  std::strcpy(characters, "Jeff");
  REQUIRE(ds_test::handle_error(map.insert(VS_INDICES[3], characters)));
  REQUIRE(map.get_size() == 4);
}

TEST_CASE("hash_map<i64, string> clearing", "[hash_map]") {
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::i64, ds::string> map = ({
    auto expected_map = create_value_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.clear();
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.is_empty());
  REQUIRE(map.get_capacity() > 0);
}

TEST_CASE("hash_map<i64, string> is empty", "[hash_map]") {
  ds::hash_map<ds::i64, ds::string> map{};
  ds::string string{};

  REQUIRE(map.is_empty());
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.get_capacity() == 0);

  REQUIRE_FALSE(map.contains(0));

  ds::expected<ds::string*, ds::error_code> expected = map.at(0);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  ds::string* pointer = map[0];
  REQUIRE(pointer == nullptr);
}

// === String Map === //

// NOLINTNEXTLINE
inline const ds::c8* STRING_HASH_MAP_WORDS[] = {
    "glass",   "history", "legacy", "lark",    "stand", "cocoa",
    "parsnip", "oat",     "ear",    "airship", "merit", "decade",
    "truth",   "bond",    "scow",   "fishing", "tunic", "founder",
};

template <bool Check = false>
ds::expected<ds::hash_map<ds::string, ds::string>, ds::error_code>
create_string_hash_map() {
  ds::hash_map<ds::string, ds::string> map{};
  ds::string string1{};
  ds::string string2{};

  if constexpr (Check) {
    // const c8* key
    REQUIRE(
        ds_test::handle_error(
            map.insert(STRING_HASH_MAP_WORDS[0], STRING_HASH_MAP_WORDS[1])
        )
    );

    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[3])));
    REQUIRE(
        ds_test::handle_error(map.insert(STRING_HASH_MAP_WORDS[2], string1))
    );

    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[5])));
    REQUIRE(
        ds_test::handle_error(
            map.insert(STRING_HASH_MAP_WORDS[4], std::move(string1))
        )
    );

    // const ds::string& key
    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[6])));
    REQUIRE(
        ds_test::handle_error(map.insert(string1, STRING_HASH_MAP_WORDS[7]))
    );

    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[8])));
    REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[9])));
    REQUIRE(ds_test::handle_error(map.insert(string1, string2)));

    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[10])));
    REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[11])));
    REQUIRE(ds_test::handle_error(map.insert(string1, std::move(string2))));

    // ds::string&& key
    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[12])));
    REQUIRE(
        ds_test::handle_error(
            map.insert(std::move(string1), STRING_HASH_MAP_WORDS[13])
        )
    );

    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[14])));
    REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[15])));
    REQUIRE(ds_test::handle_error(map.insert(std::move(string1), string2)));

    REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[16])));
    REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[17])));
    REQUIRE(
        ds_test::handle_error(
            map.insert(std::move(string1), std::move(string2))
        )
    );

    REQUIRE(map.get_size() == 9);
  } else {
    for (ds::i32 i = 0; i < 9; ++i) {
      TRY(map.insert(
              STRING_HASH_MAP_WORDS[i * 2], STRING_HASH_MAP_WORDS[i * 2 + 1]
          ),
          ds::to_unexpected);
    }
  }

  return std::move(map);
}

TEST_CASE("hash_map<string, string> inserting", "[hash_map]") {
  create_string_hash_map<true>();
}

TEST_CASE("hash_map<string, string> access existing", "[hash_map]") {
  ds::string* pointer = nullptr;
  ds::string string1{};
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::string, ds::string> map = ({
    auto expected_map = create_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  expected = map.at(STRING_HASH_MAP_WORDS[0]);
  REQUIRE(ds_test::handle_error(expected));
  REQUIRE(**expected == STRING_HASH_MAP_WORDS[1]);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[2])));
  expected = map.at(string1);
  REQUIRE(ds_test::handle_error(expected));
  REQUIRE(**expected == STRING_HASH_MAP_WORDS[3]);

  pointer = map[STRING_HASH_MAP_WORDS[4]];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == STRING_HASH_MAP_WORDS[5]);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[6])));
  pointer = map[string1];
  REQUIRE(pointer != nullptr);
  REQUIRE(*pointer == STRING_HASH_MAP_WORDS[7]);

  REQUIRE(map.contains(STRING_HASH_MAP_WORDS[8]));
  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[10])));
  REQUIRE(map.contains(string1));
}

TEST_CASE("hash_map<string, string> access non-existing", "[hash_map]") {
  ds::string* pointer = nullptr;
  ds::string string1{};
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::string, ds::string> map = ({
    auto expected_map = create_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  expected = map.at(STRING_HASH_MAP_WORDS[1]);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[3])));
  expected = map.at(string1);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  pointer = map[STRING_HASH_MAP_WORDS[5]];
  REQUIRE(pointer == nullptr);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[7])));
  pointer = map[string1];
  REQUIRE(pointer == nullptr);

  REQUIRE_FALSE(map.contains(STRING_HASH_MAP_WORDS[9]));
  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[11])));
  REQUIRE_FALSE(map.contains(string1));
}

TEST_CASE("hash_map<string, string> remove existing", "[hash_map]") {
  ds::string string1{};
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::string, ds::string> map = ({
    auto expected_map = create_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  for (ds::usize i = 0; i < 9; ++i) {
    if (i & 1) {
      map.remove(STRING_HASH_MAP_WORDS[i * 2]);
    } else {
      REQUIRE(
          ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[i * 2]))
      );
      map.remove(string1);
    }
    REQUIRE(map.get_size() == 8 - i);
    REQUIRE_FALSE(map.contains(STRING_HASH_MAP_WORDS[i * 2]));
  }

  REQUIRE(map.is_empty());
}

TEST_CASE("hash_map<string, string> remove non-existing", "[hash_map]") {
  ds::string string1{};
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::string, ds::string> map = ({
    auto expected_map = create_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.remove("asdf");
  REQUIRE(map.get_size() == 9);

  map.remove("something");
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy("else")));
  map.remove(string1);
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy("asdfasdf")));
  REQUIRE(map.get_size() == 9);
}

TEST_CASE("hash_map<string, string> reinserting", "[hash_map]") {
  ds::string string1{};
  ds::string string2{};
  ds::string* pointer = nullptr;
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::string, ds::string> map = ({
    auto expected_map = create_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  // const c8* key
  REQUIRE(
      ds_test::handle_error(
          map.insert(STRING_HASH_MAP_WORDS[0], STRING_HASH_MAP_WORDS[17])
      )
  );
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[15])));
  REQUIRE(ds_test::handle_error(map.insert(STRING_HASH_MAP_WORDS[2], string1)));
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[13])));
  REQUIRE(
      ds_test::handle_error(
          map.insert(STRING_HASH_MAP_WORDS[4], std::move(string1))
      )
  );
  REQUIRE(map.get_size() == 9);

  // const ds::string& key
  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[6])));
  REQUIRE(
      ds_test::handle_error(map.insert(string1, STRING_HASH_MAP_WORDS[11]))
  );
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[8])));
  REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[9])));
  REQUIRE(ds_test::handle_error(map.insert(string1, string2)));
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[10])));
  REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[7])));
  REQUIRE(ds_test::handle_error(map.insert(string1, std::move(string2))));
  REQUIRE(map.get_size() == 9);

  // ds::string&& key
  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[12])));
  REQUIRE(
      ds_test::handle_error(
          map.insert(std::move(string1), STRING_HASH_MAP_WORDS[5])
      )
  );
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[14])));
  REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[3])));
  REQUIRE(ds_test::handle_error(map.insert(std::move(string1), string2)));
  REQUIRE(map.get_size() == 9);

  REQUIRE(ds_test::handle_error(string1.copy(STRING_HASH_MAP_WORDS[16])));
  REQUIRE(ds_test::handle_error(string2.copy(STRING_HASH_MAP_WORDS[1])));
  REQUIRE(
      ds_test::handle_error(map.insert(std::move(string1), std::move(string2)))
  );
  REQUIRE(map.get_size() == 9);

  for (ds::i32 i = 0; i < 9; ++i) {
    pointer = map[STRING_HASH_MAP_WORDS[i * 2]]; // NOLINT
    REQUIRE(pointer != nullptr);
    REQUIRE(*pointer == STRING_HASH_MAP_WORDS[(8 - i) * 2 + 1]);
  }
}

TEST_CASE("hash_map<string, string> clearing", "[hash_map]") {
  ds::string string1{};
  ds::string string2{};
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::hash_map<ds::string, ds::string> map = ({
    auto expected_map = create_string_hash_map<>();
    REQUIRE(ds_test::handle_error(expected_map));
    std::move(*expected_map);
  });

  map.clear();
  REQUIRE(map.get_size() == 0);
  REQUIRE(map.is_empty());

  REQUIRE(map.get_capacity() > 0);
}

TEST_CASE("hash_map<string, string> is empty", "[hash_map]") {
  ds::hash_map<ds::string, ds::string> map{};
  ds::string string{};
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::string* pointer = nullptr;

  REQUIRE(ds_test::handle_error(string.copy("test")));

  REQUIRE(map.get_size() == 0);
  REQUIRE(map.get_capacity() == 0);
  REQUIRE(map.is_empty());

  REQUIRE_FALSE(map.contains("test"));

  expected = map.at("test");
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  expected = map.at(string);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error::NOT_FOUND);

  pointer = map["test"];
  REQUIRE(pointer == nullptr);

  pointer = map[string];
  REQUIRE(pointer == nullptr);
}
