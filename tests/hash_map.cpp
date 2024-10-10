/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#include "ds/hash_map.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/prime.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include <cstdlib>
#include <cstring>

template <typename Key, typename Value>
inline void test_hash_map(const ds::usize N) {
  ds::hash_map<Key, Value> map{};
  Value* pointer = nullptr;
  ds::error_code error_code{};
  ds::expected<Value*, ds::error_code> expected{};

  SECTION("Insertions") {
    for (ds::usize i = 0, counter = 0; i < 10; ++i) {
      for (ds::usize j = i; j < N; j += 10) {
        error_code = map.insert(j, j * 2);
        REQUIRE(ds_test::handle_error(error_code));
        REQUIRE(map.get_size() == ++counter);
      }
    }

    SECTION("Accessing") {
      SECTION("Existing keys") {
        for (ds::usize i = 0; i < N; ++i) {
          expected = map.at(i);
          REQUIRE(ds_test::handle_error(expected));
          REQUIRE(**expected == i * 2);

          pointer = map[i];
          REQUIRE(pointer != nullptr);
          REQUIRE(*pointer == i * 2);
        }
      }

      SECTION("Non-existing keys") {
        for (ds::usize i = N + 50; i > N; --i) {
          expected = map.at(i);
          REQUIRE_FALSE(expected);
          REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

          pointer = map[i];
          REQUIRE(pointer == nullptr);
        }
      }
    }

    SECTION("Removing") {
      SECTION("Existing keys") {
        for (ds::usize i = 0, counter = N; i < 10; ++i) {
          for (ds::usize j = i; j < N; j += 10) {
            map.remove(j);
            REQUIRE(map.get_size() == --counter);
          }
        }
      }

      SECTION("Non-existing keys") {
        for (ds::usize i = N + 50; i > N; --i) {
          map.remove(i);
          REQUIRE(N == map.get_size());
        }
      }
    }

    SECTION("Reinserting keys") {
      for (ds::usize i = 0; i < 10; ++i) {
        for (ds::usize j = i; j < N; j += 10) {
          error_code = map.insert(j, j * 3);
          REQUIRE(ds_test::handle_error(error_code));
          REQUIRE(map.get_size() == N);

          pointer = map[j];
          REQUIRE(pointer != nullptr);
          REQUIRE(*pointer == j * 3);
        }
      }
    }

    SECTION("Clearing") {
      map.clear();
      REQUIRE(map.get_size() == 0);
      REQUIRE(map.is_empty());
    }
  }
}

// Check if every key is the same hash
template <typename Key, typename Value>
inline void test_hash_map_bad_collision() {
  ds::hash_map<Key, Value> map{};
  Value* pointer = nullptr;
  ds::error_code error_code{};
  ds::expected<Value*, ds::error_code> expected{};

  const ds::usize PRIME = ds::get_first_prime();
  const ds::usize START = 1;
  const ds::usize END = 10;
  const ds::usize OFFSET = 5;
  const ds::usize SIZE = END - START;

  SECTION("Insertions (bad collision)") {
    for (ds::usize i = START, counter = 0; i < END; ++i) {
      error_code = map.insert(i * PRIME + OFFSET, i);
      REQUIRE(ds_test::handle_error(error_code));
      REQUIRE(map.get_size() == ++counter);
    }

    SECTION("Accessing") {
      SECTION("Existing keys") {
        for (ds::usize i = START; i < END; ++i) {
          expected = map.at(i * PRIME + OFFSET);
          REQUIRE(ds_test::handle_error(expected));
          REQUIRE(**expected == i);

          pointer = map[i * PRIME + OFFSET];
          REQUIRE(pointer != nullptr);
          REQUIRE(*pointer == i);
        }
      }

      SECTION("Non-existing keys") {
        for (ds::usize i = START; i < END; ++i) {
          expected = map.at(i * PRIME + OFFSET + 1);
          REQUIRE_FALSE(expected);
          REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

          pointer = map[i * PRIME + OFFSET + 1];
          REQUIRE(pointer == nullptr);
        }
      }
    }

    SECTION("Removing") {
      SECTION("Existing keys") {
        for (ds::usize i = START, counter = SIZE; i < END; ++i) {
          map.remove(i * PRIME + OFFSET);
          REQUIRE(map.get_size() == --counter);
        }
      }
    }

    SECTION("Non-existing keys") {
      for (ds::usize i = START; i < END; ++i) {
        map.remove(i * PRIME + OFFSET + 1);
        REQUIRE(map.get_size() == SIZE);
      }
    }

    SECTION("Reinserting keys") {
      for (ds::usize i = START; i < END; ++i) {
        error_code = map.insert(i * PRIME + OFFSET, i * 2);
        REQUIRE(ds_test::handle_error(error_code));
        REQUIRE(map.get_size() == SIZE);

        pointer = map[i * PRIME + OFFSET];
        REQUIRE(pointer != nullptr);
        REQUIRE(*pointer == i * 2);
      }
    }

    SECTION("Clearing") {
      map.clear();
      REQUIRE(map.get_size() == 0);
      REQUIRE(map.is_empty());
    }
  }
}

const ds::usize SIZE = 32;

TEST_CASE("hash_map<i32, i32>", "[hash_map][i32]") {
  test_hash_map<ds::i32, ds::i32>(SIZE);
  test_hash_map_bad_collision<ds::i32, ds::i32>();
}

TEST_CASE("hash_map<u32, u32>", "[hash_map][u32]") {
  test_hash_map<ds::u32, ds::u32>(SIZE);
  test_hash_map_bad_collision<ds::u32, ds::u32>();
}

TEST_CASE("hash_map<i64, i64>", "[hash_map][i64]") {
  test_hash_map<ds::i64, ds::i64>(SIZE);
  test_hash_map_bad_collision<ds::i64, ds::i64>();
}

TEST_CASE("hash_map<u64, u64>", "[hash_map][u64]") {
  test_hash_map<ds::u64, ds::u64>(SIZE);
  test_hash_map_bad_collision<ds::u64, ds::u64>();
}

TEST_CASE("hash_map<string, i64>", "[hash_map][string][i64]") {
  ds::hash_map<ds::string, ds::i64> map{};
  ds::i64* pointer = nullptr;
  ds::expected<ds::i64*, ds::error_code> expected{};
  ds::string string{};
  ds::c8 characters[32]; // NOLINT

  SECTION("Insertions") {
    REQUIRE(ds_test::handle_error(map.insert("Hello", 1)));

    REQUIRE(ds_test::handle_error(string.copy("My")));
    REQUIRE(ds_test::handle_error(map.insert(string, 2)));

    REQUIRE(ds_test::handle_error(string.copy("Name")));
    REQUIRE(ds_test::handle_error(map.insert(std::move(string), 3)));

    std::strcpy(characters, "Jeff");
    REQUIRE(ds_test::handle_error(map.insert(characters, 4)));

    REQUIRE(map.get_size() == 4);

    SECTION("Accessing") {
      SECTION("Existing keys") {
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

        REQUIRE(ds_test::handle_error(string.copy("Jeff")));
        pointer = map[string];
        REQUIRE(pointer != nullptr);
        REQUIRE(*pointer == 4);
      }

      SECTION("Non-existing keys") {
        expected = map.at("Hi");
        REQUIRE_FALSE(expected);
        REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

        pointer = map["There"];
        REQUIRE(pointer == nullptr);

        REQUIRE(ds_test::handle_error(string.copy("Some")));
        expected = map.at(string);
        REQUIRE_FALSE(expected);
        REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

        REQUIRE(ds_test::handle_error(string.copy("World")));
        pointer = map[string];
        REQUIRE(pointer == nullptr);
      }
    }

    SECTION("Remove") {
      SECTION("Existing keys") {
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

      SECTION("Non-existing keys") {
        map.remove("asdf");
        REQUIRE(map.get_size() == 4);

        map.remove("something");
        REQUIRE(map.get_size() == 4);

        map.remove("else");
        REQUIRE(map.get_size() == 4);

        map.remove("asdfasdf");
        REQUIRE(map.get_size() == 4);
      }
    }

    SECTION("Reinserting keys") {
      REQUIRE(ds_test::handle_error(map.insert("Hello", 1)));
      REQUIRE(map.get_size() == 4);

      REQUIRE(ds_test::handle_error(string.copy("My")));
      REQUIRE(ds_test::handle_error(map.insert(string, 2)));
      REQUIRE(map.get_size() == 4);

      REQUIRE(ds_test::handle_error(string.copy("Name")));
      REQUIRE(ds_test::handle_error(map.insert(std::move(string), 3)));
      REQUIRE(map.get_size() == 4);

      std::strcpy(characters, "Jeff");
      REQUIRE(ds_test::handle_error(map.insert(characters, 4)));
      REQUIRE(map.get_size() == 4);
    }

    SECTION("Clearing") {
      map.clear();
      REQUIRE(map.get_size() == 0);
      REQUIRE(map.is_empty());
    }
  }
}

TEST_CASE("hash_map<i64, string>", "[hash_map][string][i64]") {
  ds::hash_map<ds::i64, ds::string> map{};
  ds::string* pointer = nullptr;
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::string string{};
  ds::c8 characters[32]; // NOLINT

  const ds::usize PRIME = ds::get_first_prime();
  const ds::usize OFFSET = 6;
  const ds::i64 INDEX1 = OFFSET;
  const ds::i64 INDEX2 = OFFSET + PRIME;
  const ds::i64 INDEX3 = OFFSET + PRIME * 2;
  const ds::i64 INDEX4 = OFFSET + PRIME * 3;

  SECTION("Insertion") {
    REQUIRE(ds_test::handle_error(map.insert(INDEX1, "Hello")));

    REQUIRE(ds_test::handle_error(string.copy("My")));
    REQUIRE(ds_test::handle_error(map.insert(INDEX2, string)));

    REQUIRE(ds_test::handle_error(string.copy("Name")));
    REQUIRE(ds_test::handle_error(map.insert(INDEX3, std::move(string))));

    std::strcpy(characters, "Jeff");
    REQUIRE(ds_test::handle_error(map.insert(INDEX4, characters)));

    REQUIRE(map.get_size() == 4);

    SECTION("Accessing") {
      SECTION("Existing keys") {
        expected = map.at(INDEX1);
        REQUIRE(ds_test::handle_error(expected));
        REQUIRE(**expected == "Hello");

        expected = map.at(INDEX2);
        REQUIRE(ds_test::handle_error(expected));
        REQUIRE(**expected == "My");

        pointer = map[INDEX3];
        REQUIRE(pointer != nullptr);
        REQUIRE(*pointer == "Name");

        pointer = map[INDEX4];
        REQUIRE(pointer != nullptr);
        REQUIRE(*pointer == "Jeff");
      }

      SECTION("Non-existing keys") {
        expected = map.at(-1);
        REQUIRE_FALSE(expected);
        REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

        pointer = map[-100];
        REQUIRE(pointer == nullptr);

        REQUIRE(ds_test::handle_error(string.copy("Some")));
        expected = map.at(-200);
        REQUIRE_FALSE(expected);
        REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

        REQUIRE(ds_test::handle_error(string.copy("World")));
        pointer = map[0];
        REQUIRE(pointer == nullptr);
      }
    }

    SECTION("Remove") {
      SECTION("Existing keys") {
        map.remove(INDEX1);
        REQUIRE(map.get_size() == 3);

        map.remove(INDEX2);
        REQUIRE(map.get_size() == 2);

        map.remove(INDEX3);
        REQUIRE(map.get_size() == 1);

        map.remove(INDEX4);
        REQUIRE(map.get_size() == 0);
        REQUIRE(map.is_empty());
      }

      SECTION("Non-existing keys") {
        map.remove(-1);
        REQUIRE(map.get_size() == 4);

        map.remove(-100);
        REQUIRE(map.get_size() == 4);

        map.remove(-200);
        REQUIRE(map.get_size() == 4);

        map.remove(0);
        REQUIRE(map.get_size() == 4);
      }
    }

    SECTION("Reinserting keys") {
      REQUIRE(ds_test::handle_error(map.insert(INDEX1, "Hello")));
      REQUIRE(map.get_size() == 4);

      REQUIRE(ds_test::handle_error(string.copy("My")));
      REQUIRE(ds_test::handle_error(map.insert(INDEX2, string)));
      REQUIRE(map.get_size() == 4);

      REQUIRE(ds_test::handle_error(string.copy("Name")));
      REQUIRE(ds_test::handle_error(map.insert(INDEX3, std::move(string))));
      REQUIRE(map.get_size() == 4);

      std::strcpy(characters, "Jeff");
      REQUIRE(ds_test::handle_error(map.insert(INDEX4, characters)));
      REQUIRE(map.get_size() == 4);
    }

    SECTION("Clearing") {
      map.clear();
      REQUIRE(map.get_size() == 0);
      REQUIRE(map.is_empty());
    }
  }
}

TEST_CASE("hash_map<string, string>", "[hash_map][string]") {
  ds::hash_map<ds::string, ds::string> map{};
  ds::string* pointer = nullptr;
  ds::expected<ds::string*, ds::error_code> expected{};
  ds::string string1{};
  ds::string string2{};

  // NOLINTNEXTLINE
  const ds::c8* WORDS[] = {
      "glass",   "history", "legacy", "lark",    "stand", "cocoa",
      "parsnip", "oat",     "ear",    "airship", "merit", "decade",
      "truth",   "bond",    "scow",   "fishing", "tunic", "founder",
  };

  SECTION("Insertions") {
    // const c8* key
    REQUIRE(ds_test::handle_error(map.insert(WORDS[0], WORDS[1])));

    REQUIRE(ds_test::handle_error(string1.copy(WORDS[3])));
    REQUIRE(ds_test::handle_error(map.insert(WORDS[2], string1)));

    REQUIRE(ds_test::handle_error(string1.copy(WORDS[5])));
    REQUIRE(ds_test::handle_error(map.insert(WORDS[4], std::move(string1))));

    // const ds::string& key
    REQUIRE(ds_test::handle_error(string1.copy(WORDS[6])));
    REQUIRE(ds_test::handle_error(map.insert(string1, WORDS[7])));

    REQUIRE(ds_test::handle_error(string1.copy(WORDS[8])));
    REQUIRE(ds_test::handle_error(string2.copy(WORDS[9])));
    REQUIRE(ds_test::handle_error(map.insert(string1, string2)));

    REQUIRE(ds_test::handle_error(string1.copy(WORDS[10])));
    REQUIRE(ds_test::handle_error(string2.copy(WORDS[11])));
    REQUIRE(ds_test::handle_error(map.insert(string1, std::move(string2))));

    // ds::string&& key
    REQUIRE(ds_test::handle_error(string1.copy(WORDS[12])));
    REQUIRE(ds_test::handle_error(map.insert(std::move(string1), WORDS[13])));

    REQUIRE(ds_test::handle_error(string1.copy(WORDS[14])));
    REQUIRE(ds_test::handle_error(string2.copy(WORDS[15])));
    REQUIRE(ds_test::handle_error(map.insert(std::move(string1), string2)));

    REQUIRE(ds_test::handle_error(string1.copy(WORDS[16])));
    REQUIRE(ds_test::handle_error(string2.copy(WORDS[17])));
    REQUIRE(ds_test::handle_error(
        map.insert(std::move(string1), std::move(string2))
    ));

    REQUIRE(map.get_size() == 9);

    SECTION("Accessing") {
      SECTION("Existing keys") {
        expected = map.at(WORDS[0]);
        REQUIRE(ds_test::handle_error(expected));
        REQUIRE(**expected == WORDS[1]);

        REQUIRE(ds_test::handle_error(string1.copy(WORDS[2])));
        expected = map.at(string1);
        REQUIRE(ds_test::handle_error(expected));
        REQUIRE(**expected == WORDS[3]);

        pointer = map[WORDS[4]];
        REQUIRE(pointer != nullptr);
        REQUIRE(*pointer == WORDS[5]);

        REQUIRE(ds_test::handle_error(string1.copy(WORDS[6])));
        pointer = map[string1];
        REQUIRE(pointer != nullptr);
        REQUIRE(*pointer == WORDS[7]);
      }

      SECTION("Non-existing keys") {
        expected = map.at(WORDS[1]);
        REQUIRE_FALSE(expected);
        REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

        REQUIRE(ds_test::handle_error(string1.copy(WORDS[3])));
        expected = map.at(string1);
        REQUIRE_FALSE(expected);
        REQUIRE(expected.error() == ds::error_code::NOT_FOUND);

        pointer = map[WORDS[5]];
        REQUIRE(pointer == nullptr);

        REQUIRE(ds_test::handle_error(string1.copy(WORDS[7])));
        pointer = map[string1];
        REQUIRE(pointer == nullptr);
      }
    }

    SECTION("Remove") {
      SECTION("Existing keys") {
        for (ds::usize i = 0; i < 9; ++i) {
          if (i & 1) {
            map.remove(WORDS[i * 2]);
          } else {
            REQUIRE(ds_test::handle_error(string1.copy(WORDS[i * 2])));
            map.remove(string1);
          }
          REQUIRE(map.get_size() == 8 - i);
        }

        REQUIRE(map.is_empty());
      }

      SECTION("Non-existing keys") {
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
    }

    SECTION("Reinserting keys") {
      // const c8* key
      REQUIRE(ds_test::handle_error(map.insert(WORDS[0], WORDS[1])));
      REQUIRE(map.get_size() == 9);

      REQUIRE(ds_test::handle_error(string1.copy(WORDS[3])));
      REQUIRE(ds_test::handle_error(map.insert(WORDS[2], string1)));
      REQUIRE(map.get_size() == 9);

      REQUIRE(ds_test::handle_error(string1.copy(WORDS[5])));
      REQUIRE(ds_test::handle_error(map.insert(WORDS[4], std::move(string1))));
      REQUIRE(map.get_size() == 9);

      // const ds::string& key
      REQUIRE(ds_test::handle_error(string1.copy(WORDS[6])));
      REQUIRE(ds_test::handle_error(map.insert(string1, WORDS[7])));
      REQUIRE(map.get_size() == 9);

      REQUIRE(ds_test::handle_error(string1.copy(WORDS[8])));
      REQUIRE(ds_test::handle_error(string2.copy(WORDS[9])));
      REQUIRE(ds_test::handle_error(map.insert(string1, string2)));
      REQUIRE(map.get_size() == 9);

      REQUIRE(ds_test::handle_error(string1.copy(WORDS[10])));
      REQUIRE(ds_test::handle_error(string2.copy(WORDS[11])));
      REQUIRE(ds_test::handle_error(map.insert(string1, std::move(string2))));
      REQUIRE(map.get_size() == 9);

      // ds::string&& key
      REQUIRE(ds_test::handle_error(string1.copy(WORDS[12])));
      REQUIRE(ds_test::handle_error(map.insert(std::move(string1), WORDS[13])));
      REQUIRE(map.get_size() == 9);

      REQUIRE(ds_test::handle_error(string1.copy(WORDS[14])));
      REQUIRE(ds_test::handle_error(string2.copy(WORDS[15])));
      REQUIRE(ds_test::handle_error(map.insert(std::move(string1), string2)));
      REQUIRE(map.get_size() == 9);

      REQUIRE(ds_test::handle_error(string1.copy(WORDS[16])));
      REQUIRE(ds_test::handle_error(string2.copy(WORDS[17])));
      REQUIRE(ds_test::handle_error(
          map.insert(std::move(string1), std::move(string2))
      ));
      REQUIRE(map.get_size() == 9);
    }

    SECTION("Clearing") {
      map.clear();
      REQUIRE(map.get_size() == 0);
      REQUIRE(map.is_empty());
    }
  }
}
