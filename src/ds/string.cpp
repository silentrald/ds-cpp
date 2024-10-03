/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#include "./string.hpp"
#include "types.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>

namespace ds {

// === Initializers === //

error_code string::init(const char* str) noexcept {
  if (str == nullptr || str[0] == '\0') { // Empty string check
    return error_code::OK;
  }

  TRY(this->allocate(strlen(str)));
  this->size = this->capacity;
  strncpy(this->str, str, this->size);
  this->str[this->size] = '\0';

  return error_code::OK;
}

// === Copy === //
error_code string::copy(const string& other) noexcept {
  if (&other == this) {
    return error_code::OK;
  }

  if (other.size == 0) {
    this->clear();
    return error_code::OK;
  }

  if (this->str == nullptr) {
    TRY(this->allocate(other.size));
    this->size = other.size;

    strncpy(this->str, other.str, this->size);
    this->str[this->size] = '\0';
    return error_code::OK;
  }

  if (this->capacity < other.capacity) {
    TRY(this->reallocate(other.capacity));
    this->capacity = other.capacity;
  }

  this->size = other.size;
  strncpy(this->str, other.str, this->size);
  this->str[this->size] = '\0';

  return error_code::OK;
}

error_code string::copy(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  this->size = strlen(str);
  if (this->capacity < this->size) {
    TRY(this->reallocate(this->size));
  }
  strncpy(this->str, str, this->size);
  this->str[this->size] = '\0';

  return error_code::OK;
}

error_code string::copy(const char* str, usize size) noexcept {
  if (this->str == nullptr) {
    TRY(this->allocate(size));
  } else if (this->capacity < size) {
    TRY(this->reallocate(size));
  }

  this->size = size;
  strncpy(this->str, str, size);
  this->str[size] = '\0';

  return error_code::OK;
}

// === Move === //

string::string(string&& rhs) noexcept
    : str(rhs.str), size(rhs.size), capacity(rhs.capacity) {
  rhs.str = nullptr;
  rhs.size = rhs.capacity = 0;
}

string& string::operator=(string&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }
  this->~string();

  this->str = rhs.str;
  this->size = rhs.size;
  this->capacity = rhs.capacity;

  rhs.str = nullptr;
  rhs.size = rhs.capacity = 0;

  return *this;
}

// === Destructor === //

string::~string() noexcept {
  if (this->str == nullptr) {
    return;
  }

#if DS_TEST
  ds_test::free_ptr = this->str;
#endif

  // NOLINTNEXTLINE
  std::free(this->str);
  this->str = nullptr;
  this->capacity = this->size = 0;
}

// === Memory === //

error_code string::allocate(usize size) noexcept {
  assert(size > 0U);

  // Allocate the \0
  // NOLINTNEXTLINE
  this->str =
      // NOLINTNEXTLINE
      static_cast<char*>(std::malloc((size + 1) * sizeof(char)));
  if (this->str == nullptr) {
    return error_code::BAD_ALLOCATION;
  }
  this->capacity = size;

  return error_code::OK;
}

error_code string::reallocate(usize size) noexcept {
  assert(size >= this->size);

  // Allocate the \0
  // NOLINTNEXTLINE
  void* ptr = std::realloc(this->str, (size + 1) * sizeof(char));
  if (ptr == nullptr) {
    return error_code::BAD_ALLOCATION;
  }
  this->str = static_cast<char*>(ptr);
  this->capacity = size;
  return error_code::OK;
}

// === Element Access === //

expected<c8*, error_code> string::at_ptr(usize index) noexcept {
  if (this->size == 0U && index > 0U) {
    return unexpected{error_code::CONTAINER_EMPTY};
  }

  if (index >= this->size) {
    return unexpected{error_code::INDEX_OUT_OF_BOUNDS};
  }

  return this->str + index;
}

expected<c8, error_code> string::at(usize index) noexcept {
  if (index >= this->size) {
    return unexpected{error_code::INDEX_OUT_OF_BOUNDS};
  }

  return this->str[index];
}

c8& string::operator[](usize index) const noexcept {
  assert(index <= this->size);
  return this->str[index];
}

c8* string::front_ptr() noexcept {
  return this->str;
}

c8 string::front() noexcept {
  return this->size == 0U ? '\0' : this->str[0];
}

c8* string::back_ptr() noexcept {
  return this->size == 0U ? nullptr : this->str + this->size - 1U;
}

c8 string::back() noexcept {
  return this->size == 0U ? '\0' : this->str[this->size - 1U];
}

c8* string::data() noexcept {
  return this->str;
}

const c8* string::c_str() const noexcept {
  return this->str ? this->str : "";
}

// === Capacity === //

bool string::is_empty() const noexcept {
  return this->size == 0;
}

usize string::get_size() const noexcept {
  return this->size;
}

usize string::get_capacity() const noexcept {
  return this->capacity;
}

error_code string::reserve(usize size) noexcept {
  if (this->str == nullptr) {
    return this->allocate(size);
  }

  if (this->capacity < size) {
    return this->reallocate(size);
  }

  return error_code::OK;
}

// === Modifiers === //

void string::clear() noexcept {
  if (this->str == nullptr) {
    return;
  }

  this->size = 0;
  this->str[0] = '\0';
}

error_code string::push_back(char c) noexcept {
  if (this->size == this->capacity) {
    TRY(this->reallocate(this->capacity + 10));
  }

  this->str[this->size++] = c;
  this->str[this->size] = '\0';

  return error_code::OK;
}

expected<c8, error_code> string::pop_back() noexcept {
  if (this->size == 0) {
    return unexpected{error_code::CONTAINER_EMPTY};
  }

  char c = this->str[--this->size];
  this->str[this->size] = '\0';
  return c;
}

error_code string::append(const char* str) noexcept {
  if (this->str == nullptr) {
    return this->init(str);
  }

  i32 length = strlen(str);
  if (this->size + length > this->capacity) {
    TRY(this->reallocate(this->size + length));
  }

  strncpy(this->str + this->size, str, length);
  this->size += length;
  this->str[this->size] = '\0';

  return error_code::OK;
}

error_code string::append(const char* str, usize size) noexcept {
  if (this->str == nullptr) {
    TRY(this->allocate(size));
    strncpy(this->str, str, size);
    this->str[size] = '\0';
    this->size = size;
    return error_code::OK;
  }

  usize new_size = this->size + size;
  if (new_size > this->capacity) {
    TRY(this->reallocate(new_size));
  }

  strncpy(this->str + this->size, str, size);
  this->str[new_size] = '\0';
  this->size = new_size;

  return error_code::OK;
}

error_code string::append(const string& str) noexcept {
  if (this->str == nullptr) {
    return this->copy(str);
  }

  i32 new_size = this->size + str.size;
  if (new_size > this->capacity) {
    TRY(this->reallocate(new_size + 10));
  }

  strncpy(this->str + this->size, str.c_str(), str.size);
  this->str[new_size] = '\0';
  this->size = new_size;

  return error_code::OK;
}

// === View === //

expected<string, error_code> string::substring(usize start) const noexcept {
  // Out of bounds check
  string str{};
  TRY(str.allocate(this->size - start), to_unexpected);
  str.size = this->size - start;

  // Copies the '\0'
  for (i32 i = start; i <= this->size; ++i) {
    str[i - start] = this->str[i];
  }

  return str;
}

expected<string, error_code>
string::substring(usize start, usize end) const noexcept {
  string str{};
  TRY(str.allocate(end - start), to_unexpected);
  str.size = end - start;

  for (i32 i = start; i < end; ++i) {
    str[i - start] = this->str[i];
  }
  str[str.size] = '\0';

  return str;
}

// === Operators === //

bool string::operator==(const string& rhs) const noexcept {
  if (this->size != rhs.size)
    return false;
  if (this->size == 0 && rhs.size == 0)
    return true;

  return strcmp(this->str, rhs.str) == 0;
}

bool string::operator==(const char* rhs) const noexcept {
  if (rhs == nullptr)
    return false;
  if (this->size == 0)
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
  if (rhs.size == 0) {
    return lhs[0] == '\0';
  }

  return strcmp(lhs, rhs.str) == 0;
}

bool operator!=(const char* lhs, const string& rhs) noexcept {
  return !(rhs == lhs);
}

} // namespace ds
