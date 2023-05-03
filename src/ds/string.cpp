/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#include "./string.hpp"
#include "ds-error/types.hpp"
#include "ds/macro.hpp"
#include "ds/optional.hpp"
#include "ds/types.hpp"
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <type_traits>

namespace ds {

// === Initializers === //
opt_err string::init(const char* str) noexcept {
  if (str == nullptr || str[0] == '\0') { // Empty string check
    return null;
  }

  try_opt(this->allocate(strlen(str)));
  this->_size = this->_max_size;
  strncpy(this->str, str, this->_size);
  this->str[this->_size] = '\0';

  return null;
}

// === Copy === //
opt_err string::copy(const string& other) noexcept {
  if (&other == this) {
    return null;
  }

  if (other._size == 0) {
    this->clear();
    return null;
  }

  if (this->str == nullptr) {
    try_opt(this->allocate(other._size));
    this->_size = other._size;

    strncpy(this->str, other.str, this->_size);
    this->str[this->_size] = '\0';
    return null;
  }

  if (this->_max_size < other._max_size) {
    try_opt(this->reallocate(other._max_size));
    this->_max_size = other._max_size;
  }

  this->_size = other._size;
  strncpy(this->str, other.str, this->_size);
  this->str[this->_size] = '\0';

  return null;
}

opt_err string::copy(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  this->_size = strlen(str);
  if (this->_max_size < this->_size) {
    try_opt(this->reallocate(this->_size));
  }
  strncpy(this->str, str, this->_size);
  this->str[this->_size] = '\0';

  return null;
}

opt_err string::copy(const char* str, i32 size) noexcept {
  if (this->str == nullptr) {
    try_opt(this->allocate(size));
  } else if (this->_max_size < size) {
    try_opt(this->reallocate(size));
  }

  this->_size = size;
  strncpy(this->str, str, size);
  this->str[size] = '\0';

  return null;
}

// === Move === //
string::string(string&& rhs) noexcept
    : str(rhs.str), _size(rhs._size), _max_size(rhs._max_size) {
  rhs.str = nullptr;
  rhs._size = rhs._max_size = 0;
}

string& string::operator=(string&& rhs) noexcept {
  if (this == &rhs) {
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
string::~string() noexcept {
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
opt_err string::allocate(i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  this->str =
      // NOLINTNEXTLINE
      static_cast<char*>(std::malloc((size + 1) * sizeof(char)));
  if (this->str == nullptr) {
    return BAD_ALLOC_OPT;
  }
  this->_max_size = size;

  return null;
}

opt_err string::reallocate(i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  void* ptr = std::realloc(this->str, (size + 1) * sizeof(char));
  if (ptr == nullptr) {
    return BAD_ALLOC_OPT;
  }
  this->str = static_cast<char*>(ptr);
  this->_max_size = size;
  return null;
}

// === Element Access === //
exp_ptr_err<char> string::at_ptr(i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return OUT_OF_RANGE_EXP;
  }

  return this->str + index;
}

exp_err<char> string::at(i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return OUT_OF_RANGE_EXP;
  }

  return this->str[index];
}

char& string::operator[](i32 index) const noexcept {
  return this->str[index];
}

exp_ptr_err<char> string::front_ptr() noexcept {
  if (this->_size == 0) {
    return EMPTY_EXP;
  }

  return this->str;
}

exp_err<char> string::front() noexcept {
  if (this->_size == 0) {
    return EMPTY_EXP;
  }

  return this->str[0];
}

exp_ptr_err<char> string::back_ptr() noexcept {
  if (this->_size == 0) {
    return EMPTY_EXP;
  }

  return this->str + this->_size - 1;
}

exp_err<char> string::back() noexcept {
  if (this->_size == 0) {
    return EMPTY_EXP;
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

opt_err string::reserve(i32 size) noexcept {
  if (this->str == nullptr) {
    return this->allocate(size);
  }

  if (this->_max_size < size) {
    return this->reallocate(size);
  }

  return null;
}

// === Modifiers === //
void string::clear() noexcept {
  if (this->str == nullptr) {
    return;
  }

  this->_size = 0;
  this->str[0] = '\0';
}

opt_err string::push_back(char c) noexcept {
  if (this->_size == this->_max_size) {
    try_opt(this->reallocate(this->_max_size + 10));
  }

  this->str[this->_size++] = c;
  this->str[this->_size] = '\0';

  return null;
}

exp_err<char> string::pop_back() noexcept {
  if (this->_size == 0) {
    return EMPTY_EXP;
  }

  char c = this->str[--this->_size];
  this->str[this->_size] = '\0';
  return c;
}

opt_err string::append(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  i32 str_len = strlen(str);
  if (this->_size + str_len > this->_max_size) {
    try_opt(this->reallocate(this->_size + str_len));
  }

  strncpy(this->str + this->_size, str, str_len);
  this->_size += str_len;
  this->str[this->_size] = '\0';

  return null;
}

opt_err string::append(const char* str, i32 size) noexcept {
  if (this->str == nullptr) {
    try_opt(this->allocate(size));
    strncpy(this->str, str, size);
    this->str[size] = '\0';
    this->_size = size;
    return null;
  }

  i32 new_size = this->_size + size;
  if (new_size > this->_max_size) {
    try_opt(this->reallocate(new_size));
  }

  strncpy(this->str + this->_size, str, size);
  this->str[new_size] = '\0';
  this->_size = new_size;

  return null;
}

opt_err string::append(const string& str) noexcept {
  if (this->str == nullptr) {
    return this->copy(str);
  }

  i32 new_size = this->_size + str._size;
  if (new_size > this->_max_size) {
    try_opt(this->reallocate(new_size + 10));
  }

  strncpy(this->str + this->_size, str.c_str(), str._size);
  this->str[new_size] = '\0';
  this->_size = new_size;

  return null;
}

// === View === //
exp_err<string> string::substr(i32 start) const noexcept {
  string str{};
  try_opt_unexp(str.allocate(this->_size - start));
  str._size = this->_size - start;

  // Copies the '\0'
  for (i32 i = start; i <= this->_size; ++i) {
    str[i - start] = this->str[i];
  }

  return str;
}

exp_err<string> string::substr(i32 start, i32 end) const noexcept {
  string str{};
  try_opt_unexp(str.allocate(end - start));
  str._size = end - start;

  for (i32 i = start; i < end; ++i) {
    str[i - start] = this->str[i];
  }
  str[str._size] = '\0';

  return str;
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

