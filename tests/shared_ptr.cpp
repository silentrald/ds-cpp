/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-30
 * Updated: 2022-12-30
 *===============================*/

#include "ds/shared_ptr.hpp"
#include "catch2/catch_test_macros.hpp"
#include "test.hpp"

using namespace ds_test;

// NOLINTNEXTLINE
TEST_CASE("shared_ptr", "ds") {
  SECTION("Primitive type") {
    ds::shared_ptr<ds::i32> sp1{};
    REQUIRE_FALSE(sp1);

    CHECK(handle_error(sp1.set(100)));

    ds::shared_ptr<ds::i32> sp2{};
    CHECK(handle_error(sp2.copy(sp1)));

    REQUIRE(sp1.count() == 2);
    REQUIRE(sp2.count() == 2);

    REQUIRE(*sp1 == 100);
    REQUIRE(*sp2 == 100);

    *sp1 = 200;

    REQUIRE(*sp1 == 200);
    REQUIRE(*sp2 == 200);

    { // Scoping
      ds::shared_ptr<ds::i32> sp3{};
      CHECK(handle_error(sp3.copy(sp1)));

      REQUIRE(sp1.count() == 3);
      REQUIRE(sp3.count() == 3);
      REQUIRE(*sp1 == 200);
      REQUIRE(*sp3 == 200);
    }

    REQUIRE(sp1.count() == 2);
    REQUIRE(*sp1 == 200);

    sp2.release();
    REQUIRE_FALSE(sp2);

    REQUIRE(sp1.count() == 1);
    REQUIRE(*sp1 == 200);

    CHECK(handle_error(sp2.copy(sp1)));

    REQUIRE(sp2.count() == 2);
    REQUIRE(*sp2 == 200);

    sp2.release();
    REQUIRE_FALSE(sp2);

    CHECK(handle_error(sp1.copy(sp2)));

    REQUIRE_FALSE(sp1);
  };

  SECTION("Custom Class type") {
    ds_test::counter = 0;

    ds::shared_ptr<Test> sp1{};
    REQUIRE_FALSE(sp1);

    CHECK(handle_error(sp1.set(Test{100})));

    ds::shared_ptr<Test> sp2{};
    CHECK(handle_error(sp2.copy(sp1)));

    REQUIRE(sp1.count() == 2);
    REQUIRE(sp2.count() == 2);

    REQUIRE(*sp1 == 100);
    REQUIRE(*sp2 == 100);

    sp1->set_int(200);

    REQUIRE(*sp1 == 200);
    REQUIRE(*sp2 == 200);

    { // Scoping
      ds::shared_ptr<Test> sp3{};
      CHECK(handle_error(sp3.copy(sp1)));

      REQUIRE(sp1.count() == 3);
      REQUIRE(sp3.count() == 3);
      REQUIRE(*sp1 == 200);
      REQUIRE(*sp3 == 200);
    }

    REQUIRE(sp1.count() == 2);
    REQUIRE(*sp1 == 200);

    sp2.release();
    REQUIRE_FALSE(sp2);

    REQUIRE(sp1.count() == 1);
    REQUIRE(*sp1 == 200);

    CHECK(handle_error(sp2.copy(sp1)));

    REQUIRE(sp2.count() == 2);
    REQUIRE(*sp2 == 200);

    sp2.release();
    REQUIRE_FALSE(sp2);

    CHECK(handle_error(sp1.copy(sp2)));

    void* ptr = &*sp1;
    REQUIRE_FALSE(sp1);

    REQUIRE(ds_test::counter == 0);
    REQUIRE(ds_test::free_ptr == ptr);
  };
};

