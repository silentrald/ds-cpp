/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#include "ds/vector.hpp"
#include "./main.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds-error/types.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"

using namespace ds_test;

// NOLINTNEXTLINE
TEST_CASE("vector", "ds") {
  SECTION("Primitive") {
    ds::vector<ds::i32> vec{};
    ds::exp_err<ds::i32> exp{};
    ds::exp_ptr_err<ds::i32> exp_ptr{};

    SECTION("Empty initialization") {
      REQUIRE(vec.is_empty());
      REQUIRE(vec.size() == 0); // NOLINT
    }

    SECTION("Single element initialization") {
      CHECK(handle_error(vec.push_back(1)));

      REQUIRE(!vec.is_empty());
      REQUIRE(vec.size() == 1);
      REQUIRE(vec[0] == 1);
    }

    SECTION("Multi element initialization") {
      CHECK(handle_error(vec.push_back(1, 2, 3)));

      REQUIRE(!vec.is_empty());
      REQUIRE(vec.size() == 3);
      REQUIRE(vec[0] == 1);
      REQUIRE(vec[1] == 2);
      REQUIRE(vec[2] == 3);
    }

    SECTION("Passing") {
      free_ptr = nullptr;

      SECTION("Init Move (empty move)") {
        ds::vector<ds::i32> vec2 = std::move(vec);

        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());
      }

      SECTION("Init Move (non-empty move)") {
        CHECK(handle_error(vec.push_back(1, 2, 3)));

        ds::i32* ptr = vec.data();
        ds::vector<ds::i32> vec2 = std::move(vec);

        REQUIRE(vec != vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec2.size() == 3);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(ptr == vec2.data());
      }

      SECTION("Copy (empty -> empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());
      }

      SECTION("Copy (non-empty -> empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec.push_back(1, 2, 3)));
        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
      }

      SECTION("Copy (empty -> non-empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec2.push_back(1, 2, 3)));

        ds::i32* ptr = vec2.data();

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());
      }

      SECTION("Copy (smaller non-empty -> bigger non-empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec.push_back(1, 2, 3)));
        CHECK(handle_error(vec2.push_back(4, 5, 6, 7, 8)));

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(vec.max_size() == 3);
        REQUIRE(vec2.max_size() == 5);
      }

      SECTION("Copy (bigger non-empty -> smaller non-empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec.push_back(1, 2, 3, 4, 5)));
        CHECK(handle_error(vec2.push_back(6, 7, 8)));

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(vec2[3] == 4);
        REQUIRE(vec2[4] == 5);
        REQUIRE(vec.max_size() == 5);
        REQUIRE(vec2.max_size() == 5);
      }

      SECTION("Move (empty -> empty)") {
        ds::vector<ds::i32> vec2;

        vec2 = std::move(vec);
        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());
      }

      SECTION("Move (non-empty -> empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec.push_back(1, 2, 3)));

        ds::i32* ptr = vec.data();
        vec2 = std::move(vec);

        REQUIRE(vec != vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(ptr == vec2.data());
      }

      SECTION("Move (empty -> non-empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec2.push_back(4, 5, 6)));

        ds::i32* ptr = vec2.data();
        vec2 = std::move(vec);

        REQUIRE(vec == vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec.data() == nullptr);
        REQUIRE(ptr == free_ptr);
      }

      SECTION("Move (non-empty -> non-empty)") {
        ds::vector<ds::i32> vec2;

        CHECK(handle_error(vec.push_back(1, 2, 3)));
        CHECK(handle_error(vec2.push_back(4, 5, 6)));

        ds::i32* ptr = vec.data();
        ds::i32* ptr2 = vec2.data();
        vec2 = std::move(vec);

        REQUIRE(vec != vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(ptr == vec2.data());
        REQUIRE(ptr2 == free_ptr);
      }
    }

    SECTION("Element accessing") {

      SECTION("at") {
        SECTION("Empty") {
          exp_ptr = vec.at(0);
          REQUIRE_FALSE(exp_ptr);
        }

        SECTION("Non-empty") {
          CHECK(handle_error(vec.push_back(1, 2, 3, 4, 5)));

          exp_ptr = vec.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 1);

          exp_ptr = vec.at(2);
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 3);

          exp_ptr = vec.at(4);
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 5);

          // Out of bounds
          exp_ptr = vec.at(5);
          REQUIRE_FALSE(exp_ptr);
        }
      }

      SECTION("front/back") {
        SECTION("Empty") {
          exp_ptr = vec.front();
          REQUIRE_FALSE(exp_ptr);

          exp_ptr = vec.back();
          REQUIRE_FALSE(exp_ptr);
        }

        SECTION("Non-empty") {
          CHECK(handle_error(vec.push_back(6, 7, 8, 9, 10)));

          exp_ptr = vec.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 6);

          exp_ptr = vec.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 10);
        }
      }
    }

    SECTION("Modifiers") {
      SECTION("Push back") {
        CHECK(handle_error(vec.push_back(1)));
        REQUIRE(vec[0] == 1);

        CHECK(handle_error(vec.push_back(2, 3, 4)));
        REQUIRE(vec[1] == 2);
        REQUIRE(vec[2] == 3);
        REQUIRE(vec[3] == 4);
      }

      SECTION("Inserting") {
        CHECK(handle_error(vec.insert(vec.begin(), 1)));
        REQUIRE(vec[0] == 1);

        CHECK(handle_error(vec.insert(vec.begin(), 2)));
        REQUIRE(vec[0] == 2);
        REQUIRE(vec[1] == 1);

        CHECK(handle_error(vec.insert(vec.begin() + 1, 3)));
        REQUIRE(vec[1] == 3);
        REQUIRE(vec[2] == 1);

        CHECK(handle_error(vec.insert(vec.begin() + 2, 4, 5, 6)));
        REQUIRE(vec[2] == 4);
        REQUIRE(vec[3] == 5);
        REQUIRE(vec[4] == 6);
        REQUIRE(vec[5] == 1);
      }

      SECTION("Clearing") {
        CHECK(handle_error(vec.push_back(1, 2, 3, 4, 5)));
        REQUIRE_FALSE(vec.is_empty());

        vec.clear();
        REQUIRE(vec.is_empty());
      }
    }

    SECTION("Iterator") {
      for (auto i : vec) {
        CHECK(false);
      }

      CHECK(handle_error(vec.push_back(1)));
      for (auto i : vec) {
        REQUIRE(i == 1);
      }

      CHECK(handle_error(vec.push_back(2, 3, 4, 5)));
      ds::i32 ei = 0;
      for (auto i : vec) {
        REQUIRE(i == ++ei);
      }

      ei = 0;
      for (const auto i : vec) {
        REQUIRE(i == ++ei);
      }
    }

    SECTION("Reserving") {
      CHECK(handle_error(vec.reserve(10)));
      REQUIRE(vec.size() == 0);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.reserve(5)));
      REQUIRE(vec.size() == 0);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.reserve(15)));
      REQUIRE(vec.size() == 0);
      REQUIRE(vec.max_size() == 15);
    }

    SECTION("Resizing") {
      CHECK(handle_error(vec.resize(10)));
      REQUIRE(vec.size() == 10);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.resize(5)));
      REQUIRE(vec.size() == 5);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.resize(15)));
      REQUIRE(vec.size() == 15);
      REQUIRE(vec.max_size() == 15);
    }

    SECTION("Erasing") {
      CHECK(handle_error(vec.push_back(1, 2, 3, 4, 5)));

      vec.erase(vec.begin() + 2);
      REQUIRE(vec.size() == 4);
      REQUIRE(vec[0] == 1);
      REQUIRE(vec[1] == 2);
      REQUIRE(vec[2] == 4);
      REQUIRE(vec[3] == 5);

      vec.erase(vec.begin() + 3);
      REQUIRE(vec.size() == 3);
      REQUIRE(vec[0] == 1);
      REQUIRE(vec[1] == 2);
      REQUIRE(vec[2] == 4);
    }
  }

  SECTION("Custom class") {
    ds::vector<Test> vec{};
    ds::exp_err<Test> exp{};
    ds::exp_ptr_err<Test> exp_ptr{};
    counter = 0;

    SECTION("Empty initialization") {
      REQUIRE(vec.is_empty());
      REQUIRE(vec.size() == 0);

      vec.~vector();
      REQUIRE(counter == 0);
    }

    SECTION("Single element initialization") {
      CHECK(handle_error(vec.push_back(Test{1})));

      REQUIRE(!vec.is_empty());
      REQUIRE(vec.size() == 1);
      REQUIRE(vec[0] == 1);

      vec.~vector();
      REQUIRE(counter == 0);
    }

    SECTION("Multi element initialization") {
      CHECK(handle_error(vec.push_back(Test{1}, Test{2}, Test{3})));

      REQUIRE(!vec.is_empty());
      REQUIRE(vec.size() == 3);
      REQUIRE(vec[0] == 1);
      REQUIRE(vec[1] == 2);
      REQUIRE(vec[2] == 3);

      vec.~vector();
      REQUIRE(counter == 0);
    }

    SECTION("Passing") {
      free_ptr = nullptr;
      SECTION("Init Move (empty move)") {
        ds::vector<Test> vec2 = std::move(vec);

        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Init Move (non-empty move)") {
        CHECK(handle_error(vec.push_back(Test{1}, Test{2}, Test{3})));
        Test* ptr = vec.data();
        ds::vector<Test> vec2 = std::move(vec);

        REQUIRE(vec != vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec2.size() == 3);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(ptr == vec2.data());

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Copy (empty -> empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());
      }

      SECTION("Copy (non-empty -> empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(vec.push_back(Test{1}, Test{2}, Test{3})));

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Copy (empty -> non-empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(vec2.push_back(Test{1}, Test{2}, Test{3})));

        Test* ptr = vec2.data();

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Copy (smaller non-empty -> bigger non-empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(vec.push_back(Test{1}, Test{2}, Test{3})));
        CHECK(handle_error(
            vec2.push_back(Test{4}, Test{5}, Test{6}, Test{7}, Test{8})
        ));

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(vec.max_size() == 3);
        REQUIRE(vec2.max_size() == 5);

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Copy (bigger non-empty -> smaller non-empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(
            vec.push_back(Test{1}, Test{2}, Test{3}, Test{4}, Test{5})
        ));
        CHECK(handle_error(vec2.push_back(Test{6}, Test{7}, Test{8})));

        CHECK(handle_error(vec2.copy(vec)));

        REQUIRE(vec == vec2);
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(vec2[3] == 4);
        REQUIRE(vec2[4] == 5);
        REQUIRE(vec.max_size() == 5);
        REQUIRE(vec2.max_size() == 5);

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Move (empty -> empty)") {
        ds::vector<Test> vec2;

        vec2 = std::move(vec);
        REQUIRE(vec == vec2);
        REQUIRE(vec2.is_empty());

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Move (non-empty -> empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(vec.push_back(Test{1}, Test{2}, Test{3})));

        Test* ptr = vec.data();
        vec2 = std::move(vec);

        REQUIRE(vec != vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(ptr == vec2.data());

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Move (empty -> non-empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(vec2.push_back(Test{4}, Test{5}, Test{6})));

        Test* ptr = vec2.data();
        vec2 = std::move(vec);

        REQUIRE(vec == vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec.data() == nullptr);
        REQUIRE(ptr == free_ptr);

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Move (non-empty -> non-empty)") {
        ds::vector<Test> vec2;

        CHECK(handle_error(vec.push_back(Test{1}, Test{2}, Test{3})));
        CHECK(handle_error(vec2.push_back(Test{4}, Test{5}, Test{6})));

        Test* ptr = vec.data();
        Test* ptr2 = vec2.data();
        vec2 = std::move(vec);

        REQUIRE(vec != vec2);
        REQUIRE(vec.is_empty());
        REQUIRE(vec2[0] == 1);
        REQUIRE(vec2[1] == 2);
        REQUIRE(vec2[2] == 3);
        REQUIRE(ptr == vec2.data());
        REQUIRE(ptr2 == free_ptr);

        vec.~vector();
        vec2.~vector();
        REQUIRE(counter == 0);
      }
    }

    SECTION("Element accessing") {
      SECTION("at") {
        SECTION("Empty") {
          exp_ptr = vec.at(0);
          REQUIRE_FALSE(exp_ptr);

          vec.~vector();
          REQUIRE(counter == 0);
        }

        SECTION("Non-empty") {
          CHECK(handle_error(
              vec.push_back(Test{1}, Test{2}, Test{3}, Test{4}, Test{5})
          ));

          exp_ptr = vec.at(0);
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 1);

          exp_ptr = vec.at(2);
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 3);

          exp_ptr = vec.at(4);
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 5);

          // Out of bounds
          exp_ptr = vec.at(5);
          REQUIRE_FALSE(exp_ptr);

          exp.~expected();
          vec.~vector();
          REQUIRE(counter == 0);
        }
      }

      SECTION("front/back") {
        SECTION("Empty") {
          exp_ptr = vec.front();
          REQUIRE_FALSE(exp_ptr);

          exp_ptr = vec.back();
          REQUIRE_FALSE(exp_ptr);

          vec.~vector();
          REQUIRE(counter == 0);
        }

        SECTION("Non-empty") {
          CHECK(handle_error(
              vec.push_back(Test{6}, Test{7}, Test{8}, Test{9}, Test{10})
          ));

          exp_ptr = vec.front();
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 6);

          exp_ptr = vec.back();
          CHECK(handle_error(exp_ptr));
          REQUIRE_FALSE(exp_ptr.is_null());
          REQUIRE(*exp_ptr == 10);

          exp.~expected();
          exp_ptr.~expected_ptr();
          vec.~vector();
          REQUIRE(counter == 0);
        }
      }
    }

    SECTION("Modifiers") {
      SECTION("Push back") {
        CHECK(handle_error(vec.push_back(Test{1})));
        REQUIRE(vec[0] == 1);

        CHECK(handle_error(vec.push_back(Test{2}, Test{3}, Test{4})));
        REQUIRE(vec[1] == 2);
        REQUIRE(vec[2] == 3);
        REQUIRE(vec[3] == 4);

        vec.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Inserting") {
        CHECK(handle_error(vec.insert(vec.begin(), Test{1})));
        REQUIRE(vec[0] == 1);

        CHECK(handle_error(vec.insert(vec.begin(), Test{2})));
        REQUIRE(vec[0] == 2);
        REQUIRE(vec[1] == 1);

        CHECK(handle_error(vec.insert(vec.begin() + 1, Test{3})));
        REQUIRE(vec[1] == 3);
        REQUIRE(vec[2] == 1);

        CHECK(
            handle_error(vec.insert(vec.begin() + 2, Test{4}, Test{5}, Test{6}))
        );
        REQUIRE(vec[2] == 4);
        REQUIRE(vec[3] == 5);
        REQUIRE(vec[4] == 6);
        REQUIRE(vec[5] == 1);

        vec.~vector();
        REQUIRE(counter == 0);
      }

      SECTION("Clearing") {
        CHECK(handle_error(
            vec.push_back(Test{1}, Test{2}, Test{3}, Test{4}, Test{5})
        ));
        REQUIRE_FALSE(vec.is_empty());

        vec.clear();
        REQUIRE(vec.is_empty());

        vec.~vector();
        REQUIRE(counter == 0);
      }
    }

    SECTION("Iterator") {
      for (auto& c : vec) {
        CHECK(false);
      }

      CHECK(handle_error(vec.push_back(Test{1})));
      for (auto& c : vec) {
        REQUIRE(c == 1);
      }

      CHECK(handle_error(vec.push_back(Test{2}, Test{3}, Test{4}, Test{5})));
      ds::i32 ei = 0;
      for (auto& c : vec) {
        REQUIRE(c == ++ei);
      }

      ei = 0;
      for (const auto& c : vec) {
        REQUIRE(c == ++ei);
      }

      vec.~vector();
      REQUIRE(counter == 0);
    }

    SECTION("Reserving") {
      CHECK(handle_error(vec.reserve(10)));
      REQUIRE(vec.size() == 0);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.reserve(5)));
      REQUIRE(vec.size() == 0);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.reserve(15)));
      REQUIRE(vec.size() == 0);
      REQUIRE(vec.max_size() == 15);

      vec.~vector();
      REQUIRE(counter == 0);
    }

    SECTION("Resizing") {
      CHECK(handle_error(vec.resize(10)));
      REQUIRE(vec.size() == 10);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.resize(5)));
      REQUIRE(vec.size() == 5);
      REQUIRE(vec.max_size() == 10);

      CHECK(handle_error(vec.resize(15)));
      REQUIRE(vec.size() == 15);
      REQUIRE(vec.max_size() == 15);

      vec.~vector();
      REQUIRE(counter == 0);
    }

    SECTION("Erasing") {
      CHECK(handle_error(
          vec.push_back(Test{1}, Test{2}, Test{3}, Test{4}, Test{5})
      ));

      vec.erase(vec.begin() + 2);
      REQUIRE(vec.size() == 4);
      REQUIRE(vec[0] == 1);
      REQUIRE(vec[1] == 2);
      REQUIRE(vec[2] == 4);
      REQUIRE(vec[3] == 5);

      vec.erase(vec.begin() + 3);
      REQUIRE(vec.size() == 3);
      REQUIRE(vec[0] == 1);
      REQUIRE(vec[1] == 2);
      REQUIRE(vec[2] == 4);

      vec.~vector();
      REQUIRE(counter == 0);
    }
  }

  SECTION("Specialized functions on String class") {
    ds::vector<ds::string> vec{};
    ds::exp_err<ds::string> exp{};
    ds::exp_ptr_err<ds::string> exp_ptr{};

    SECTION("Init") {
      CHECK(handle_error(vec.push_back("Hi", "world", "my", "name", "jeff")));

      REQUIRE(vec[0] == "Hi");
      REQUIRE(vec[2] == "my");
      REQUIRE(vec[4] == "jeff");
    }

    SECTION("Push back") {
      CHECK(handle_error(vec.push_back("Hi")));
      REQUIRE(vec[0] == "Hi");

      CHECK(handle_error(vec.push_back("my", "name", "jeff")));
      REQUIRE(vec[0] == "Hi");
      REQUIRE(vec[1] == "my");
      REQUIRE(vec[3] == "jeff");
    }

    SECTION("Insert") {
      CHECK(handle_error(vec.insert(vec.begin(), "Hello")));
      REQUIRE(vec[0] == "Hello");

      CHECK(handle_error(vec.insert(vec.begin() + 1, "jeff")));
      REQUIRE(vec[0] == "Hello");
      REQUIRE(vec[1] == "jeff");

      CHECK(handle_error(vec.insert(vec.begin(), "***")));
      REQUIRE(vec[0] == "***");
      REQUIRE(vec[1] == "Hello");
      REQUIRE(vec[2] == "jeff");

      CHECK(handle_error(vec.insert(vec.begin() + 2, "everybody", "my", "name"))
      );
      REQUIRE(vec[0] == "***");
      REQUIRE(vec[1] == "Hello");
      REQUIRE(vec[2] == "everybody");
      REQUIRE(vec[3] == "my");
      REQUIRE(vec[4] == "name");
      REQUIRE(vec[5] == "jeff");
    }

    SECTION("Mixed") {
      ds::string str_copy;
      ds::string str_move;
      CHECK(handle_error(str_copy.copy("uwu")));
      CHECK(handle_error(str_move.copy("owo")));

      SECTION("Init (move last)") {
        CHECK(handle_error(vec.push_back("hello", str_copy, std::move(str_move))
        ));
        REQUIRE(vec[0] == "hello");
        REQUIRE(vec[1] == str_copy);
        REQUIRE(vec[2] == "owo");
        REQUIRE(str_move.is_empty());
      }

      SECTION("Init (copy last)") {
        CHECK(handle_error(vec.push_back("hello", std::move(str_move), str_copy)
        ));
        REQUIRE(vec[0] == "hello");
        REQUIRE(vec[1] == "owo");
        REQUIRE(vec[2] == str_copy);
        REQUIRE(str_move.is_empty());

        // NOLINTNEXTLINE
        const char* exp_strings[] = {"hello", "owo", "uwu", "<>", "<>"};
        ds::i32 i = 0;
        for (auto& s : vec) {
          // NOLINTNEXTLINE
          REQUIRE(s == exp_strings[i++]);
        }
      }

      SECTION("Push back (move last)") {
        CHECK(handle_error(vec.push_back("hi", str_copy, std::move(str_move))));
        REQUIRE(vec[0] == "hi");
        REQUIRE(vec[1] == str_copy);
        REQUIRE(vec[2] == "owo");
        REQUIRE(str_move.is_empty());

        // NOLINTNEXTLINE
        const char* exp_strings[] = {"hi", "uwu", "owo", "<>", "<>"};
        ds::i32 i = 0;
        for (auto& s : vec) {
          REQUIRE(s == exp_strings[i++]);
        }
      }

      SECTION("Push back (copy last)") {
        CHECK(handle_error(vec.push_back("hi", std::move(str_move), str_copy)));
        REQUIRE(vec[0] == "hi");
        REQUIRE(vec[1] == "owo");
        REQUIRE(vec[2] == str_copy);
        REQUIRE(str_move.is_empty());

        // NOLINTNEXTLINE
        const char* exp_strings[] = {"hi", "owo", "uwu", "<>", "<>"};
        ds::i32 i = 0;
        for (auto& s : vec) {
          REQUIRE(s == exp_strings[i++]);
        }
      }

      SECTION("Insert (move last)") {
        CHECK(handle_error(vec.push_back("some", "values")));

        CHECK(handle_error(
            vec.insert(vec.begin() + 1, ":^)", str_copy, std::move(str_move))
        ));
        REQUIRE(vec[1] == ":^)");
        REQUIRE(vec[2] == str_copy);
        REQUIRE(vec[3] == "owo");
        REQUIRE(str_move.is_empty());

        // NOLINTNEXTLINE
        const char* exp_strings[] = {"some",   ":^)", "uwu", "owo",
                                     "values", "<>",  "<>"};
        ds::i32 i = 0;
        for (auto& s : vec) {
          REQUIRE(s == exp_strings[i++]);
        }
      }

      SECTION("Insert (copy last)") {
        CHECK(handle_error(vec.push_back("some", "values")));

        CHECK(handle_error(
            vec.insert(vec.begin() + 1, ":^)", std::move(str_move), str_copy)
        ));
        REQUIRE(vec[1] == ":^)");
        REQUIRE(vec[2] == "owo");
        REQUIRE(vec[3] == str_copy);
        REQUIRE(str_move.is_empty());

        // NOLINTNEXTLINE
        const char* exp_strings[] = {"some",   ":^)", "owo", "uwu",
                                     "values", "<>",  "<>"};
        ds::i32 i = 0;
        for (auto& s : vec) {
          REQUIRE(s == exp_strings[i++]);
        }
      }
    }
  }
}
