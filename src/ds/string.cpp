/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#include "./string.hpp"
#include "ds/macro.hpp"
#include "ds/types.hpp"
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <type_traits>

namespace ds {

// === Initializers === //
err_code string::init(const char* str) noexcept {
  if (str == nullptr || str[0] == '\0') { // Empty string check
    return ec::SUCCESS;
  }

  try_err_code(this->allocate(strlen(str)));
  this->_size = this->_max_size;
  strncpy(this->str, str, this->_size);
  this->str[this->_size] = '\0';

  return ec::SUCCESS;
}

// === Copy === //
err_code string::copy(const string& other) noexcept {
  if (&other == this) {
    return ec::SUCCESS;
  }

  if (other._size == 0) {
    this->clear();
    return ec::SUCCESS;
  }

  if (this->str == nullptr) {
    try_err_code(this->allocate(other._size));
    this->_size = other._size;

    strncpy(this->str, other.str, this->_size);
    this->str[this->_size] = '\0';
    return ec::SUCCESS;
  }

  if (this->_max_size < other._max_size) {
    try_err_code(this->reallocate(other._max_size));
    this->_max_size = other._max_size;
  }

  this->_size = other._size;
  strncpy(this->str, other.str, this->_size);
  this->str[this->_size] = '\0';

  return ec::SUCCESS;
}

err_code string::copy(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  this->_size = strlen(str);
  if (this->_max_size < this->_size) {
    try_err_code(this->reallocate(this->_size));
  }
  strncpy(this->str, str, this->_size);
  this->str[this->_size] = '\0';

  return ec::SUCCESS;
}

err_code string::copy(const char* str, i32 size) noexcept {
  if (this->str == nullptr) {
    try_err_code(this->allocate(size));
  } else if (this->_max_size < size) {
    try_err_code(this->reallocate(size));
  }

  this->_size = size;
  strncpy(this->str, str, size);
  this->str[size] = '\0';

  return ec::SUCCESS;
}

// === Move === //
string::string(string&& rhs) noexcept
    : str(rhs.str), _size(rhs._size), _max_size(rhs._max_size) {
  rhs.str = nullptr;
  rhs._size = rhs._max_size = 0;
}

string& string::operator=(string&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  this->~string();

  this->str = rhs.str;
  this->_size = rhs._size;
  this->_max_size = rhs._max_size;

  rhs.str = nullptr;
  rhs._size = rhs._max_size = 0;

  return *this;
}

// === Destructor === //
string::~string() {
  if (this->str) {
#if DS_TEST
    ds_test::free_ptr = this->str;
#endif

    // NOLINTNEXTLINE
    std::free(this->str);
    this->str = nullptr;
    this->_max_size = this->_size = 0;
  }
}

// === Memory === //
err_code string::allocate(i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  this->str =
      // NOLINTNEXTLINE
      static_cast<char*>(std::malloc((size + 1) * sizeof(char)));
  if (this->str == nullptr) {
    return ec::BAD_ALLOC;
  }
  this->_max_size = size;

  return ec::SUCCESS;
}

err_code string::reallocate(i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  void* ptr = std::realloc(this->str, (size + 1) * sizeof(char));
  if (ptr == nullptr) {
    return ec::BAD_ALLOC;
  }
  this->str = static_cast<char*>(ptr);
  this->_max_size = size;
  return ec::SUCCESS;
}

// === Element Access === //
exp_ptr_err_code<char> string::at_ptr(i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return unexpected{ec::OUT_OF_RANGE};
  }

  return this->str + index;
}

exp_err_code<char> string::at(i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return unexpected{ec::OUT_OF_RANGE};
  }

  return this->str[index];
}

char& string::operator[](i32 index) noexcept {
  return this->str[index];
}

exp_ptr_err_code<char> string::front_ptr() noexcept {
  if (this->_size == 0) {
    return unexpected{ec::EMPTY};
  }

  return this->str;
}

exp_err_code<char> string::front() noexcept {
  if (this->_size == 0) {
    return unexpected{ec::EMPTY};
  }

  return this->str[0];
}

exp_ptr_err_code<char> string::back_ptr() noexcept {
  if (this->_size == 0) {
    return unexpected{ec::EMPTY};
  }

  return this->str + this->_size - 1;
}

exp_err_code<char> string::back() noexcept {
  if (this->_size == 0) {
    return unexpected{ec::EMPTY};
  }

  return this->str[this->_size - 1];
}

char* string::data() noexcept {
  return this->str;
}

const char* string::c_str() const noexcept {
  return this->str ? this->str : "";
}

// === Capacity === //
bool string::is_empty() const noexcept {
  return this->_size == 0;
}

i32 string::size() const noexcept {
  return this->_size;
}

i32 string::length() const noexcept {
  return this->_size;
}

i32 string::max_size() const noexcept {
  return this->_max_size;
}

err_code string::reserve(i32 size) noexcept {
  if (this->str == nullptr) {
    return this->allocate(size);
  }

  if (this->_max_size < size) {
    return this->reallocate(size);
  }

  return ec::SUCCESS;
}

// === Modifiers === //
void string::clear() noexcept {
  if (this->str == nullptr) {
    return;
  }

  this->_size = 0;
  this->str[0] = '\0';
}

err_code string::push_back(char c) noexcept {
  if (this->_size == this->_max_size) {
    try_err_code(this->reallocate(this->_max_size + 10));
  }

  this->str[this->_size++] = c;
  this->str[this->_size] = '\0';

  return ec::SUCCESS;
}

exp_err_code<char> string::pop_back() noexcept {
  if (this->_size == 0) {
    return unexpected{ec::EMPTY};
  }

  char c = this->str[--this->_size];
  this->str[this->_size] = '\0';
  return c;
}

err_code string::append(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  i32 str_len = strlen(str);
  if (this->_size + str_len > this->_max_size) {
    try_err_code(this->reallocate(this->_size + str_len));
  }

  strncpy(this->str + this->_size, str, str_len);
  this->_size += str_len;
  this->str[this->_size] = '\0';

  return ec::SUCCESS;
}

err_code string::append(const char* str, i32 size) noexcept {
  if (this->str == nullptr) {
    try_err_code(this->allocate(size));
    strncpy(this->str, str, size);
    this->str[size] = '\0';
    this->_size = size;
    return ec::SUCCESS;
  }

  i32 new_size = this->_size + size;
  if (new_size > this->_max_size) {
    try_err_code(this->reallocate(new_size));
  }

  strncpy(this->str + this->_size, str, size);
  this->str[new_size] = '\0';
  this->_size = new_size;

  return ec::SUCCESS;
}

err_code string::append(const string& str) noexcept {
  if (this->str == nullptr) {
    return this->copy(str);
  }

  i32 new_size = this->_size + str._size;
  if (new_size > this->_max_size) {
    try_err_code(this->reallocate(new_size + 10));
  }

  strncpy(this->str + this->_size, str.c_str(), str._size);
  this->str[new_size] = '\0';
  this->_size = new_size;

  return ec::SUCCESS;
}

// === Operators === //
bool string::operator==(const string& rhs) const noexcept {
  if (this->_size != rhs._size)
    return false;
  if (this->_size == 0 && rhs._size == 0)
    return true;

  return strcmp(this->str, rhs.str) == 0;
}

bool string::operator==(const char* rhs) const noexcept {
  if (rhs == nullptr)
    return false;
  if (this->_size == 0)
    return rhs[0] == '\0';

  return strcmp(this->str, rhs) == 0;
}

bool string::operator!=(const string& rhs) const noexcept {
  return !(*this == rhs);
}

bool string::operator!=(const char* rhs) const noexcept {
  return !(*this == rhs);
}

// === Non-member functions === //
bool operator==(const char* lhs, const string& rhs) noexcept {
  if (lhs == nullptr)
    return false;
  if (rhs._size == 0) {
    return lhs[0] == '\0';
  }

  return strcmp(lhs, rhs.str) == 0;
}

bool operator!=(const char* lhs, const string& rhs) noexcept {
  return !(rhs == lhs);
}

std::ostream& operator<<(std::ostream& out, const string& rhs) noexcept {
  if (rhs._size == 0) {
    return out;
  }

  return out << rhs.str;
}

} // namespace ds

