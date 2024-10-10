/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#include "ds/string.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include <cstdlib>
#include <cstring>

using namespace ds_test;

TEST_CASE("string normal use case", "[string]") {
  ds::string str{};
  ds::error_code error_code{};
  ds::expected<ds::c8, ds::error_code> expected{};

  error_code = str.copy("Hello World!");
  REQUIRE(handle_error(error_code));

  REQUIRE(str.get_size() == std::strlen("Hello World!"));

  REQUIRE(str[0] == 'H');
  REQUIRE(str[2] == 'l');
  REQUIRE(str[4] == 'o');

  expected = str.at(6);
  REQUIRE(handle_error(expected));
  REQUIRE(*expected == 'W');

  expected = str.at(8);
  REQUIRE(handle_error(expected));
  REQUIRE(*expected == 'r');

  expected = str.at(10);
  REQUIRE(handle_error(expected));
  REQUIRE(*expected == 'd');

  str.clear();
  REQUIRE(str.get_size() == 0);
  REQUIRE(str.is_empty());
}

TEST_CASE("string empty handling", "[string]") {
  ds::expected<ds::c8*, ds::error_code> exp_ptr{};
  ds::expected<ds::c8, ds::error_code> exp{};
  ds::string str{};
  free_ptr = nullptr;

  SECTION("Definition") {
    REQUIRE(str.is_empty());
    REQUIRE(str.get_size() == 0);
    REQUIRE(str.get_capacity() == 0);
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

      REQUIRE(str2.is_empty());
      REQUIRE(str2.get_capacity() == 0);
    }

    SECTION("Copy") {
      ds::string str2;
      CHECK(handle_error(str2.copy("S")));
      CHECK(handle_error(str2.copy(str)));

      REQUIRE(str2 == str);
      REQUIRE(str2.get_capacity() > 0);
    }

    SECTION("Move") {
      ds::string str2;
      CHECK(handle_error(str2.copy("S")));

      char* ptr = str2.data();
      str2 = std::move(str);
      REQUIRE(ptr == free_ptr);

      REQUIRE(str2.is_empty());
      REQUIRE(str2.get_capacity() == 0);
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
      ds::c8 c = str.front();
      REQUIRE(c == '\0');

      ds::c8* c_ptr = str.front_ptr();
      REQUIRE(c_ptr == nullptr);
    }

    SECTION("Back") {
      ds::c8 c = str.back();
      REQUIRE(c == '\0');

      ds::c8* c_ptr = str.front_ptr();
      REQUIRE(c_ptr == nullptr);
    }

    SECTION("Pointers") {
      char* data = str.data();
      REQUIRE(data == nullptr);

      const char* cdata = str.c_str();
      REQUIRE(strcmp(cdata, "") == 0);
    }
  }
}

TEST_CASE("string substring", "[string]") {
  ds::string str{};
  ds::error_code error_code{};
  ds::expected<ds::string, ds::error_code> expected{};

  error_code = str.copy("Hello World");
  REQUIRE(ds_test::handle_error(error_code));

  expected = str.substring(0, 5);
  REQUIRE(ds_test::handle_error(expected));
  REQUIRE(*expected == "Hello");

  expected = str.substring(6);
  REQUIRE(handle_error(expected));
  REQUIRE(*expected == "World");
}
