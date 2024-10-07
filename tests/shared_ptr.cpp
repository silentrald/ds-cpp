/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 *===============================*/

#include "ds/shared_ptr.hpp"
#include "./main.hpp"
#include "catch2/catch_test_macros.hpp"

using namespace ds_test;

TEST_CASE("shared_ptr primitive types", "[shared_ptr][i32]") {
  ds::shared_ptr<ds::i32> sp1{};
  REQUIRE_FALSE(sp1);

  CHECK(handle_error(sp1.set(100)));

  ds::shared_ptr<ds::i32> sp2{};
  CHECK(handle_error(sp2.copy(sp1)));

  REQUIRE(sp1.get_count() == 2);
  REQUIRE(sp2.get_count() == 2);

  REQUIRE(*sp1 == 100);
  REQUIRE(*sp2 == 100);

  *sp1 = 200;

  REQUIRE(*sp1 == 200);
  REQUIRE(*sp2 == 200);

  { // Scoping
    ds::shared_ptr<ds::i32> sp3{};
    CHECK(handle_error(sp3.copy(sp1)));

    REQUIRE(sp1.get_count() == 3);
    REQUIRE(sp3.get_count() == 3);
    REQUIRE(*sp1 == 200);
    REQUIRE(*sp3 == 200);
  }

  REQUIRE(sp1.get_count() == 2);
  REQUIRE(*sp1 == 200);

  sp2.reset();
  REQUIRE_FALSE(sp2);

  REQUIRE(sp1.get_count() == 1);
  REQUIRE(*sp1 == 200);

  CHECK(handle_error(sp2.copy(sp1)));

  REQUIRE(sp2.get_count() == 2);
  REQUIRE(*sp2 == 200);

  sp2.reset();
  REQUIRE_FALSE(sp2);

  CHECK(handle_error(sp1.copy(sp2)));

  REQUIRE_FALSE(sp1);
};

TEST_CASE("shared_ptr custom class", "[shared_ptr]") {
  // TODO: Look into this, might check another way to handle this
  // ds_test::counter = 0;

  ds::shared_ptr<Test> sp1{};
  REQUIRE_FALSE(sp1);

  CHECK(handle_error(sp1.set(Test{100})));

  ds::shared_ptr<Test> sp2{};
  CHECK(handle_error(sp2.copy(sp1)));

  REQUIRE(sp1.get_count() == 2);
  REQUIRE(sp2.get_count() == 2);

  REQUIRE(*sp1 == 100);
  REQUIRE(*sp2 == 100);

  sp1->set_int(200);

  REQUIRE(*sp1 == 200);
  REQUIRE(*sp2 == 200);

  { // Scoping
    ds::shared_ptr<Test> sp3{};
    CHECK(handle_error(sp3.copy(sp1)));

    REQUIRE(sp1.get_count() == 3);
    REQUIRE(sp3.get_count() == 3);
    REQUIRE(*sp1 == 200);
    REQUIRE(*sp3 == 200);
  }

  REQUIRE(sp1.get_count() == 2);
  REQUIRE(*sp1 == 200);

  sp2.reset();
  REQUIRE_FALSE(sp2);

  REQUIRE(sp1.get_count() == 1);
  REQUIRE(*sp1 == 200);

  CHECK(handle_error(sp2.copy(sp1)));

  REQUIRE(sp2.get_count() == 2);
  REQUIRE(*sp2 == 200);

  sp2.reset();
  REQUIRE_FALSE(sp2);

  CHECK(handle_error(sp1.copy(sp2)));

  void* ptr = &*sp1;
  REQUIRE_FALSE(sp1);

  sp1.reset();
  // REQUIRE(ds_test::counter == 0);
  // REQUIRE(ds_test::free_ptr == ptr);
};
