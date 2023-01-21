/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2023-01-07
 *===============================*/

#ifndef DS_ERROR_HPP
#define DS_ERROR_HPP

#include "./types.hpp"
#include "expected/def.hpp"
#include "optional/def.hpp"
#include <cstdlib>
#include <cstring>

#define MAX_ERROR_STACK 10

namespace ds {

struct error_location {
  char* file = nullptr;
  char* function = nullptr;
  i32 line = 0;

  void destroy() noexcept {
    if (this->file) {
      std::free(this->file); // NOLINT
      this->file = nullptr;
    }

    if (this->function) {
      std::free(this->function); // NOLINT
      this->function = nullptr;
    }
  }
};

class error {
private:
  char* msg = nullptr;
  error_location* locations = nullptr;
  i32 size = 0;

public:
  error() noexcept = default;
  error(const error&) = delete;
  error& operator=(const error&) = delete;

  // === Constructors === //
  explicit error(const char* msg) noexcept {
    this->set_msg(msg);
  }

  error(
      const char* msg, const char* file, const char* function, i32 line
  ) noexcept {
    this->set_msg(msg);
    this->push_back_location(file, function, line);
  }

  error(error&& rhs) noexcept
      : msg(rhs.msg), locations(rhs.locations), size(rhs.size) {
    rhs.msg = nullptr;
    rhs.locations = nullptr;
    rhs.size = 0;
  }

  error& operator=(error&& rhs) noexcept {
    if (&rhs == this) {
      return *this;
    }
    this->~error();

    this->msg = rhs.msg;
    this->locations = rhs.locations;

    rhs.msg = nullptr;
    rhs.locations = nullptr;
    rhs.size = 0;

    return *this;
  }

  ~error() {
    if (this->msg) {
      std::free(this->msg); // NOLINT
      this->msg = nullptr;
    }

    for (i32 i = 0; i < this->size; ++i) {
      this->locations[i].destroy();
    }
  }

  void set_msg(const char* msg) {
    if (this->msg) {
      std::free(this->msg); // NOLINT
      this->msg = nullptr;
    }

    this->msg =                                                // NOLINT
        static_cast<char*>(std::malloc(std::strlen(msg) + 1)); // NOLINT
    if (this->msg == nullptr)
      return;

    std::strcpy(this->msg, msg);
  }

  void push_back_location(
      const char* file, const char* function, i32 line
  ) noexcept {
    if (this->size >= MAX_ERROR_STACK) {
      return;
    }

    if (this->locations == nullptr) {
      // NOLINTNEXTLINE
      this->locations = static_cast<error_location*>(
          std::malloc(sizeof(error_location) * MAX_ERROR_STACK) // NOLINT
      );
      if (this->locations == nullptr) {
        return; // Just give up :)
      }
    }

    auto* location = this->locations + this->size;

    // NOLINTNEXTLINE
    location->file = static_cast<char*>(std::malloc(std::strlen(file) + 1));
    if (location->file) {
      std::strcpy(location->file, file);
    }

    location->function =                                            // NOLINT
        static_cast<char*>(std::malloc(std::strlen(function) + 1)); // NOLINT
    if (location->function) {
      std::strcpy(location->function, function);
    }

    location->line = line;

    ++this->size;
  }

  [[nodiscard]] const char* get_msg() const noexcept {
    return this->msg ? this->msg : "couldn't create err msg";
  }

  [[nodiscard]] error_location* get_location(i32 index) const noexcept {
    return this->locations + index;
  }

  [[nodiscard]] i32 get_size() const noexcept {
    return this->size;
  }
};

using opt_err = optional<error>;
template <typename T> using exp_err = expected<T, error>;
template <typename T> using exp_ptr_err = expected_ptr<T, error>;

} // namespace ds

#endif
