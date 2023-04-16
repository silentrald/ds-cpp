/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_ERROR_HPP
#define DS_ERROR_HPP

#include "ds/types.hpp"

#define MAX_ERROR_STACK 10

namespace ds {

struct error_location {
  const char* file = nullptr;
  u32 line = 0;
};

class error {
private:
  // Only accepts const char to not copy any messages
  union {
    const char* _static; // String literals
    char* _dynamic;      // Error built on runtime
  } msg{};               // Initialized to nullptr
  const char* file = nullptr;
  error_location* locations = nullptr;
  i8 size = -1;
  bool dynamic = false;
  u32 line = 0;

public:
  error() noexcept = default;
  error(const error&) = delete;
  error& operator=(const error&) = delete;

  // === Constructors === //

  error(char* msg, const char* fallback, const char* file, u32) noexcept;
  error(char* msg, const char* fallback) noexcept;

  error(const char* msg, const char* file, u32 line) noexcept;
  error(const char* msg) noexcept; // NOLINT

  error(error&& rhs) noexcept;
  error& operator=(error&& rhs) noexcept;

  ~error() noexcept;

  void push_back_location(const char* file, u32 line) noexcept;
  [[nodiscard]] const char* get_msg() const noexcept;
  [[nodiscard]] const char* get_def_file() const noexcept;
  [[nodiscard]] u32 get_def_line() const noexcept;
  [[nodiscard]] error_location* get_location(i32 index) const noexcept;

  /**
   * Only returns the items within the locations array
   **/
  [[nodiscard]] i8 get_size() const noexcept;
};

} // namespace ds

#endif
