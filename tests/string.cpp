/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-12-28
 *===============================*/

#include "ds/string.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/error.hpp"
#include <cstdlib>

using namespace ds_test;

// NOLINTNEXTLINE
TEST_CASE("string", "ds") {
  ds::exp_ptr_err<char> exp_ptr{};
  ds::exp_err<char> exp{};
  ds::string str{};
  free_ptr = nullptr;

  SECTION("Empty string") {
    SECTION("Defcopyion") {
      REQUIRE(str.empty());
      REQUIRE(str.size() == 0); // NOLINT
      REQUIRE(str.length() == 0);
      REQUIRE(str.max_size() == 0);
    }

    SECTION("Comparison to const char*") {
      REQUIRE(str == "");       // NOLINT
      REQUIRE("" == str);       // NOLINT
      REQUIRE_FALSE(str != ""); // NOLINT
      REQUIRE_FALSE("" != str); // NOLINT
      REQUIRE(str != "Hello World");
      REQUIRE("Hello World" != str);

      REQUIRE_FALSE(str == nullptr);
      REQUIRE_FALSE(nullptr == str);
    }

    SECTION("Comparison to another string") {
      ds::string str2;
      CHECK(handle_error(str2.copy("Hello World")));
      REQUIRE(str != str2);
    }

    SECTION("Passing") {
      SECTION("Init Move") {
        ds::string str2 = std::move(str);

        REQUIRE(str2.empty());
        REQUIRE(str2.max_size() == 0);
      }

      SECTION("Copy") {
        ds::string str2;
        CHECK(handle_error(str2.copy("S")));
        CHECK(handle_error(str2.copy(str)));

        REQUIRE(str2 == str);
        REQUIRE(str2.max_size() > 0);
      }

      SECTION("Move") {
        ds::string str2;
        CHECK(handle_error(str2.copy("S")));

        char* ptr = str2.data();
        str2 = std::move(str);
        REQUIRE(ptr == free_ptr);

        REQUIRE(str2.empty());
        REQUIRE(str2.max_size() == 0);
      }
    }

    SECTION("Element accessing") {
      SECTION("At") {
        exp = str.at(0);
        REQUIRE_FALSE(exp);

        exp_ptr = str.at_ptr(0);
        REQUIRE_FALSE(exp_ptr);
      }

      SECTION("Front") {
        exp = str.front();
        REQUIRE_FALSE(exp);

        exp_ptr = str.front_ptr();
        REQUIRE_FALSE(exp_ptr);
      }

      SECTION("Back") {
        exp = str.back();
        REQUIRE_FALSE(exp);

        exp_ptr = str.back_ptr();
        REQUIRE_FALSE(exp_ptr);
      }

      SECTION("Pointers") {
        char* data = str.data();
        REQUIRE(data == nullptr);

        const char* cdata = str.c_str();
        REQUIRE(cdata == nullptr);
      }
    }
  }
}
