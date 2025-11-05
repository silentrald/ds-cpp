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

TEST_CASE("string copy", "[string]") {
  const ds::c8* const WORD = "Hello World!";
  ds::string str{};
  ds::error_code error_code{};
  ds::expected<ds::c8, ds::error_code> expected{};

  error_code = str.copy(WORD);
  REQUIRE(handle_error(error_code));

  REQUIRE(str.get_size() == std::strlen(WORD));
  REQUIRE(str.get_capacity() > str.get_size());

  REQUIRE(str == WORD);

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

  expected = str.at(12);
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error_codes::INDEX_OUT_OF_BOUNDS);

  str[0] = 'h';
  str[6] = 'w';
  REQUIRE(str == "hello world!");

  str.clear();
  REQUIRE(str.get_size() == 0);
  REQUIRE(str.is_empty());
}

TEST_CASE("string push and pop", "[string]") {
  const ds::c8* const WORD = "Hello World!";
  ds::string str{};

  for (ds::i32 i = 0; i < std::strlen(WORD); ++i) {
    REQUIRE(ds_test::handle_error(str.push(WORD[i])));
  }

  REQUIRE(str == WORD);

  for (ds::i32 i = std::strlen(WORD) - 1; i > -1; --i) {
    REQUIRE(str.pop() == WORD[i]);
  }

  auto expected = str.pop_safe();
  REQUIRE_FALSE(expected);
  REQUIRE(expected.error() == ds::error_codes::CONTAINER_EMPTY);
}

// === Empty String === //

TEST_CASE("empty string definition", "[string]") {
  ds::string str{};

  REQUIRE(str.is_empty());
  REQUIRE(str.get_size() == 0);
  REQUIRE(str.get_capacity() == 0);
}

TEST_CASE("empty string comparison to const c8*", "[string]") {
  ds::string str{};

  REQUIRE(str == "");       // NOLINT
  REQUIRE("" == str);       // NOLINT
  REQUIRE_FALSE(str != ""); // NOLINT
  REQUIRE_FALSE("" != str); // NOLINT
  REQUIRE(str != "Hello World");
  REQUIRE("Hello World" != str);

  REQUIRE_FALSE(str == nullptr);
  REQUIRE_FALSE(nullptr == str);
}

TEST_CASE("empty string comparison to another string", "[string]") {
  ds::string str{};
  ds::string str2{};
  CHECK(handle_error(str2.copy("Hello World")));
  REQUIRE(str != str2);
}

TEST_CASE("init moving an empty string to another string", "[string]") {
  ds::string str{};
  ds::string str2 = std::move(str);

  REQUIRE(str2.is_empty());
  REQUIRE(str2.get_capacity() == 0);
}

TEST_CASE("copying an empty string to another string", "[string]") {
  ds::string str{};
  ds::string str2{};
  CHECK(handle_error(str2.copy("S")));
  CHECK(handle_error(str2.copy(str)));

  REQUIRE(str2 == str);
  REQUIRE(str2.get_capacity() > 0);
}

TEST_CASE("moving an empty string to another string", "[string]") {
  ds::string str{};
  ds::string str2{};
  CHECK(handle_error(str2.copy("S")));

  char* ptr = str2.data();
  str2 = std::move(str);

  REQUIRE(str2.is_empty());
  REQUIRE(str2.get_capacity() == 0);
}

TEST_CASE("empty string element accessing", "[string]") {
  ds::expected<ds::c8*, ds::error_code> exp_ptr{};
  ds::expected<ds::c8, ds::error_code> exp{};
  ds::string str{};

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
