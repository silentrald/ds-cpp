/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-12-27
 *===============================*/

#include "../macro.hpp"
#include "./def.hpp"
#include <cstdlib>
#include <cstring>
#include <type_traits>

namespace ds {

const char* const STRING_BAD_ALLOC = "string:bad_alloc";
const char* const STRING_OUT_OF_RANGE = "string:out_of_range";
const char* const STRING_EMPTY = "string:empty";

// === Initializers ===
ds::opt_error string::init(const char* str) noexcept {
  if (str[0] == '\0') { // Empty string check
    return ds::null;
  }

  try_opt(this->allocate(std::strlen(str)));
  this->_size = this->_max_size;
  strcpy(this->str, str);

  return ds::null;
}

// === Copy ===
ds::opt_error string::copy(const string& other) noexcept {
  if (&other == this) {
    return ds::null;
  }

  if (other._size == 0) {
    this->clear();
    return ds::null;
  }

  if (this->str == nullptr) {
    try_opt(this->allocate(other._size));
    this->_size = other._size;
    std::strcpy(this->str, other.str);
    return ds::null;
  }

  if (this->_max_size < other._max_size) {
    try_opt(this->reallocate(other._max_size));
    this->_max_size = other._max_size;
  }

  this->_size = other._size;
  std::strcpy(this->str, other.str);

  return ds::null;
}

ds::opt_error string::copy(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  this->_size = std::strlen(str);
  if (this->_max_size < this->_size) {
    try_opt(this->reallocate(this->_size));
  }
  std::strcpy(this->str, str);

  return ds::null;
}

// === Move ===
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

// === Destructor ===
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

// Private Functions
ds::opt_error string::allocate(ds::i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  this->str =
      // NOLINTNEXTLINE
      static_cast<char*>(std::malloc((size + 1) * sizeof(char)));
  if (this->str == nullptr) {
    return ds::error{STRING_BAD_ALLOC};
  }
  this->_max_size = size;

  return ds::null;
}

ds::opt_error string::reallocate(ds::i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  void* ptr = std::realloc(this->str, (size + 1) * sizeof(char));
  if (ptr == nullptr) {
    return ds::error{STRING_BAD_ALLOC};
  }
  this->str = static_cast<char*>(ptr);
  this->_max_size = size;
  return ds::null;
}

// === Element Access ===
ds::expected_ptr<char> string::at_ptr(ds::i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return ds::unexpected(ds::error{STRING_OUT_OF_RANGE});
  }

  return this->str + index;
}

ds::expected<char> string::at(ds::i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return ds::unexpected(ds::error{STRING_OUT_OF_RANGE});
  }

  return this->str[index];
}

char& string::operator[](ds::i32 index) noexcept {
  return this->str[index];
}

ds::expected_ptr<char> string::front_ptr() noexcept {
  if (this->_size == 0) {
    return ds::unexpected(ds::error{STRING_EMPTY});
  }

  return this->str;
}

ds::expected<char> string::front() noexcept {
  if (this->_size == 0) {
    return ds::unexpected(ds::error{STRING_EMPTY});
  }

  return this->str[0];
}

ds::expected_ptr<char> string::back_ptr() noexcept {
  if (this->_size == 0) {
    return ds::unexpected(ds::error{STRING_EMPTY});
  }

  return this->str + this->_size - 1;
}

ds::expected<char> string::back() noexcept {
  if (this->_size == 0) {
    return ds::unexpected(ds::error{STRING_EMPTY});
  }

  return this->str[this->_size - 1];
}

char* string::data() noexcept {
  return this->str;
}

const char* string::c_str() const noexcept {
  return this->str;
}

// === Capacity ===
bool string::empty() const noexcept {
  return this->_size == 0;
}

ds::i32 string::size() const noexcept {
  return this->_size;
}

ds::i32 string::length() const noexcept {
  return this->_size;
}

ds::i32 string::max_size() const noexcept {
  return this->_max_size;
}

ds::opt_error string::reserve(ds::i32 size) noexcept {
  if (this->str == nullptr) {
    return this->allocate(size);
  }

  if (this->_max_size < size) {
    return this->reallocate(size);
  }

  return ds::null;
}

// === Modifiers ===
void string::clear() noexcept {
  if (this->str == nullptr) {
    return;
  }

  this->_size = 0;
  this->str[0] = '\0';
}

ds::opt_error string::push_back(char c) noexcept {
  if (this->_size == this->_max_size) {
    try_opt(this->reallocate(this->_max_size + 10));
  }

  this->str[this->_size++] = c;
  this->str[this->_size] = '\0';

  return ds::null;
}

ds::expected<char> string::pop_back() noexcept {
  if (this->_size == 0) {
    return ds::unexpected(ds::error{STRING_EMPTY});
  }

  char c = this->str[--this->_size];
  this->str[this->_size] = '\0';
  return c;
}

ds::opt_error string::append(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  ds::i32 new_size = this->_size + std::strlen(str);
  if (new_size > this->_max_size) {
    try_opt(this->reallocate(new_size));
  }

  std::strcpy(this->str + this->_size, str);
  this->_size = new_size;

  return ds::null;
}

ds::opt_error string::append(const string& str) noexcept {
  if (this->str == nullptr) {
    return this->copy(str);
  }

  ds::i32 new_size = this->_size + str._size;
  if (new_size > this->_max_size) {
    try_opt(this->reallocate(new_size + 10));
  }

  std::strcpy(this->str + this->_size, str.c_str());
  this->_size = new_size;

  return ds::null;
}

// === Operators ===
bool string::operator==(const string& rhs) const noexcept {
  if (this->_size != rhs._size)
    return false;
  if (this->_size == 0 && rhs._size == 0)
    return true;

  return std::strcmp(this->str, rhs.str) == 0;
}

bool string::operator==(const char* rhs) const noexcept {
  if (rhs == nullptr)
    return false;
  if (this->_size == 0)
    return rhs[0] == '\0';

  return std::strcmp(this->str, rhs) == 0;
}

bool string::operator!=(const string& rhs) const noexcept {
  return !(*this == rhs);
}

bool string::operator!=(const char* rhs) const noexcept {
  return !(*this == rhs);
}

// === Non-member functions ===
bool operator==(const char* lhs, const string& rhs) noexcept {
  if (lhs == nullptr)
    return false;
  if (rhs._size == 0) {
    return lhs[0] == '\0';
  }

  return std::strcmp(lhs, rhs.str) == 0;
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

