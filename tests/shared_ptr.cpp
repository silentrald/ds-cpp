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

TEST_CASE("shared_ptr primitive types", "[shared_ptr]") {
  ds::shared_ptr<ds::i32> sp1{};
  REQUIRE_FALSE(sp1);

  REQUIRE(handle_error(sp1.set(100)));

  ds::shared_ptr<ds::i32> sp2{};
  REQUIRE(handle_error(sp2.copy(sp1)));

  REQUIRE(sp1.get_count() == 2);
  REQUIRE(sp2.get_count() == 2);

  REQUIRE(*sp1 == 100);
  REQUIRE(*sp2 == 100);

  *sp1 = 200;

  REQUIRE(*sp1 == 200);
  REQUIRE(*sp2 == 200);

  { // Scoping
    ds::shared_ptr<ds::i32> sp3{};
    REQUIRE(handle_error(sp3.copy(sp1)));

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

  REQUIRE(handle_error(sp2.copy(sp1)));

  REQUIRE(sp2.get_count() == 2);
  REQUIRE(*sp2 == 200);

  sp2.reset();
  REQUIRE_FALSE(sp2);

  REQUIRE(handle_error(sp1.copy(sp2)));

  REQUIRE_FALSE(sp1);
};

TEST_CASE("shared_ptr custom class", "[shared_ptr]") {
  // TODO: Look into this, might check another way to handle this
  ds::isize counter = 0;

  ds::shared_ptr<Test> sp1{};
  REQUIRE_FALSE(sp1);

  REQUIRE(handle_error(sp1.set(Test{&counter, 100})));
  REQUIRE(counter == 1);

  ds::shared_ptr<Test> sp2{};
  REQUIRE(handle_error(sp2.copy(sp1)));

  REQUIRE(sp1.get_count() == 2);
  REQUIRE(sp2.get_count() == 2);

  REQUIRE(*sp1 == 100);
  REQUIRE(*sp2 == 100);

  sp1->set_value(200);

  REQUIRE(*sp1 == 200);
  REQUIRE(*sp2 == 200);

  { // Scoping
    ds::shared_ptr<Test> sp3{};
    REQUIRE(handle_error(sp3.copy(sp1)));

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

  REQUIRE(handle_error(sp2.copy(sp1)));

  REQUIRE(sp2.get_count() == 2);
  REQUIRE(*sp2 == 200);

  sp2.reset();
  REQUIRE_FALSE(sp2);

  REQUIRE(handle_error(sp1.copy(sp2)));

  void* ptr = &*sp1;
  REQUIRE_FALSE(sp1);

  sp1.reset();
  REQUIRE(counter == 0);
};
