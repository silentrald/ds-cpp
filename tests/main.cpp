/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-28
 *===============================*/

#include "./main.hpp"
#include "catch2/catch_message.hpp"
#include <cassert>

// Define the test class
bool ds_test::handle_error(ds::error_code error) noexcept {
  if (!error) {
    return true;
  }

  UNSCOPED_INFO("Got error_code: " << error);
  return false;
}

namespace ds_test {

// === Initializers === //

Test::Test(ds::isize* pointer) noexcept : pointer(pointer) {
  assert(pointer != nullptr);
  ++*pointer;
}

Test::Test(ds::isize* pointer, ds::isize value) noexcept
    : pointer(pointer), value(value) {
  assert(pointer != nullptr);
  ++*pointer;
}

bool Test::reinit(ds::isize* pointer) noexcept {
  assert(pointer != nullptr);
  if (this->pointer != nullptr) {
    return false;
  }

  this->pointer = pointer;
  ++*pointer;

  return true;
}

bool Test::reinit(ds::isize* pointer, ds::isize value) noexcept {
  assert(pointer != nullptr);
  if (this->pointer != nullptr) {
    return false;
  }

  this->pointer = pointer;
  this->value = value;
  ++*pointer;

  return true;
}

// === Move/Copy === //

Test::Test(Test&& rhs) noexcept : pointer(rhs.pointer), value(rhs.value) {
  rhs.pointer = nullptr;
}

Test& Test::operator=(Test&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  if (this->pointer != nullptr) {
    --*this->pointer;
  }

  this->pointer = other.pointer;
  this->value = other.value;
  other.pointer = nullptr;

  return *this;
}

ds::error_code Test::copy(const Test& other) noexcept {
  if (this == &other) {
    return ds::error::OK;
  }

  this->value = other.value;

  if (this->pointer == nullptr) {
    ++*other.pointer;
  } else if (this->pointer != other.pointer) {
    --*this->pointer;
    ++*other.pointer;
  }
  this->pointer = other.pointer;

  return ds::error::OK;
}

// === Destructor === //

Test::~Test() noexcept {
  this->destroy();
}

void Test::destroy() noexcept {
  if (this->pointer == nullptr) {
    return;
  }

  --*this->pointer;
  this->pointer = nullptr;
}

void Test::set_value(ds::isize value) noexcept {
  this->value = value;
}

ds::isize Test::get_value() const noexcept {
  return this->value;
}

// === Checkers === //

[[nodiscard]] bool Test::operator==(const Test& rhs) const noexcept {
  return this->value == rhs.value;
}

[[nodiscard]] bool Test::operator==(ds::isize rhs) const noexcept {
  return this->value == rhs;
}

[[nodiscard]] bool Test::operator!=(const Test& rhs) const noexcept {
  return this->value != rhs.value;
}

[[nodiscard]] bool Test::operator!=(ds::isize rhs) const noexcept {
  return this->value != rhs;
}

bool operator==(ds::isize lhs, const Test& rhs) noexcept {
  return lhs == rhs.value;
}

bool operator!=(ds::isize lhs, const Test& rhs) noexcept {
  return lhs != rhs.value;
}

} // namespace ds_test
