/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-12-27
 *===============================*/

#include "./string.hpp"
#include "./macro.hpp"
#include <cstdlib>
#include <cstring>
#include <type_traits>

namespace ds {

// === Initializers === //
opt_err string::init(const char* str) noexcept {
  if (str[0] == '\0') { // Empty string check
    return null;
  }

  try_opt(this->allocate(std::strlen(str)));
  this->_size = this->_max_size;
  strcpy(this->str, str);

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
    std::strcpy(this->str, other.str);
    return null;
  }

  if (this->_max_size < other._max_size) {
    try_opt(this->reallocate(other._max_size));
    this->_max_size = other._max_size;
  }

  this->_size = other._size;
  std::strcpy(this->str, other.str);

  return null;
}

opt_err string::copy(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  this->_size = std::strlen(str);
  if (this->_max_size < this->_size) {
    try_opt(this->reallocate(this->_size));
  }
  std::strcpy(this->str, str);

  return null;
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
opt_err string::allocate(i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  this->str =
      // NOLINTNEXTLINE
      static_cast<char*>(std::malloc((size + 1) * sizeof(char)));
  if (this->str == nullptr) {
    return error{STRING_BAD_ALLOC};
  }
  this->_max_size = size;

  return null;
}

opt_err string::reallocate(i32 size) noexcept {
  // Allocate the \0
  // NOLINTNEXTLINE
  void* ptr = std::realloc(this->str, (size + 1) * sizeof(char));
  if (ptr == nullptr) {
    return error{STRING_BAD_ALLOC};
  }
  this->str = static_cast<char*>(ptr);
  this->_max_size = size;
  return null;
}

// === Element Access === //
exp_ptr_err<char> string::at_ptr(i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return unexpected{error{STRING_OUT_OF_RANGE, def_err_vals}};
  }

  return this->str + index;
}

exp_err<char> string::at(i32 index) noexcept {
  if (index < 0 || index >= this->_size) {
    return unexpected{error{STRING_OUT_OF_RANGE, def_err_vals}};
  }

  return this->str[index];
}

char& string::operator[](i32 index) noexcept {
  return this->str[index];
}

exp_ptr_err<char> string::front_ptr() noexcept {
  if (this->_size == 0) {
    return unexpected{error{STRING_EMPTY, def_err_vals}};
  }

  return this->str;
}

exp_err<char> string::front() noexcept {
  if (this->_size == 0) {
    return unexpected{error{STRING_EMPTY, def_err_vals}};
  }

  return this->str[0];
}

exp_ptr_err<char> string::back_ptr() noexcept {
  if (this->_size == 0) {
    return unexpected{error{STRING_EMPTY, def_err_vals}};
  }

  return this->str + this->_size - 1;
}

exp_err<char> string::back() noexcept {
  if (this->_size == 0) {
    return unexpected{error{STRING_EMPTY, def_err_vals}};
  }

  return this->str[this->_size - 1];
}

char* string::data() noexcept {
  return this->str;
}

const char* string::c_str() const noexcept {
  return this->str;
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
    return unexpected(error{STRING_EMPTY});
  }

  char c = this->str[--this->_size];
  this->str[this->_size] = '\0';
  return c;
}

opt_err string::append(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  i32 new_size = this->_size + std::strlen(str);
  if (new_size > this->_max_size) {
    try_opt(this->reallocate(new_size));
  }

  std::strcpy(this->str + this->_size, str);
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

  std::strcpy(this->str + this->_size, str.c_str());
  this->_size = new_size;

  return null;
}

// === Operators === //
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

// === Non-member functions === //
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

