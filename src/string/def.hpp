/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 * Updated: 2022-12-27
 *===============================*/

#ifndef DS_STRING_BUILD_HPP
#define DS_STRING_BUILD_HPP

#include "../error.hpp"
#include "../types.hpp"
#include <ostream>

#if DS_TEST
#include "../test.hpp"
#endif

namespace ds {

// NOTE: Only handles 8 bit characters for now
class string {
public:
private:
  char* str = nullptr;
  ds::i32 _size = 0;
  ds::i32 _max_size = 0;

  // === Memory ===
  [[nodiscard]] ds::opt_error allocate(ds::i32 size) noexcept;
  [[nodiscard]] ds::opt_error reallocate(ds::i32 size) noexcept;

  /**
   * Initializes the string with the passed const char*
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] ds::opt_error init(const char* str) noexcept;

public:
  string() noexcept = default;
  string(const string& rhs) = delete;
  string& operator=(const string& rhs) = delete;

  // === Copy ===
  /**
   * Copies the string
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] ds::opt_error copy(const string& other) noexcept;

  /**
   * Copies the const char* string
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] ds::opt_error copy(const char* str) noexcept;

  // === Move ===
  /**
   * Initializes the string and moves the passed string
   **/
  string(string&& rhs) noexcept;
  string& operator=(string&& rhs) noexcept;

  // === Destructor ===
  ~string();

  // === Element Access ===
  /**
   * Returns a pointer from the indexed passed
   **/
  [[nodiscard]] ds::expected_ptr<char> at_ptr(ds::i32 index) noexcept;
  [[nodiscard]] ds::expected<char> at(ds::i32 index) noexcept;
  [[nodiscard]] char& operator[](ds::i32 index) noexcept;
  [[nodiscard]] ds::expected_ptr<char> front_ptr() noexcept;
  [[nodiscard]] ds::expected<char> front() noexcept;
  [[nodiscard]] ds::expected_ptr<char> back_ptr() noexcept;
  [[nodiscard]] ds::expected<char> back() noexcept;
  [[nodiscard]] char* data() noexcept;
  [[nodiscard]] const char* c_str() const noexcept;

  // === Iterators ===

  // === Capacity ===
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] ds::i32 size() const noexcept;
  [[nodiscard]] ds::i32 length() const noexcept;
  [[nodiscard]] ds::i32 max_size() const noexcept;
  [[nodiscard]] ds::opt_error reserve(ds::i32 size) noexcept;

  // === Modifiers ===
  void clear() noexcept;
  [[nodiscard]] ds::opt_error push_back(char c) noexcept;
  [[nodiscard]] ds::expected<char> pop_back() noexcept;
  [[nodiscard]] ds::opt_error append(const char* str) noexcept;
  [[nodiscard]] ds::opt_error append(const string& str) noexcept;

  // === Operators ===
  bool operator==(const string& rhs) const noexcept;
  bool operator==(const char* rhs) const noexcept;
  bool operator!=(const string& rhs) const noexcept;
  bool operator!=(const char* rhs) const noexcept;

  // === Non-member functions ===
  friend bool operator==(const char* lhs, const string& rhs) noexcept;
  friend bool operator!=(const char* lhs, const string& rhs) noexcept;
  friend std::ostream&
  operator<<(std::ostream& out, const string& rhs) noexcept;
};

} // namespace ds

#endif

