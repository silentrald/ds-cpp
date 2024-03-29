/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-16
 *===============================*/

#include "./error.hpp"
#include "ds/types.hpp"
#include <cstdlib>
#include <cstring>

#ifdef DS_THREAD
#include <pthread.h>
#endif

namespace ds {

namespace memory {

// TODO: Can be edited
const i32 MAX_ERROR_MSG = DS_ERROR_MSG;
#ifdef DS_THREAD
char err_memory_space[MAX_ERROR_MSG * 32];             // NOLINT
u32 err_bitset = 0;                                    // NOLINT
pthread_mutex_t err_mutex = PTHREAD_MUTEX_INITIALIZER; // NOLINT
#else
char err_memory_space[MAX_ERROR_MSG]; // NOLINT
#endif

i32 get_err_memory() noexcept {
#ifdef DS_THREAD
  pthread_mutex_lock(&err_mutex);
  i32 i = 0;
  for (u32 bits = err_bitset; bits > 0U; bits >> 1, ++i) { // NOLINT
    if ((bits & 1) == 0) {
      err_bitset ^= 1 << i;
      pthread_mutex_unlock(&err_mutex);
      // TODO: If 32 bit system needs more addresses, return the i instead
      return i * MAX_ERROR_MSG; // NOLINT
    }
  }
  pthread_mutex_unlock(&err_mutex);
  return -1;
#endif // DS_THREAD
  return 0;
}

// Assume that every return is valid
#ifdef DS_THREAD
void return_err_memory(u32 pos) noexcept {
  // Toggle the bit
  err_bitset ^= 1 << (pos / MAX_ERROR_MSG);
}
#endif

} // namespace memory

// === Constructors === //
#ifdef BIT_64
error::error(
    char* msg, i32 type, const char* fallback, const char* file, u32 line
) noexcept
    : file(file), line(line), size(0) {
  this->msg._dynamic.pos = memory::get_err_memory();
  if (this->msg._dynamic.pos > -1) {
    i32 size = strnlen(msg, MAX_ERROR_STACK);
    strncpy(memory::err_memory_space + this->msg._dynamic.pos, msg, size);
    memory::err_memory_space[this->msg._dynamic.pos + size] = '\0';
    this->msg._dynamic.type = type;

    this->dynamic = true;
    return;
  }

  this->msg._static = fallback;
  this->dynamic = false;
}

error::error(char* msg, i32 type, const char* fallback) noexcept {
  this->msg._dynamic.pos = memory::get_err_memory();
  if (this->msg._dynamic.pos > -1) {
    i32 size = strnlen(msg, MAX_ERROR_STACK);
    strncpy(memory::err_memory_space + this->msg._dynamic.pos, msg, size);
    memory::err_memory_space[this->msg._dynamic.pos + size] = '\0';
    this->msg._dynamic.type = type;

    this->dynamic = true;
    return;
  }

  this->msg._static = fallback;
  this->dynamic = false;
}
#else
error::error(
    char* msg, i16 type, const char* fallback, const char* file, u32 line
) noexcept
    : file(file), line(line), size(0) {
  this->msg._dynamic.pos = memory::get_err_memory();
  if (this->msg._dynamic.pos > -1) {
    i32 size = strnlen(msg, MAX_ERROR_STACK);
    strncpy(memory::err_memory_space + this->msg._dynamic.pos, msg, size);
    memory::err_memory_space[this->msg._dynamic.pos + size] = '\0';
    this->msg._dynamic.type = type;

    this->dynamic = true;
    return;
  }

  this->msg._static = fallback;
  this->dynamic = false;
}

error::error(char* msg, i16 type, const char* fallback) noexcept {
  this->msg._dynamic.pos = memory::get_err_memory();
  if (this->msg._dynamic.pos > -1) {
    i32 size = strnlen(msg, MAX_ERROR_STACK);
    strncpy(memory::err_memory_space + this->msg._dynamic.pos, msg, size);
    memory::err_memory_space[this->msg._dynamic.pos + size] = '\0';
    this->msg._dynamic.type = type;

    this->dynamic = true;
    return;
  }

  this->msg._static = fallback;
  this->dynamic = false;
}
#endif

error::error(const char* msg, const char* file, u32 line) noexcept
    : file(file), line(line), size(0) {
  this->msg._static = msg; // NOLINT
}

error::error(const char* msg) noexcept {
  this->msg._static = msg;
}

error::error(error&& rhs) noexcept
    : msg(rhs.msg),
      file(rhs.file),
      line(rhs.line),
      locations(rhs.locations),
      dynamic(rhs.dynamic),
      size(rhs.size) {
  /* rhs.msg = nullptr; Wouldn't matter that much */
  rhs.locations = nullptr;
  rhs.dynamic = false;
  rhs.size = 0;
}

error& error::operator=(error&& rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }
  this->~error();

  this->msg = rhs.msg;
  this->locations = rhs.locations;
  this->file = rhs.file;
  this->line = rhs.line;

  /* rhs.msg = nullptr; Wouldn't matter that much */
  rhs.locations = nullptr;
  rhs.size = 0;

  return *this;
}

error::~error() noexcept {
#ifdef DS_THREAD
  if (this->dynamic) {
    memory::return_err_memory(this->msg._dynamic.pos);
  }
#endif

  if (this->locations) {
    std::free(this->locations); // NOLINT
  }
}

void error::push_back_location(const char* file, u32 line) noexcept {
  if (this->size >= MAX_ERROR_STACK || this->size == -2) {
    return;
  }

  if (this->size == -1) {
    this->file = file;
    this->line = line;
    return;
  }

  if (this->locations == nullptr) {
    // NOLINTNEXTLINE
    this->locations = static_cast<error_location*>(
        std::malloc(sizeof(error_location) * MAX_ERROR_STACK) // NOLINT
    );
    if (this->locations == nullptr) {
      // Just give up :), you have atleast 1 location if this fails
      this->size = -2;
      return;
    }
  }

  this->locations[this->size].file = file;
  this->locations[this->size].line = line;

  ++this->size;
}

const char* error::get_msg() const noexcept {
  return this->msg._static;
}

const char* error::get_def_file() const noexcept {
  return this->file ? this->file : "";
}

u32 error::get_def_line() const noexcept {
  return this->line;
}

error_location* error::get_location(i32 index) const noexcept {
  return this->locations + index;
}

#ifdef BIT_64
i32 error::get_type() const noexcept {
  return this->dynamic ? this->msg._dynamic.type : 0;
}
#else
i16 error::get_type() const noexcept {
  return this->dynamic ? this->msg._dynamic.type : 0;
}
#endif

/**
 * Only returns the items within the locations array
 **/
i8 error::get_size() const noexcept {
  return this->size;
}

} // namespace ds

