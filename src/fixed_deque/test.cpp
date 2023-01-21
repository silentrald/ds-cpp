/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-12-28
 *===============================*/

#include "../test.hpp"
#include "../error.hpp"
#include "../types.hpp"
#include "./def.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include <cstdio>
#include <iostream>

using namespace ds_test;

// NOLINTNEXTLINE
TEST_CASE("fixed_deque", "ds") {
  SECTION("Integer Primitive") {
    ds::fixed_deque<ds::i32> dq;
    ds::expected<ds::i32> exp;
    ds::expected_ptr<ds::i32> exp_ptr;

    SECTION("Negative Size") {
      auto error = dq.resize(-1);
      REQUIRE(error);
    }

    SECTION("Size 0") {
      CHECK(handle_error(dq.resize(0)));

      SECTION("Defresizeion checks") {
        REQUIRE(dq.empty());
        REQUIRE(dq.full());
        REQUIRE(dq.empty());
        REQUIRE(dq.capacity() == 0);
      }

      SECTION("Peek Error") {
        SECTION("Front") {
          exp_ptr = dq.front();
          REQUIRE_FALSE(exp_ptr);
        }

        SECTION("Back") {
          exp_ptr = dq.back();
          REQUIRE_FALSE(exp_ptr);
        }
      }

      SECTION("Push Error") {
        SECTION("Back") {
          auto error = dq.push_back(1);
          REQUIRE(error);
        }

        SECTION("Front") {
          auto error = dq.push_front(1);
          REQUIRE(error);
        }
      }

      SECTION("Pop Errors") {
        SECTION("Back") {
          auto exp = dq.pop_back();
          REQUIRE_FALSE(exp);
        }

        SECTION("Front") {
          auto exp = dq.pop_front();
          REQUIRE_FALSE(exp);
        }
      }

      SECTION("Iterator") {
        for (auto i : dq) {
          CHECK(false);
        }
      }
    }

    SECTION("Size 1") {
      CHECK(handle_error(dq.resize(1)));

      SECTION("Defresizeion Check") {
        REQUIRE(dq.empty());
        REQUIRE_FALSE(dq.full());
        REQUIRE(dq.capacity() == 1);
      }

      SECTION("Successful Operations") {
        SECTION("Push Bzack / Pop Back") {
          CHECK(handle_error(dq.push_back(1)));

          REQUIRE(dq[0] == 1);

          exp_ptr = dq.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 1);

          exp_ptr = dq.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 1);

          exp_ptr = dq.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 1);

          exp = dq.pop_back();
          CHECK(handle_error(exp));
          REQUIRE(*exp == 1);
        }

        SECTION("Push Front / Pop Back") {
          CHECK(handle_error(dq.push_front(3)));

          REQUIRE(dq[0] == 3);

          exp_ptr = dq.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 3);

          exp_ptr = dq.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 3);

          exp_ptr = dq.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 3);

          exp = dq.pop_back();
          CHECK(handle_error(exp));
          REQUIRE(*exp == 3);
        }

        SECTION("Push Front / Pop Front") {
          CHECK(handle_error(dq.push_front(4)));

          REQUIRE(dq[0] == 4);

          exp_ptr = dq.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 4);

          exp_ptr = dq.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 4);

          exp_ptr = dq.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 4);

          exp = dq.pop_front();
          CHECK(handle_error(exp));
          REQUIRE(*exp == 4);
        }
      }

      SECTION("Invalid Operations") {
        SECTION("Immediate pop") {
          exp = dq.pop_back();
          REQUIRE_FALSE(exp);

          exp = dq.pop_front();
          REQUIRE_FALSE(exp);
        }

        SECTION("Overflow") {
          SECTION("Push back first") {
            CHECK(handle_error(dq.push_back(1)));

            auto error = dq.push_back(2);
            REQUIRE(error);

            error = dq.push_front(3);
            REQUIRE(error);

            // The dq should still be useable
            exp = dq.pop_back();
            CHECK(handle_error(exp));
            REQUIRE(*exp == 1);
          }

          SECTION("Push front first") {
            CHECK(handle_error(dq.push_front(1)));

            auto error = dq.push_back(2);
            REQUIRE(error);

            error = dq.push_front(3);
            REQUIRE(error);

            // The dq should still be useable
            exp = dq.pop_back();
            CHECK(handle_error(exp));
            REQUIRE(*exp == 1);
          }
        }

        SECTION("Underflow") {
          CHECK(handle_error(dq.push_back(1)));
          exp = dq.pop_back();

          exp = dq.pop_back();
          REQUIRE_FALSE(exp);

          exp = dq.pop_front();
          REQUIRE_FALSE(exp);
        }

        SECTION("Out of range") {
          CHECK(handle_error(dq.push_back(1)));

          exp_ptr = dq.at(1);
          REQUIRE_FALSE(exp_ptr);

          exp_ptr = dq.at(-1);
          REQUIRE_FALSE(exp_ptr);
        }
      }
    }

    SECTION("Size 2") {
      CHECK(handle_error(dq.resize(2)));

      SECTION("Front Looping") {
        auto* ptr = &dq[0];

        CHECK(handle_error(dq.push_back(0)));
        CHECK(handle_error(dq.push_back(1)));

        // [ x 1 2 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(2)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 0);
        REQUIRE(ptr[2] == 2);

        // [ 3 x 2 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(3)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 1);
        REQUIRE(ptr[0] == 3);

        // [ 3 4 x ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(4)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 2);
        REQUIRE(ptr[1] == 4);

        // [ x 4 5 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(5)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 3);
        REQUIRE(ptr[2] == 5);

        // [ 6 x 5 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(6)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 4);
        REQUIRE(ptr[0] == 6);
      }

      SECTION("Back Looping") {
        auto* ptr = &dq[0];

        CHECK(handle_error(dq.push_back(1)));
        CHECK(handle_error(dq.push_back(0)));

        // [ 1 x 2 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(2)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 0);
        REQUIRE(ptr[2] == 2);

        // [ x 3 2 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(3)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 1);
        REQUIRE(ptr[1] == 3);

        // [ 4 3 x ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(4)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 2);
        REQUIRE(ptr[0] == 4);

        // [ 4 x 5 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(5)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 3);
        REQUIRE(ptr[2] == 5);

        // [ x 6 5 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(6)));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 4);
        REQUIRE(ptr[1] == 6);
      }
    }

    // Normal check
    SECTION("Size 5") {
      CHECK(handle_error(dq.resize(5)));

      SECTION("Size Check") {
        REQUIRE(dq.empty());
        REQUIRE(dq.empty());

        // Head <= Tail
        CHECK(handle_error(dq.push_back(1)));
        CHECK(handle_error(dq.push_back(2)));

        REQUIRE(dq.size() == 2);
        REQUIRE_FALSE(dq.empty());
        REQUIRE_FALSE(dq.full());

        CHECK(handle_error(dq.push_back(1)));
        CHECK(handle_error(dq.push_back(2)));

        REQUIRE(dq.size() == 4);

        // Try to overflow
        CHECK(handle_error(dq.push_back(1)));
        auto error = dq.push_back(2);
        REQUIRE(error);

        REQUIRE(dq.size() == 5);
        REQUIRE(dq.full());

        dq.clear();

        REQUIRE(dq.empty());
        REQUIRE(dq.empty());

        // Try to underflow
        exp = dq.pop_back();
        exp = dq.pop_back();
        REQUIRE(error);

        REQUIRE(dq.empty());

        // Head > Tail
        CHECK(handle_error(dq.push_front(1)));

        REQUIRE(dq.size() == 1);

        CHECK(handle_error(dq.push_front(2)));

        REQUIRE(dq.size() == 2);

        CHECK(handle_error(dq.push_front(1)));
        CHECK(handle_error(dq.push_front(2)));

        REQUIRE(dq.size() == 4);
      }

      SECTION("Passing") {
        SECTION("Move (empty)") {
          ds::fixed_deque<ds::i32> dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq2.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.capacity() == 5);
        }

        SECTION("Move (non-empty") {
          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(0)));

          ds::fixed_deque<ds::i32> dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.size() == 3);
          REQUIRE(dq2.capacity() == 5);

          for (auto it : dq) {
            CHECK(false);
          }

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }
        }

        SECTION("Copy (empty -> empty)") {
          ds::fixed_deque<ds::i32> dq2;
          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());
        }

        SECTION("Copy (non-empty -> empty)") {
          ds::fixed_deque<ds::i32> dq2;

          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(3)));

          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());

          ds::expected_ptr<ds::i32> exp_ptr2;
          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            exp_ptr2 = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(exp_ptr2);
            REQUIRE(**exp_ptr == **exp_ptr2);
          }
        }

        SECTION("Copy (empty -> non-empty)") {
          ds::fixed_deque<ds::i32> dq2;
          CHECK(handle_error(dq2.resize(3)));

          CHECK(handle_error(dq2.push_front(1)));
          CHECK(handle_error(dq2.push_front(2)));
          CHECK(handle_error(dq2.push_front(3)));

          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());
        }

        SECTION("Copy (non-empty -> non-empty)") {
          ds::fixed_deque<ds::i32> dq2;
          CHECK(handle_error(dq2.resize(2)));

          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(3)));

          CHECK(handle_error(dq2.push_back(1)));
          CHECK(handle_error(dq2.push_back(1)));

          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());

          ds::expected_ptr<ds::i32> exp_ptr2;
          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            exp_ptr2 = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(exp_ptr2);
            REQUIRE(**exp_ptr == **exp_ptr2);
          }
        }

        SECTION("Move (empty -> empty)") {
          ds::fixed_deque<ds::i32> dq2;
          dq2 = std::move(dq);

          REQUIRE(dq.empty());
          REQUIRE(dq2.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.capacity() == 5);
        }

        SECTION("Move (non-empty -> empty)") {
          ds::fixed_deque<ds::i32> dq2;

          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(0)));

          dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.size() == 3);
          REQUIRE(dq2.capacity() == 5);

          for (auto it : dq) {
            CHECK(false);
          }

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }
        }

        SECTION("Move (empty -> non-empty)") {
          ds::fixed_deque<ds::i32> dq2;
          CHECK(handle_error(dq2.resize(3)));

          CHECK(handle_error(dq2.push_front(2)));
          CHECK(handle_error(dq2.push_front(1)));
          CHECK(handle_error(dq2.push_front(0)));

          dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.empty());
          REQUIRE(dq2.capacity() == 5);
        }

        SECTION("Move (non-empty -> non-empty)") {
          ds::fixed_deque<ds::i32> dq2;
          CHECK(handle_error(dq2.resize(3)));
          CHECK(handle_error(dq2.push_front(3)));
          CHECK(handle_error(dq2.push_front(4)));
          CHECK(handle_error(dq2.push_front(5)));

          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(0)));

          dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.size() == 3);
          REQUIRE(dq2.capacity() == 5);

          for (auto it : dq) {
            CHECK(false);
          }

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }
        }
      }

      SECTION("Peeking") {
        // Initialize the data
        CHECK(handle_error(dq.push_back(0)));
        CHECK(handle_error(dq.push_back(1)));
        CHECK(handle_error(dq.push_back(2)));
        CHECK(handle_error(dq.push_back(3)));
        CHECK(handle_error(dq.push_back(4)));

        exp_ptr = dq.front();
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 0);

        exp_ptr = dq.back();
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 4);

        exp_ptr = dq.at(0);
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 0);
        exp_ptr = dq.at(2);
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 2);
        exp_ptr = dq.at(4);
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 4);
      }

      SECTION("Iterator") {
        CHECK(handle_error(dq.push_back(0)));
        CHECK(handle_error(dq.push_back(1)));
        CHECK(handle_error(dq.push_back(2)));
        CHECK(handle_error(dq.push_back(3)));
        CHECK(handle_error(dq.push_back(4)));

        ds::i32 i = 0;
        for (auto di : dq) {
          REQUIRE(di == i++);
        }
      }

      SECTION("Out of range") {
        CHECK(handle_error(dq.push_back(0)));
        CHECK(handle_error(dq.push_back(1)));
        CHECK(handle_error(dq.push_back(2)));
        CHECK(handle_error(dq.push_back(3)));
        CHECK(handle_error(dq.push_back(4)));

        exp_ptr = dq.at(-1);
        REQUIRE_FALSE(exp_ptr);
        exp_ptr = dq.at(5);
        REQUIRE_FALSE(exp_ptr);
      }
    }

    SECTION("Resizing") {
      SECTION("Empty") {
        SECTION("Bigger") {
          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);

          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);
        }

        SECTION("Smaller") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
        }
      }

      SECTION("Data is Shifted") {
        // data can be shifted
        SECTION("Bigger") {
          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);

          CHECK(handle_error(dq.push_back(0)));
          CHECK(handle_error(dq.push_back(1)));
          CHECK(handle_error(dq.push_back(2)));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);

          CHECK(handle_error(dq.push_back(3)));

          CHECK(handle_error(dq.resize(5)));

          REQUIRE(dq.size() == 3);
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(4)));
          CHECK(handle_error(dq.push_back(5)));

          for (ds::i32 i = 0; i < 5; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }
        }

        SECTION("Smaller") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(0)));
          CHECK(handle_error(dq.push_back(1)));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);
          CHECK(handle_error(dq.push_back(2)));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 2);

          for (ds::i32 i = 0; i < 2; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }
        }

        SECTION("Smaller Exact") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(0)));
          CHECK(handle_error(dq.push_back(1)));
          CHECK(handle_error(dq.push_back(2)));
          CHECK(handle_error(dq.push_back(3)));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }
        }

        SECTION("Smaller Full") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(0)));
          CHECK(handle_error(dq.push_back(1)));
          CHECK(handle_error(dq.push_back(2)));
          CHECK(handle_error(dq.push_back(3)));
          CHECK(handle_error(dq.push_back(4)));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);
          CHECK(handle_error(dq.push_back(5)));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }
        }
      }

      SECTION("Data is Wrapped") {
        // data is wrapped
        // | xxT__Hxx |
        SECTION("Bigger") {
          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);

          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(0)));

          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);
          REQUIRE(dq.size() == 3);

          CHECK(handle_error(dq.push_back(3)));
          CHECK(handle_error(dq.push_back(4)));

          for (ds::i32 i = 0; i < 5; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }
        }

        SECTION("Smaller") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(0)));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 2);

          for (ds::i32 i = 0; i < 2; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }
        }

        SECTION("Smaller Exact") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(0)));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }
        }

        SECTION("Smaller Full") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_front(4)));
          CHECK(handle_error(dq.push_front(3)));
          CHECK(handle_error(dq.push_front(2)));
          CHECK(handle_error(dq.push_front(1)));
          CHECK(handle_error(dq.push_front(0)));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }
        }
      }
    }
  }

  SECTION("Custom Class") {
    ds::fixed_deque<Test> dq{};
    ds::expected<Test> exp{};
    ds::expected_ptr<Test> exp_ptr{};

    SECTION("Negative Size") {
      auto error = dq.resize(-1);
      REQUIRE(error);
    }

    SECTION("Size 0") {
      CHECK(handle_error(dq.resize(0)));

      SECTION("Defresizeion checks") {
        REQUIRE(dq.empty());
        REQUIRE(dq.full());
        REQUIRE(dq.empty());
        REQUIRE(dq.capacity() == 0);
      }

      SECTION("Peek Error") {
        SECTION("Back") {
          exp_ptr = dq.front();
          REQUIRE_FALSE(exp_ptr);
        }

        SECTION("Front") {
          exp_ptr = dq.back();
          REQUIRE_FALSE(exp_ptr);
        }
      }

      SECTION("Push Error") {
        SECTION("Back") {
          auto error = dq.push_back(Test{1});
          REQUIRE(error);
        }

        SECTION("Front") {
          auto error = dq.push_front(Test{1});
          REQUIRE(error);
        }
      }

      SECTION("Pop Errors") {
        SECTION("Back") {
          auto exp = dq.pop_back();
          REQUIRE_FALSE(exp);
        }

        SECTION("Front") {
          auto exp = dq.pop_front();
          REQUIRE_FALSE(exp);
        }
      }

      SECTION("Iterator") {
        for (auto& c : dq) {
          CHECK(false);
        }
      }
    }

    SECTION("Size 1") {
      counter = 0;
      CHECK(handle_error(dq.resize(1)));

      SECTION("Defresizeion Check") {
        REQUIRE(dq.empty());
        REQUIRE_FALSE(dq.full());
        REQUIRE(dq.empty());
        REQUIRE(dq.capacity() == 1);
      }

      SECTION("Successful Operations") {
        SECTION("Push Back / Pop Back") {
          CHECK(handle_error(dq.push_back(Test{1})));

          REQUIRE(dq[0] == 1);

          exp_ptr = dq.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 1);

          exp_ptr = dq.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 1);

          exp_ptr = dq.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 1);

          exp = dq.pop_back();
          CHECK(handle_error(exp));
          REQUIRE(*exp == 1);

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }

        SECTION("Push Back / Pop Front") {
          CHECK(handle_error(dq.push_back(Test{2})));

          REQUIRE(dq[0] == 2);

          exp_ptr = dq.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 2);

          exp_ptr = dq.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 2);

          exp_ptr = dq.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 2);

          exp = dq.pop_front();
          CHECK(handle_error(exp));
          REQUIRE(*exp == 2);

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }

        SECTION("Push Front / Pop Back") {
          CHECK(handle_error(dq.push_front(Test{3})));

          REQUIRE(dq[0] == 3);

          exp_ptr = dq.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 3);

          exp_ptr = dq.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 3);

          exp_ptr = dq.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 3);

          exp = dq.pop_back();
          CHECK(handle_error(exp));
          REQUIRE(*exp == 3);

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }

        SECTION("Push Front / Pop Front") {
          CHECK(handle_error(dq.push_front(Test{4})));

          REQUIRE(dq[0] == 4);

          exp_ptr = dq.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 4);

          exp_ptr = dq.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 4);

          exp_ptr = dq.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE(**exp_ptr == 4);

          exp = dq.pop_front();
          CHECK(handle_error(exp));
          REQUIRE(*exp == 4);

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }
      }

      SECTION("Invalid Operations") {
        SECTION("Immediate pop") {
          exp = dq.pop_back();
          REQUIRE_FALSE(exp);

          exp = dq.pop_front();
          REQUIRE_FALSE(exp);
        }

        SECTION("Overflow") {
          SECTION("Push back first") {
            CHECK(handle_error(dq.push_back(Test{1})));

            auto error = dq.push_back(Test{2});
            REQUIRE(error);

            error = dq.push_front(Test{3});
            REQUIRE(error);

            // The dq should still be useable
            exp = dq.pop_back();
            CHECK(handle_error(exp));
            REQUIRE(*exp == 1);
          }

          SECTION("Push front first") {
            CHECK(handle_error(dq.push_front(Test{1})));

            auto error = dq.push_back(Test{2});
            REQUIRE(error);

            error = dq.push_front(Test{3});
            REQUIRE(error);

            // The dq should still be useable
            exp = dq.pop_back();
            CHECK(handle_error(exp));
            REQUIRE(*exp == 1);
          }
        }

        SECTION("Underflow") {
          CHECK(handle_error(dq.push_back(Test{1})));
          exp = dq.pop_back();

          exp = dq.pop_back();
          REQUIRE_FALSE(exp);

          exp = dq.pop_front();
          REQUIRE_FALSE(exp);
        }

        SECTION("Out of range") {
          CHECK(handle_error(dq.push_back(Test{1})));

          exp_ptr = dq.at(1);
          REQUIRE_FALSE(exp_ptr);

          exp_ptr = dq.at(-1);
          REQUIRE_FALSE(exp_ptr);
        }
      }
    }

    SECTION("Size 2") {
      CHECK(handle_error(dq.resize(2)));

      SECTION("Front Looping") {
        auto* ptr = &dq[0];

        CHECK(handle_error(dq.push_back(Test{0})));
        CHECK(handle_error(dq.push_back(Test{1})));

        // [ x 1 2 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(Test{2})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 0);
        REQUIRE(ptr[2] == 2);

        // [ 3 x 2 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(Test{3})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 1);
        REQUIRE(ptr[0] == 3);

        // [ 3 4 x ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(Test{4})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 2);
        REQUIRE(ptr[1] == 4);

        // [ x 4 5 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(Test{5})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 3);
        REQUIRE(ptr[2] == 5);

        // [ 6 x 5 ]
        exp = dq.pop_front();
        CHECK(handle_error(dq.push_back(Test{6})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 4);
        REQUIRE(ptr[0] == 6);
      }

      SECTION("Back Looping") {
        auto* ptr = &dq[0];

        CHECK(handle_error(dq.push_back(Test{1})));
        CHECK(handle_error(dq.push_back(Test{0})));

        // [ 1 x 2 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(Test{2})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 0);
        REQUIRE(ptr[2] == 2);

        // [ x 3 2 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(Test{3})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 1);
        REQUIRE(ptr[1] == 3);

        // [ 4 3 x ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(Test{4})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 2);
        REQUIRE(ptr[0] == 4);

        // [ 4 x 5 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(Test{5})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 3);
        REQUIRE(ptr[2] == 5);

        // [ x 6 5 ]
        exp = dq.pop_back();
        CHECK(handle_error(dq.push_front(Test{6})));
        CHECK(handle_error(exp));
        REQUIRE(*exp == 4);
        REQUIRE(ptr[1] == 6);
      }
    }

    // Normal check
    SECTION("Size 5") {
      counter = 0;
      CHECK(handle_error(dq.resize(5)));

      SECTION("Size Check") {
        REQUIRE(dq.empty());

        // Head <= Tail
        CHECK(handle_error(dq.push_back(Test{1})));
        CHECK(handle_error(dq.push_back(Test{2})));

        REQUIRE(dq.size() == 2);
        REQUIRE_FALSE(dq.empty());
        REQUIRE_FALSE(dq.full());

        CHECK(handle_error(dq.push_back(Test{1})));
        CHECK(handle_error(dq.push_back(Test{2})));

        REQUIRE(dq.size() == 4);

        // Try to overflow
        CHECK(handle_error(dq.push_back(Test{1})));
        auto error = dq.push_back(Test{2});
        REQUIRE(error);

        REQUIRE(dq.size() == 5);
        REQUIRE(dq.full());

        dq.clear();

        REQUIRE(dq.empty());

        // Try to underflow
        exp = dq.pop_back();
        exp = dq.pop_back();
        REQUIRE_FALSE(exp);

        REQUIRE(dq.empty());

        // Head > Tail
        CHECK(handle_error(dq.push_front(Test{1})));

        REQUIRE(dq.size() == 1);

        CHECK(handle_error(dq.push_front(Test{2})));

        REQUIRE(dq.size() == 2);

        CHECK(handle_error(dq.push_front(Test{1})));
        CHECK(handle_error(dq.push_front(Test{2})));

        REQUIRE(dq.size() == 4);

        dq.~fixed_deque();
        REQUIRE(counter == 0);
      }

      SECTION("Passing") {
        SECTION("Init Move (empty)") {
          ds::fixed_deque<Test> dq2 = std::move(dq);

          REQUIRE(dq.empty());
          REQUIRE(dq2.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.capacity() == 5);

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Init Move (non-empty)") {
          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{0})));

          ds::fixed_deque<Test> dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.size() == 3);
          REQUIRE(dq2.capacity() == 5);

          for (auto& it : dq) {
            CHECK(false);
          }

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Copy (empty -> empty)") {
          ds::fixed_deque<Test> dq2;
          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Copy (non-empty -> empty)") {
          ds::fixed_deque<Test> dq2;

          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{3})));

          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());

          ds::expected_ptr<Test> exp_ptr2;
          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            exp_ptr2 = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(exp_ptr2);
            REQUIRE(**exp_ptr == **exp_ptr2);
          }

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Copy (empty -> non-empty)") {
          ds::fixed_deque<Test> dq2;
          CHECK(handle_error(dq2.resize(3)));
          CHECK(handle_error(dq2.push_front(Test{1})));
          CHECK(handle_error(dq2.push_front(Test{2})));
          CHECK(handle_error(dq2.push_front(Test{3})));

          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Copy (non-empty -> non-empty)") {
          ds::fixed_deque<Test> dq2{};
          CHECK(handle_error(dq2.resize(2)));

          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{3})));

          CHECK(handle_error(dq2.push_back(Test{1})));
          CHECK(handle_error(dq2.push_back(Test{1})));

          CHECK(handle_error(dq2.copy(dq)));

          REQUIRE(dq.capacity() == dq2.capacity());
          REQUIRE(dq.size() == dq2.size());

          ds::expected_ptr<Test> exp_ptr2;
          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            exp_ptr2 = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(exp_ptr2);
            REQUIRE(**exp_ptr == **exp_ptr2);
          }

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Move (empty -> empty)") {
          ds::fixed_deque<Test> dq2;
          dq2 = std::move(dq);

          REQUIRE(dq.empty());
          REQUIRE(dq2.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.capacity() == 5);

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Move (non-empty -> empty)") {
          ds::fixed_deque<Test> dq2;

          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{0})));

          dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.size() == 3);
          REQUIRE(dq2.capacity() == 5);

          for (auto& it : dq) {
            CHECK(false);
          }

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Move (empty -> non-empty)") {
          ds::fixed_deque<Test> dq2;
          CHECK(handle_error(dq2.resize(3)));

          CHECK(handle_error(dq2.push_front(Test{2})));
          CHECK(handle_error(dq2.push_front(Test{1})));
          CHECK(handle_error(dq2.push_front(Test{0})));

          dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.empty());
          REQUIRE(dq2.capacity() == 5);

          for (auto& it : dq) {
            CHECK(false);
          }

          for (auto& it : dq2) {
            CHECK(false);
          }

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Move (non-empty -> non-empty)") {
          ds::fixed_deque<Test> dq2;
          CHECK(handle_error(dq2.resize(3)));

          CHECK(handle_error(dq2.push_front(Test{3})));
          CHECK(handle_error(dq2.push_front(Test{4})));
          CHECK(handle_error(dq2.push_front(Test{5})));

          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{0})));

          dq2 = std::move(dq);
          REQUIRE(dq.empty());
          REQUIRE(dq.capacity() == 0);
          REQUIRE(dq2.size() == 3);
          REQUIRE(dq2.capacity() == 5);

          for (auto& it : dq) {
            CHECK(false);
          }

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq2.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }

          dq.~fixed_deque();
          dq2.~fixed_deque();
          REQUIRE(counter == 0);
        }
      }

      SECTION("Peeking") {
        // Initialize the data
        CHECK(handle_error(dq.push_back(Test{0})));
        CHECK(handle_error(dq.push_back(Test{1})));
        CHECK(handle_error(dq.push_back(Test{2})));
        CHECK(handle_error(dq.push_back(Test{3})));
        CHECK(handle_error(dq.push_back(Test{4})));

        exp_ptr = dq.front();
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 0);

        exp_ptr = dq.back();
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 4);

        exp_ptr = dq.at(0);
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 0);
        exp_ptr = dq.at(2);
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 2);
        exp_ptr = dq.at(4);
        CHECK(handle_error(exp_ptr));
        REQUIRE(**exp_ptr == 4);

        exp.~expected();
        dq.~fixed_deque();
        REQUIRE(counter == 0);
      }

      SECTION("Iterator") {
        CHECK(handle_error(dq.push_back(Test{0})));
        CHECK(handle_error(dq.push_back(Test{1})));
        CHECK(handle_error(dq.push_back(Test{2})));
        CHECK(handle_error(dq.push_back(Test{3})));
        CHECK(handle_error(dq.push_back(Test{4})));

        ds::i32 i = 0;
        for (auto& dc : dq) {
          REQUIRE(dc == i++);
        }

        dq.~fixed_deque();
        REQUIRE(counter == 0);
      }

      SECTION("Out of range") {
        CHECK(handle_error(dq.push_back(Test{0})));
        CHECK(handle_error(dq.push_back(Test{1})));
        CHECK(handle_error(dq.push_back(Test{2})));
        CHECK(handle_error(dq.push_back(Test{3})));
        CHECK(handle_error(dq.push_back(Test{4})));

        exp_ptr = dq.at(-1);
        REQUIRE_FALSE(exp_ptr);
        exp_ptr = dq.at(5);
        REQUIRE_FALSE(exp_ptr);

        dq.~fixed_deque();
        REQUIRE(counter == 0);
      }
    }

    SECTION("Resizing") {
      SECTION("Empty") {
        SECTION("Bigger") {
          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);

          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          dq.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Smaller") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);

          dq.~fixed_deque();
          REQUIRE(counter == 0);
        }
      }

      SECTION("Data is Shifted") {
        // data can be shifted
        SECTION("Bigger") {
          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);

          CHECK(handle_error(dq.push_back(Test{0})));
          CHECK(handle_error(dq.push_back(Test{1})));
          CHECK(handle_error(dq.push_back(Test{2})));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);

          CHECK(handle_error(dq.push_back(Test{3})));

          CHECK(handle_error(dq.resize(5)));

          REQUIRE(dq.size() == 3);
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(Test{4})));
          CHECK(handle_error(dq.push_back(Test{5})));

          for (ds::i32 i = 0; i < 5; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }

        SECTION("Smaller") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(Test{0})));
          CHECK(handle_error(dq.push_back(Test{1})));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);
          CHECK(handle_error(dq.push_back(Test{2})));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 2);

          for (ds::i32 i = 0; i < 2; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }

        SECTION("Smaller Exact") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(Test{0})));
          CHECK(handle_error(dq.push_back(Test{1})));
          CHECK(handle_error(dq.push_back(Test{2})));
          CHECK(handle_error(dq.push_back(Test{3})));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }

        SECTION("Smaller Full") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_back(Test{0})));
          CHECK(handle_error(dq.push_back(Test{1})));
          CHECK(handle_error(dq.push_back(Test{2})));
          CHECK(handle_error(dq.push_back(Test{3})));
          CHECK(handle_error(dq.push_back(Test{4})));

          exp = dq.pop_front();
          REQUIRE(*exp == 0);
          CHECK(handle_error(dq.push_back(Test{5})));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i + 1);
          }

          dq.~fixed_deque();
          exp.~expected();
          REQUIRE(counter == 0);
        }
      }

      SECTION("Data is Wrapped") {
        // data is wrapped
        // | xxT__Hxx |
        SECTION("Bigger") {
          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);

          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{0})));

          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);
          REQUIRE(dq.size() == 3);

          CHECK(handle_error(dq.push_back(Test{3})));
          CHECK(handle_error(dq.push_back(Test{4})));

          for (ds::i32 i = 0; i < 5; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }

          dq.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Smaller") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{0})));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 2);

          for (ds::i32 i = 0; i < 2; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }

          dq.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Smaller Exact") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{0})));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }

          dq.~fixed_deque();
          REQUIRE(counter == 0);
        }

        SECTION("Smaller Full") {
          CHECK(handle_error(dq.resize(5)));
          REQUIRE(dq.capacity() == 5);

          CHECK(handle_error(dq.push_front(Test{4})));
          CHECK(handle_error(dq.push_front(Test{3})));
          CHECK(handle_error(dq.push_front(Test{2})));
          CHECK(handle_error(dq.push_front(Test{1})));
          CHECK(handle_error(dq.push_front(Test{0})));

          CHECK(handle_error(dq.resize(3)));
          REQUIRE(dq.capacity() == 3);
          REQUIRE(dq.size() == 3);

          for (ds::i32 i = 0; i < 3; i++) {
            exp_ptr = dq.at(i);
            CHECK(handle_error(exp_ptr));
            REQUIRE(**exp_ptr == i);
          }

          dq.~fixed_deque();
          REQUIRE(counter == 0);
        }
      }
    }
  }
}

