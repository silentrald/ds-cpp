/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-28
 *===============================*/

#include "./main.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"

namespace ds_test {

ds::i32 counter = 0;      // NOLINT
void* free_ptr = nullptr; // NOLINT

void print_error(const ds::error& error) noexcept {
  UNSCOPED_INFO("Msg: " << error.get_msg());

  ds::error_location* location = nullptr;
  for (ds::i32 i = 0; i < error.get_size(); ++i) {
    location = error.get_location(i);
    UNSCOPED_INFO(i << " ~ " << location->file << ':' << location->line);
  }
}

bool handle_error(const ds::opt_err& error) noexcept {
  if (!error) {
    return true;
  }

  print_error(*error);
  return false;
}

// Define the test class

// === Copy ===
ds::opt_err Test::copy(const Test& other) noexcept {
  if (&other == this) {
    return ds::null;
  }

  if (!this->initialized && other.initialized) {
    ++ds_test::counter;
  }

  this->i = other.i;
  this->initialized = other.initialized;

  return ds::null;
}

// === Move ===
Test::Test(Test&& rhs) noexcept : i(rhs.i), initialized(rhs.initialized) {
  rhs.initialized = false;
}

Test& Test::operator=(Test&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  if (this->initialized) {
    --ds_test::counter;
  }

  this->i = rhs.i;
  this->initialized = rhs.initialized;
  rhs.initialized = false;

  return *this;
}

// === Destructor === //
Test::~Test() {
  if (this->initialized) {
    this->initialized = false;
    --ds_test::counter;
  }
}

// === Setter / Getter === //
void Test::set_int(ds::i32 i) noexcept {
  this->i = i;
}

ds::i32 Test::get_int() const noexcept {
  return this->i;
}

// === Checkers === //
bool Test::is_initialized() const noexcept {
  return this->initialized;
}

bool operator==(const Test& lhs, const Test& rhs) noexcept {
  return lhs.i == rhs.i;
}

bool operator==(const Test& lhs, ds::i32 rhs) noexcept {
  return lhs.i == rhs;
}

bool operator!=(const Test& lhs, const Test& rhs) noexcept {
  return lhs.i != rhs.i;
}

bool operator!=(const Test& lhs, ds::i32 rhs) noexcept {
  return lhs.i != rhs;
}

} // namespace ds_test
