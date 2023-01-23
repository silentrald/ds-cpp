/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 * Updated: 2022-12-28
 *===============================*/

#include "ds/hash_set.hpp"
#include "./main.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/error.hpp"
#include "ds/string.hpp"

using namespace ds_test;

TEST_CASE("hash_set", "ds") { // NOLINT
  SECTION("Primitive type") {
    ds::hash_set<ds::i32> set{};

    SECTION("Definition") {
      REQUIRE(set.empty());
    }

    // Insert elements
    SECTION("Insert, lookup and delete") {
      CHECK(handle_error(set.insert(100)));
      CHECK(handle_error(set.insert(200)));
      CHECK(handle_error(set.insert(300)));

      REQUIRE(!set.empty());
      REQUIRE(set.size() == 3);
      REQUIRE(set.contains(100));
      REQUIRE(set.contains(200));
      REQUIRE(set.contains(300));

      set.erase(300);
      REQUIRE_FALSE(set.contains(300));

      set.erase(200);
      REQUIRE_FALSE(set.contains(200));

      set.erase(100);
      REQUIRE_FALSE(set.contains(100));

      REQUIRE(set.empty());
    }

    SECTION("Same element insertion") {
      for (ds::i32 i = 0; i < 3; ++i) {
        CHECK(handle_error(set.insert(100)));
      }

      REQUIRE(set.size() == 1);
      REQUIRE(set.contains(100));
    }

    SECTION("Delete a non-existing element") {
      CHECK(handle_error(set.insert(100)));

      REQUIRE_FALSE(set.contains(200));
      set.erase(200);
      REQUIRE_FALSE(set.contains(200));

      REQUIRE_FALSE(set.contains(300));
      set.erase(300);
      REQUIRE_FALSE(set.contains(300));
    }

    SECTION("Passing") {
      SECTION("Init Move (empty move)") {
        ds::hash_set<ds::i32> other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Init Move (non-empty move)") {
        CHECK(handle_error(set.insert(100)));
        CHECK(handle_error(set.insert(200)));
        CHECK(handle_error(set.insert(300)));

        ds::hash_set<ds::i32> other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains(100));
        REQUIRE(other.contains(200));
        REQUIRE(other.contains(300));
      }

      SECTION("Copy (empty -> empty)") {
        ds::hash_set<ds::i32> other{};
        CHECK(handle_error(other.copy(set)));

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Copy (non-empty -> empty)") {
        CHECK(handle_error(set.insert(100)));
        CHECK(handle_error(set.insert(200)));
        CHECK(handle_error(set.insert(300)));

        ds::hash_set<ds::i32> other{};
        CHECK(handle_error(other.copy(set)));

        REQUIRE_FALSE(set.empty());
        REQUIRE(other.contains(100));
        REQUIRE(other.contains(200));
        REQUIRE(other.contains(300));

        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains(100));
        REQUIRE(other.contains(200));
        REQUIRE(other.contains(300));
      }

      SECTION("Copy (empty -> non-empty)") {
        ds::hash_set<ds::i32> other{};
        CHECK(handle_error(other.insert(100)));

        CHECK(handle_error(other.insert(200)));

        CHECK(handle_error(other.insert(300)));

        CHECK(handle_error(other.copy(set)));

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Copy (non-empty -> non-empty)") {
        ds::hash_set<ds::i32> other{};
        CHECK(handle_error(other.insert(100)));
        CHECK(handle_error(other.insert(200)));
        CHECK(handle_error(other.insert(300)));

        CHECK(handle_error(set.insert(400)));
        CHECK(handle_error(set.insert(500)));
        CHECK(handle_error(set.insert(600)));

        CHECK(handle_error(other.copy(set)));

        REQUIRE_FALSE(set.empty());
        REQUIRE(set.contains(400));
        REQUIRE(set.contains(500));
        REQUIRE(set.contains(600));

        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains(400));
        REQUIRE(other.contains(500));
        REQUIRE(other.contains(600));
      }

      SECTION("Move (empty -> empty)") {
        ds::hash_set<ds::i32> other{};
        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Move (non-empty -> empty)") {
        CHECK(handle_error(set.insert(100)));
        CHECK(handle_error(set.insert(200)));
        CHECK(handle_error(set.insert(300)));

        ds::hash_set<ds::i32> other{};
        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains(100));
        REQUIRE(other.contains(200));
        REQUIRE(other.contains(300));
      }

      SECTION("Move (empty -> non-empty)") {
        ds::hash_set<ds::i32> other{};
        CHECK(handle_error(other.insert(100)));
        CHECK(handle_error(other.insert(200)));
        CHECK(handle_error(other.insert(300)));

        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Move (non-empty -> non-empty)") {
        ds::hash_set<ds::i32> other{};
        CHECK(handle_error(other.insert(100)));
        CHECK(handle_error(other.insert(200)));
        CHECK(handle_error(other.insert(300)));

        CHECK(handle_error(set.insert(400)));
        CHECK(handle_error(set.insert(500)));
        CHECK(handle_error(set.insert(600)));

        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains(400));
        REQUIRE(other.contains(500));
        REQUIRE(other.contains(600));
      }
    }

    SECTION("Big HashSet") {
      ds::i32 max_size = 1000;
      for (ds::i32 i = 0; i < max_size; ++i) {
        REQUIRE_FALSE(set.insert(i));
      }

      REQUIRE(set.size() == max_size);
      for (ds::i32 i = 0; i < max_size; ++i) {
        REQUIRE(set.contains(i));
      }

      for (ds::i32 i = 0; i < max_size; ++i) {
        set.erase(i);
      }

      REQUIRE(set.empty());
    }

    SECTION("Iterators") {
      CHECK(handle_error(set.insert(0)));
      CHECK(handle_error(set.insert(1)));
      CHECK(handle_error(set.insert(2)));
      CHECK(handle_error(set.insert(3)));
      CHECK(handle_error(set.insert(4)));

      ds::i32 count = 0;
      bool expected[] = {true, true, true, true, true}; // NOLINT
      // Ordering does not matter
      for (auto& i : set) { // NOLINT
        REQUIRE(expected[i]);
        expected[i] = false;
        ++count;
      }

      for (ds::i32 i = 0; i < 5; ++i) { // NOLINT
        expected[i] = true;
      }

      REQUIRE(count == 5);
      count = 0;

      for (const auto& i : set) {
        REQUIRE(expected[i]);
        expected[i] = false;
        ++count;
      }

      REQUIRE(count == 5);
    }
  }

  SECTION("String type") {
    ds::hash_set<ds::string> set{};

    SECTION("Definition") {
      REQUIRE(set.empty());
    }

    // Insert elements
    SECTION("Insert, lookup and delete") {
      CHECK(handle_error(set.insert("hi")));
      CHECK(handle_error(set.insert("hello")));
      CHECK(handle_error(set.insert(":^)")));

      REQUIRE(!set.empty());
      REQUIRE(set.size() == 3);
      REQUIRE(set.contains("hi"));
      REQUIRE(set.contains("hello"));
      REQUIRE(set.contains(":^)"));

      set.erase(":^)");
      REQUIRE_FALSE(set.contains(":^)"));

      set.erase("hello");
      REQUIRE_FALSE(set.contains("hello"));

      set.erase("hi");
      REQUIRE_FALSE(set.contains("hi"));

      REQUIRE(set.empty());
    }

    SECTION("Same element insertion") {
      for (ds::i32 i = 0; i < 3; ++i) {
        CHECK(handle_error(set.insert("hello world")));
      }

      REQUIRE(set.size() == 1);
      REQUIRE(set.contains("hello world"));
    }

    SECTION("Delete a non-existing element") {
      CHECK(handle_error(set.insert("hi")));

      REQUIRE_FALSE(set.contains("hello"));
      set.erase("hello");
      REQUIRE_FALSE(set.contains("hello"));

      REQUIRE_FALSE(set.contains("uwu"));
      set.erase("uwu");
      REQUIRE_FALSE(set.contains("uwu"));
    }

    SECTION("Passing") {
      SECTION("Init Move (empty move)") {
        ds::hash_set<ds::string> other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Init Move (non-empty move)") {
        CHECK(handle_error(set.insert("hi")));
        CHECK(handle_error(set.insert("uwu")));
        CHECK(handle_error(set.insert("owo")));

        ds::hash_set<ds::string> other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains("hi"));
        REQUIRE(other.contains("uwu"));
        REQUIRE(other.contains("owo"));
      }

      SECTION("Copy (empty -> empty)") {
        ds::hash_set<ds::string> other{};
        CHECK(handle_error(other.copy(set)));

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Copy (non-empty -> empty)") {
        CHECK(handle_error(set.insert("hi")));
        CHECK(handle_error(set.insert("uwu")));
        CHECK(handle_error(set.insert("owo")));

        ds::hash_set<ds::string> other{};
        CHECK(handle_error(other.copy(set)));

        REQUIRE_FALSE(set.empty());
        REQUIRE(other.contains("hi"));
        REQUIRE(other.contains("uwu"));
        REQUIRE(other.contains("owo"));

        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains("hi"));
        REQUIRE(other.contains("uwu"));
        REQUIRE(other.contains("owo"));
      }

      SECTION("Copy (empty -> non-empty)") {
        ds::hash_set<ds::string> other{};
        CHECK(handle_error(other.insert("hi")));
        CHECK(handle_error(other.insert("owo")));
        CHECK(handle_error(other.insert("uwu")));

        CHECK(handle_error(other.copy(set)));

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Copy (non-empty -> non-empty)") {
        ds::hash_set<ds::string> other{};
        CHECK(handle_error(other.insert("hi")));
        CHECK(handle_error(other.insert("owo")));
        CHECK(handle_error(other.insert("uwu")));

        CHECK(handle_error(set.insert("HI")));
        CHECK(handle_error(set.insert("OwO")));
        CHECK(handle_error(set.insert("UwU")));

        CHECK(handle_error(other.copy(set)));

        REQUIRE_FALSE(set.empty());
        REQUIRE(set.contains("HI"));
        REQUIRE(set.contains("OwO"));
        REQUIRE(set.contains("UwU"));

        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains("HI"));
        REQUIRE(other.contains("OwO"));
        REQUIRE(other.contains("UwU"));
      }

      SECTION("Move (empty -> empty)") {
        ds::hash_set<ds::string> other{};
        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Move (non-empty -> empty)") {
        CHECK(handle_error(set.insert("hi")));
        CHECK(handle_error(set.insert("uwu")));
        CHECK(handle_error(set.insert("owo")));

        ds::hash_set<ds::string> other{};
        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains("hi"));
        REQUIRE(other.contains("uwu"));
        REQUIRE(other.contains("owo"));
      }

      SECTION("Move (empty -> non-empty)") {
        ds::hash_set<ds::string> other{};
        CHECK(handle_error(other.insert("hi")));
        CHECK(handle_error(other.insert("uwu")));
        CHECK(handle_error(other.insert("owo")));

        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE(other.empty());
      }

      SECTION("Move (non-empty -> non-empty)") {
        ds::hash_set<ds::string> other{};
        CHECK(handle_error(other.insert("hi")));
        CHECK(handle_error(other.insert("uwu")));
        CHECK(handle_error(other.insert("owo")));

        CHECK(handle_error(set.insert("hi")));
        CHECK(handle_error(set.insert("UwU")));
        CHECK(handle_error(set.insert("OwO")));

        other = std::move(set);

        REQUIRE(set.empty());
        REQUIRE_FALSE(other.empty());
        REQUIRE(other.contains("hi"));
        REQUIRE(other.contains("UwU"));
        REQUIRE(other.contains("OwO"));
      }
    }

    // TODO: Add utils in ds
    /* SECTION("Big HashSet") { */
    /*   ds::i32 max_size = 1000; */
    /*   for (ds::i32 i = 0; i < max_size; ++i) { */
    /*     REQUIRE_FALSE(set.insert(i)); */
    /*   } */

    /*   REQUIRE(set.size() == max_size); */
    /*   for (ds::i32 i = 0; i < max_size; ++i) { */
    /*     REQUIRE(set.contains(i)); */
    /*   } */

    /*   for (ds::i32 i = 0; i < max_size; ++i) { */
    /*     set.erase(i); */
    /*   } */

    /*   REQUIRE(set.empty()); */
    /* } */

    SECTION("Iterators") {
      CHECK(handle_error(set.insert("a")));
      CHECK(handle_error(set.insert("b")));
      CHECK(handle_error(set.insert("c")));
      CHECK(handle_error(set.insert("d")));
      CHECK(handle_error(set.insert("e")));

      ds::i32 count = 0;
      bool expected[] = {true, true, true, true, true}; // NOLINT
      // Ordering does not matter
      for (const auto& i : set) {
        REQUIRE(expected[i.c_str()[0] - 'a']);
        expected[i.c_str()[0] - 'a'] = false;
        ++count;
      }

      REQUIRE(count == 5);
    }
  }
}

