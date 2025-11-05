/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#include "ds/vector.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/string.hpp"
#include "ds/types.hpp"
#include "main.hpp"
#include "test.hpp"

// === vector<i32> === //

ds::expected<ds::vector<ds::i32>, ds::error_code>
create_vector_i32(const ds::usize N, const ds::usize multiplier) noexcept {
  ds::vector<ds::i32> vector{};
  DS_TRY(vector.resize(N), ds::to_unexpected);

  for (ds::usize i = 0; i < N; ++i) {
    vector[i] = i * multiplier;
  }

  return std::move(vector);
}

TEST_CASE("vector<i32> insert elements", "[vector]") {
  ds::vector<ds::i32> vector{};

  const ds::i32 MULTIPLIER1 = 10;
  const ds::i32 MULTIPLIER2 = 20;

  for (ds::usize i = 1; i <= 32; ++i) {
    REQUIRE(ds_test::handle_error(vector.push(i * MULTIPLIER1)));
    REQUIRE(vector.get_size() == i);
  }

  for (ds::usize i = 8; i < 16; ++i) {
    REQUIRE(ds_test::handle_error(vector.insert(i, i * MULTIPLIER2)));
    REQUIRE(vector.get_size() == i + 33 - 8);
  }

  for (ds::usize i = 0; i < 8; ++i) {
    REQUIRE(vector[i] == (i + 1) * MULTIPLIER1);
  }
  for (ds::usize i = 8; i < 16; ++i) {
    REQUIRE(vector[i] == i * MULTIPLIER2);
  }
  for (ds::usize i = 16; i < 40; ++i) {
    REQUIRE(vector[i] == (i - 7) * MULTIPLIER1);
  }
}

TEST_CASE("vector<i32> removing elements", "[vector]") {
  const ds::usize N = 27U;
  const ds::isize MULTIPLIER = 10U;

  const ds::usize SAFE = 0U;
  const ds::usize UNSAFE = 1U;
  const ds::usize REMOVE = 2U;

  ds::i32 number = 0;
  ds::expected<ds::i32, ds::error_code> expected{};
  ds::expected<ds::i32*, ds::error_code> expected_pointer{};

  ds::vector<ds::i32> vector = ({
    auto expected_vector = create_vector_i32(N, MULTIPLIER);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  // Remove the center
  for (ds::isize i = N / 3, size = N; i < N * 2 / 3; ++i) {
    if (i & 1) {
      vector.remove(N / 3);
    } else {
      REQUIRE(ds_test::handle_error(vector.remove_safe(N / 3)));
    }
    REQUIRE(vector.get_size() == --size);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.at(N / 3)));
    REQUIRE(**expected_pointer == (i + 1) * MULTIPLIER);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.at(0U)));
    REQUIRE(**expected_pointer == 0);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.front_safe()));
    REQUIRE(**expected_pointer == 0);

    REQUIRE(ds_test::handle_error(
        expected_pointer = vector.at(vector.get_size() - 1U)
    ));
    REQUIRE(**expected_pointer == (N - 1) * MULTIPLIER);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.back_safe()));
    REQUIRE(**expected_pointer == (N - 1) * MULTIPLIER);
  }

  // Pop at the back
  for (ds::usize i = N * 2 / 3 - 1U, j = (N - 1) * MULTIPLIER; i >= N / 3;
       --i) {
    switch (i % 3) {
    case SAFE:
      REQUIRE(ds_test::handle_error(expected = vector.pop_safe()));
      number = *expected;
      break;

    case UNSAFE:
      number = vector.pop();
      break;

    case REMOVE:
      number = vector[vector.get_size() - 1U];
      vector.remove(vector.get_size() - 1U);
      break;
    }
    REQUIRE(vector.get_size() == i);
    REQUIRE(number == j);
    j -= MULTIPLIER;

    REQUIRE(vector[0U] == 0);
  }

  // Pop at the front
  for (ds::usize i = 0U, size = N / 3; i < N / 3; ++i) {
    vector.remove(0U);
    REQUIRE(vector.get_size() == --size);
    if (vector.get_size() != 0U) {
      REQUIRE(vector[0U] == (i + 1) * MULTIPLIER);
    }
  }

  REQUIRE(vector.is_empty());
}

TEST_CASE("vector<i32> forward iterators", "[vector]") {
  const ds::usize N = 40U;
  const ds::usize MULTIPLIER = 10U;

  ds::vector<ds::i32> vector = ({
    auto expected_vector = create_vector_i32(N, MULTIPLIER);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  ds::usize expected = 0U;
  for (auto actual : vector) {
    REQUIRE(actual == expected);
    expected += MULTIPLIER;
  }

  expected = 0U;
  for (const auto actual : vector) {
    REQUIRE(actual == expected);
    expected += MULTIPLIER;
  }

  expected = 0U;
  for (auto it = vector.begin(); it != vector.end(); ++it) {
    REQUIRE(*it == expected);
    expected += MULTIPLIER;
  }

  expected = 0U;
  for (auto it = vector.cbegin(); it != vector.cend(); ++it) {
    REQUIRE(*it == expected);
    expected += MULTIPLIER;
  }
}

TEST_CASE("vector<i32> reverse iterators", "[vector]") {
  const ds::usize N = 40U;
  const ds::usize MULTIPLIER = 10U;

  ds::vector<ds::i32> vector = ({
    auto expected_vector = create_vector_i32(N, MULTIPLIER);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  ds::usize expected = (N - 1) * MULTIPLIER;
  for (auto it = vector.rbegin(); it != vector.rend(); --it) {
    REQUIRE(*it == expected);
    expected -= MULTIPLIER;
  }

  expected = (N - 1) * MULTIPLIER;
  for (auto it = vector.crbegin(); it != vector.crend(); --it) {
    REQUIRE(*it == expected);
    expected -= MULTIPLIER;
  }
}

// === vector<Test> === //

ds::expected<ds::vector<ds_test::Test>, ds::error_code> create_vector_test(
    const ds::isize N, const ds::isize multiplier, ds::isize* pointer
) noexcept {
  ds::vector<ds_test::Test> vector{};
  DS_TRY(vector.resize(N), ds::to_unexpected);

  for (ds::isize i = 0; i < N; ++i) {
    vector[i] = ds_test::Test{pointer, i * multiplier};
  }

  return std::move(vector);
}

// NOTE: 1 for 1 with vector<i32>

TEST_CASE("vector<Test> insert elements", "[vector]") {
  ds::vector<ds_test::Test> vector{};
  ds::isize counter = 0;

  const ds::isize MULTIPLIER1 = 10;
  const ds::isize MULTIPLIER2 = 20;

  for (ds::isize i = 1; i <= 32; ++i) {
    REQUIRE(ds_test::handle_error(
        vector.push(ds_test::Test{&counter, i * MULTIPLIER1})
    ));
    REQUIRE(vector.get_size() == i);
    REQUIRE(counter == i);
  }

  for (ds::isize i = 8; i < 16; ++i) {
    REQUIRE(ds_test::handle_error(
        vector.insert(i, ds_test::Test{&counter, i * MULTIPLIER2})
    ));
    REQUIRE(vector.get_size() == i + 33 - 8);
  }

  for (ds::usize i = 0; i < 8; ++i) {
    REQUIRE(vector[i] == (i + 1) * MULTIPLIER1);
  }
  for (ds::usize i = 8; i < 16; ++i) {
    REQUIRE(vector[i] == i * MULTIPLIER2);
  }
  for (ds::usize i = 16; i < 40; ++i) {
    REQUIRE(vector[i] == (i - 7) * MULTIPLIER1);
  }
}

TEST_CASE("vector<Test> removing elements", "[vector]") {
  const ds::isize N = 27;
  const ds::isize MULTIPLIER = 10;

  const ds::usize SAFE = 0U;
  const ds::usize UNSAFE = 1U;
  const ds::usize REMOVE = 2U;

  ds_test::Test test{};
  ds::isize counter = 0;
  ds::expected<ds_test::Test, ds::error_code> expected{};
  ds::expected<ds_test::Test*, ds::error_code> expected_pointer{};

  ds::vector<ds_test::Test> vector = ({
    auto expected_vector = create_vector_test(N, MULTIPLIER, &counter);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  // Remove the center
  for (ds::isize i = N / 3, size = N; i < N * 2 / 3; ++i) {
    if (i & 1) {
      vector.remove(N / 3);
    } else {
      REQUIRE(ds_test::handle_error(vector.remove_safe(N / 3)));
    }
    REQUIRE(vector.get_size() == --size);
    REQUIRE(counter == size);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.at(N / 3)));
    REQUIRE(**expected_pointer == (i + 1) * MULTIPLIER);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.at(0U)));
    REQUIRE(**expected_pointer == 0);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.front_safe()));
    REQUIRE(**expected_pointer == 0);

    REQUIRE(ds_test::handle_error(
        expected_pointer = vector.at(vector.get_size() - 1U)
    ));
    REQUIRE(**expected_pointer == (N - 1) * MULTIPLIER);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.back_safe()));
    REQUIRE(**expected_pointer == (N - 1) * MULTIPLIER);
  }

  // Pop at the back
  for (ds::usize i = N * 2 / 3 - 1U, j = (N - 1) * MULTIPLIER; i >= N / 3;
       --i) {
    switch (i % 3) {
    case SAFE:
      REQUIRE(ds_test::handle_error(expected = vector.pop_safe()));
      test = std::move(*expected);
      break;

    case UNSAFE:
      test = vector.pop();
      break;

    case REMOVE:
      test = std::move(vector[vector.get_size() - 1U]);
      vector.remove(vector.get_size() - 1U);
      break;
    }
    REQUIRE(vector.get_size() == i);
    REQUIRE(test == j);
    j -= MULTIPLIER;

    REQUIRE(vector[0U] == 0);
  }

  // Pop at the front
  for (ds::usize i = 0U, size = N / 3; i < N / 3; ++i) {
    vector.remove(0U);
    REQUIRE(vector.get_size() == --size);
    if (vector.get_size() != 0U) {
      REQUIRE(vector[0U] == (i + 1) * MULTIPLIER);
    }
  }

  REQUIRE(vector.is_empty());

  test.destroy();
  vector.destroy();

  REQUIRE(counter == 0);
}

TEST_CASE("vector<Test> forward iterators", "[vector]") {
  const ds::isize N = 40U;
  const ds::isize MULTIPLIER = 10U;

  ds::isize counter = 0;
  ds::vector<ds_test::Test> vector = ({
    auto expected_vector = create_vector_test(N, MULTIPLIER, &counter);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  ds::isize expected = 0U;
  for (auto it = vector.begin(); it != vector.end(); ++it) {
    REQUIRE(*it == expected);
    expected += MULTIPLIER;
  }

  expected = 0U;
  for (auto it = vector.cbegin(); it != vector.cend(); ++it) {
    REQUIRE(*it == expected);
    expected += MULTIPLIER;
  }
  REQUIRE(counter == vector.get_size());
}

TEST_CASE("vector<Test> reverse iterators", "[vector]") {
  const ds::isize N = 40U;
  const ds::isize MULTIPLIER = 10U;

  ds::isize counter = 0;
  ds::vector<ds_test::Test> vector = ({
    auto expected_vector = create_vector_test(N, MULTIPLIER, &counter);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  ds::isize expected = (N - 1) * MULTIPLIER;
  for (auto it = vector.rbegin(); it != vector.rend(); --it) {
    REQUIRE(*it == expected);
    expected -= MULTIPLIER;
  }

  expected = (N - 1) * MULTIPLIER;
  for (auto it = vector.crbegin(); it != vector.crend(); --it) {
    REQUIRE(*it == expected);
    expected -= MULTIPLIER;
  }
}

// === vector<string> === //

ds::expected<ds::vector<ds::string>, ds::error_code>
create_vector_string(const ds::c8** words, ds::usize size) noexcept {
  ds::vector<ds::string> vector{};
  DS_TRY(vector.resize(size), ds::to_unexpected);

  for (ds::usize i = 0; i < size; ++i) {
    DS_TRY(vector[i].copy(words[i]), ds::to_unexpected);
  }

  return std::move(vector);
}

// NOLINTNEXTLINE
const ds::c8* WORDS[] = {"liability", "ignite",   "arrow",  "fight",
                         "excuse",    "fibre",    "scheme", "cage",
                         "soprano",   "dealer",   "rabbit", "mask",
                         "plaintiff", "specimen", "stem",   "castle",
                         "point",     "dragon"};
const ds::usize WORDS_SIZE = 18U;

TEST_CASE("vector<string> insert elements", "[vector]") {
  ds::vector<ds::string> vector{};
  ds::string string{};
  ds::expected<ds::string*, ds::error_code> expected{};

  const ds::usize CONST_CHAR = 0;
  const ds::usize COPY = 1;

  for (ds::usize i = 0; i < WORDS_SIZE / 2; ++i) {
    switch (i % 3) {
    case CONST_CHAR:
      REQUIRE(ds_test::handle_error(vector.push(WORDS[i])));
      break;

    case COPY:
      REQUIRE(ds_test::handle_error(string.copy(WORDS[i])));
      REQUIRE(ds_test::handle_error(vector.push(string)));
      break;

    default: // MOVE
      REQUIRE(ds_test::handle_error(string.copy(WORDS[i])));
      REQUIRE(ds_test::handle_error(vector.push(std::move(string))));
      break;
    }
    REQUIRE(vector.get_size() == i + 1U);
  }

  for (ds::usize i = WORDS_SIZE / 2; i < WORDS_SIZE; ++i) {
    switch (i % 3) {
    case CONST_CHAR:
      REQUIRE(ds_test::handle_error(vector.insert(WORDS_SIZE / 3U, WORDS[i])));
      break;

    case COPY:
      REQUIRE(ds_test::handle_error(string.copy(WORDS[i])));
      REQUIRE(ds_test::handle_error(vector.insert(WORDS_SIZE / 3U, string)));
      break;

    default: // MOVE
      REQUIRE(ds_test::handle_error(string.copy(WORDS[i])));
      REQUIRE(ds_test::handle_error(
          vector.insert(WORDS_SIZE / 3U, std::move(string))
      ));
      break;
    }
    REQUIRE(vector.get_size() == i + 1U);
  }

  for (ds::usize i = 0; i < WORDS_SIZE / 3; ++i) {
    REQUIRE(ds_test::handle_error(expected = vector.at(i)));
    REQUIRE(**expected == WORDS[i]);
  }
  for (ds::usize i = 0; i < WORDS_SIZE / 2; ++i) {
    REQUIRE(vector[i + WORDS_SIZE / 3U] == WORDS[WORDS_SIZE - i - 1U]);
  }
  for (ds::usize i = 0; i < WORDS_SIZE / 6; ++i) {
    REQUIRE(vector[i + WORDS_SIZE * 5 / 6] == WORDS[i + WORDS_SIZE / 3U]);
  }
}

TEST_CASE("vector<string> removing elements", "[vector]") {
  ds::string string{};
  ds::expected<ds::string, ds::error_code> expected{};
  ds::expected<ds::string*, ds::error_code> expected_pointer{};

  const ds::usize SAFE = 0U;
  const ds::usize UNSAFE = 1U;
  const ds::usize REMOVE = 2U;

  ds::vector<ds::string> vector = ({
    auto expected_vector = create_vector_string(WORDS, WORDS_SIZE);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  // Remove the center
  for (ds::usize i = WORDS_SIZE / 3, size = WORDS_SIZE; i < WORDS_SIZE * 2 / 3;
       ++i) {
    if (i & 1) {
      vector.remove(WORDS_SIZE / 3);
    } else {
      REQUIRE(ds_test::handle_error(vector.remove_safe(WORDS_SIZE / 3)));
    }
    REQUIRE(vector.get_size() == --size);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.at(WORDS_SIZE / 3))
    );
    REQUIRE(**expected_pointer == WORDS[i + 1U]);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.at(0U)));
    REQUIRE(**expected_pointer == WORDS[0U]);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.front_safe()));
    REQUIRE(**expected_pointer == WORDS[0U]);

    REQUIRE(ds_test::handle_error(
        expected_pointer = vector.at(vector.get_size() - 1U)
    ));
    REQUIRE(**expected_pointer == WORDS[WORDS_SIZE - 1U]);

    REQUIRE(ds_test::handle_error(expected_pointer = vector.back_safe()));
    REQUIRE(**expected_pointer == WORDS[WORDS_SIZE - 1U]);
  }

  // Pop at the back
  for (ds::usize i = WORDS_SIZE * 2 / 3 - 1U, j = WORDS_SIZE;
       i >= WORDS_SIZE / 3; --i) {
    switch (i % 3) {
    case SAFE:
      REQUIRE(ds_test::handle_error(expected = vector.pop_safe()));
      string = std::move(*expected);
      break;

    case UNSAFE:
      string = std::move(vector.pop());
      break;

    case REMOVE:
      string = std::move(vector[vector.get_size() - 1U]);
      vector.remove(vector.get_size() - 1U);
      break;
    }
    REQUIRE(vector.get_size() == i);
    REQUIRE(string == WORDS[--j]);

    REQUIRE(vector[0U] == WORDS[0U]);
  }

  // Pop at the front
  for (ds::usize i = 0U, size = WORDS_SIZE / 3; i < WORDS_SIZE / 3; ++i) {
    vector.remove(0U);
    REQUIRE(vector.get_size() == --size);
    if (vector.get_size() != 0U) {
      REQUIRE(vector[0U] == WORDS[i + 1U]);
    }
  }

  REQUIRE(vector.is_empty());
}

TEST_CASE("vector<string> forward iterators", "[vector]") {
  ds::vector<ds::string> vector = ({
    auto expected_vector = create_vector_string(WORDS, WORDS_SIZE);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  ds::usize index = 0U;
  for (auto it = vector.begin(); it != vector.end(); ++it) {
    REQUIRE(*it == WORDS[index++]);
  }

  index = 0U;
  for (auto it = vector.cbegin(); it != vector.cend(); ++it) {
    REQUIRE(*it == WORDS[index++]);
  }
}

TEST_CASE("vector<string> reverse iterators", "[vector]") {
  ds::vector<ds::string> vector = ({
    auto expected_vector = create_vector_string(WORDS, WORDS_SIZE);
    REQUIRE(ds_test::handle_error(expected_vector));
    std::move(*expected_vector);
  });

  ds::usize index = WORDS_SIZE;
  for (auto it = vector.rbegin(); it != vector.rend(); --it) {
    REQUIRE(*it == WORDS[--index]);
  }

  index = WORDS_SIZE;
  for (auto it = vector.crbegin(); it != vector.crend(); --it) {
    REQUIRE(*it == WORDS[--index]);
  }
}
