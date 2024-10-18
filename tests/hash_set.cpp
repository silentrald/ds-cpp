/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#include "ds/hash_set.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/prime.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include <cstdlib>
#include <cstring>

// TODO: Break the test cases even further to get correct errors cases

template <typename Key> inline void test_hash_set(const ds::usize N) {
  ds::hash_set<Key> set{};

  SECTION("Insertions") {
    for (ds::usize i = 0, counter = 0; i < 10; ++i) {
      for (ds::usize j = i; j < N; j += 10) {
        REQUIRE(ds_test::handle_error(set.insert(j)));
        REQUIRE(set.get_size() == ++counter);
      }
    }

    SECTION("Accessing") {
      SECTION("Existing keys") {
        for (ds::usize i = 0; i < N; ++i) {
          REQUIRE(set.contains(i));
        }
      }

      SECTION("Non-existing keys") {
        for (ds::usize i = N + 50; i > N; --i) {
          REQUIRE_FALSE(set.contains(i));
        }
      }
    }

    SECTION("Removing") {
      SECTION("Existing keys") {
        for (ds::usize i = 0, counter = N; i < 10; ++i) {
          for (ds::usize j = i; j < N; j += 10) {
            set.remove(j);
            REQUIRE(set.get_size() == --counter);
          }
        }
      }

      SECTION("Non-existing keys") {
        for (ds::usize i = N + 50; i > N; --i) {
          set.remove(i);
          REQUIRE(N == set.get_size());
        }
      }
    }

    SECTION("Reinserting keys") {
      for (ds::usize i = 0; i < 10; ++i) {
        for (ds::usize j = i; j < N; j += 10) {
          REQUIRE(ds_test::handle_error(set.insert(j)));
          REQUIRE(set.get_size() == N);

          REQUIRE(set.contains(j));
        }
      }
    }

    SECTION("Clearing") {
      set.clear();
      REQUIRE(set.get_size() == 0);
      REQUIRE(set.is_empty());
    }
  }
}

// Check if every key is the same hash
template <typename Key> inline void test_hash_set_bad_collision() {
  ds::hash_set<Key> set{};

  const ds::usize PRIME = ds::get_first_prime();
  const ds::usize START = 1;
  const ds::usize END = 10;
  const ds::usize OFFSET = 5;
  const ds::usize SIZE = END - START;

  SECTION("Insertions (bad collision)") {
    for (ds::usize i = START, counter = 0; i < END; ++i) {
      REQUIRE(ds_test::handle_error(set.insert(i * PRIME + OFFSET)));
      REQUIRE(set.get_size() == ++counter);
    }

    SECTION("Accessing") {
      SECTION("Existing keys") {
        for (ds::usize i = START; i < END; ++i) {
          REQUIRE(set.contains(i * PRIME + OFFSET));
        }
      }

      SECTION("Non-existing keys") {
        for (ds::usize i = START; i < END; ++i) {
          REQUIRE_FALSE(set.contains(i * PRIME + OFFSET + 1));
        }
      }
    }

    SECTION("Removing") {
      SECTION("Existing keys") {
        for (ds::usize i = START, counter = SIZE; i < END; ++i) {
          set.remove(i * PRIME + OFFSET);
          REQUIRE(set.get_size() == --counter);
        }
      }
    }

    SECTION("Non-existing keys") {
      for (ds::usize i = START; i < END; ++i) {
        set.remove(i * PRIME + OFFSET + 1);
        REQUIRE(set.get_size() == SIZE);
      }
    }

    SECTION("Reinserting keys") {
      for (ds::usize i = START; i < END; ++i) {
        REQUIRE(ds_test::handle_error(set.insert(i * PRIME + OFFSET)));
        REQUIRE(set.get_size() == SIZE);

        REQUIRE(set.contains(i * PRIME + OFFSET));
      }
    }

    SECTION("Clearing") {
      set.clear();
      REQUIRE(set.get_size() == 0);
      REQUIRE(set.is_empty());
    }
  }
}

const ds::usize SIZE = 32;

TEST_CASE("hash_set<i32, i32>", "[hash_set]") {
  test_hash_set<ds::i32>(SIZE);
  test_hash_set_bad_collision<ds::i32>();
}

TEST_CASE("hash_set<u32, u32>", "[hash_set]") {
  test_hash_set<ds::u32>(SIZE);
  test_hash_set_bad_collision<ds::u32>();
}

TEST_CASE("hash_set<i64, i64>", "[hash_set]") {
  test_hash_set<ds::i64>(SIZE);
  test_hash_set_bad_collision<ds::i64>();
}

TEST_CASE("hash_set<u64, u64>", "[hash_set]") {
  test_hash_set<ds::u64>(SIZE);
  test_hash_set_bad_collision<ds::u64>();
}

TEST_CASE("hash_set<string>", "[hash_set]") {
  ds::hash_set<ds::string> set{};
  ds::string string{};
  ds::c8 characters[32]; // NOLINT

  SECTION("Insertions") {
    REQUIRE(ds_test::handle_error(set.insert("Hello")));

    REQUIRE(ds_test::handle_error(string.copy("My")));
    REQUIRE(ds_test::handle_error(set.insert(string)));

    REQUIRE(ds_test::handle_error(string.copy("Name")));
    REQUIRE(ds_test::handle_error(set.insert(std::move(string))));

    std::strcpy(characters, "Jeff");
    REQUIRE(ds_test::handle_error(set.insert(characters)));

    REQUIRE(set.get_size() == 4);

    SECTION("Accessing") {
      SECTION("Existing keys") {
        REQUIRE(set.contains("Hello"));

        REQUIRE(ds_test::handle_error(string.copy("My")));
        REQUIRE(set.contains(string));

        std::strcpy(characters, "Name");
        REQUIRE(set.contains(characters));

        REQUIRE(set.contains("Jeff"));
      }

      SECTION("Non-existing keys") {
        REQUIRE_FALSE(set.contains("Hi"));

        REQUIRE_FALSE(set.contains("There"));

        REQUIRE(ds_test::handle_error(string.copy("Some")));
        REQUIRE_FALSE(set.contains(string));

        REQUIRE(ds_test::handle_error(string.copy("World")));
        REQUIRE_FALSE(set.contains(string));
      }
    }

    SECTION("Remove") {
      SECTION("Existing keys") {
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

      SECTION("Non-existing keys") {
        set.remove("asdf");
        REQUIRE(set.get_size() == 4);

        set.remove("something");
        REQUIRE(set.get_size() == 4);

        set.remove("else");
        REQUIRE(set.get_size() == 4);

        set.remove("asdfasdf");
        REQUIRE(set.get_size() == 4);
      }
    }

    SECTION("Reinserting keys") {
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

    SECTION("Clearing") {
      set.clear();
      REQUIRE(set.get_size() == 0);
      REQUIRE(set.is_empty());
    }
  }
}
