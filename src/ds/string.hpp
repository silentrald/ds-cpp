/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_STRING_BUILD_HPP
#define DS_STRING_BUILD_HPP

#include "ds/types.hpp"
#include <ostream>

#if DS_TEST
#include "../../tests/main.hpp"
#endif

namespace ds {

// NOTE: Only handles 8 bit characters for now
class string {
public:
private:
  char* str = nullptr;
  i32 _size = 0;
  i32 _max_size = 0;

  // === Memory === //
  [[nodiscard]] err_code allocate(i32 size) noexcept;
  [[nodiscard]] err_code reallocate(i32 size) noexcept;

  /**
   * Initializes the string with the passed const char*
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] err_code init(const char* str) noexcept;

public:
  string() noexcept = default;
  string(const string& rhs) = delete;
  string& operator=(const string& rhs) = delete;

  // === Copy === //
  /**
   * Copies the string
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] err_code copy(const string& other) noexcept;

  /**
   * Copies the const char* string
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] err_code copy(const char* str) noexcept;

  /**
   * Copies the const char* string with a size limit
   *
   * @errors
   *  - bad allocation
   **/
  [[nodiscard]] err_code copy(const char* str, i32 size) noexcept;

  // === Move === //
  /**
   * Initializes the string and moves the passed string
   **/
  string(string&& rhs) noexcept;
  string& operator=(string&& rhs) noexcept;

  // === Destructor === //
  ~string();

  // === Element Access === //
  /**
   * Returns a pointer from the indexed passed
   **/
  [[nodiscard]] exp_ptr_err_code<char> at_ptr(i32 index) noexcept;
  [[nodiscard]] exp_err_code<char> at(i32 index) noexcept;
  [[nodiscard]] char& operator[](i32 index) noexcept;
  [[nodiscard]] exp_ptr_err_code<char> front_ptr() noexcept;
  [[nodiscard]] exp_err_code<char> front() noexcept;
  [[nodiscard]] exp_ptr_err_code<char> back_ptr() noexcept;
  [[nodiscard]] exp_err_code<char> back() noexcept;
  [[nodiscard]] char* data() noexcept;
  [[nodiscard]] const char* c_str() const noexcept;

  // === Iterators === //

  // === Capacity === //
  [[nodiscard]] bool is_empty() const noexcept;
  [[nodiscard]] i32 size() const noexcept;
  [[nodiscard]] i32 length() const noexcept;
  [[nodiscard]] i32 max_size() const noexcept;
  [[nodiscard]] err_code reserve(i32 size) noexcept;

  // === Modifiers === //
  void clear() noexcept;
  [[nodiscard]] err_code push_back(char c) noexcept;
  [[nodiscard]] exp_err_code<char> pop_back() noexcept;
  [[nodiscard]] err_code append(const char* str) noexcept;
  [[nodiscard]] err_code append(const char* str, i32 size) noexcept;
  [[nodiscard]] err_code append(const string& str) noexcept;

  // === Operators === //
  bool operator==(const string& rhs) const noexcept;
  bool operator==(const char* rhs) const noexcept;
  bool operator!=(const string& rhs) const noexcept;
  bool operator!=(const char* rhs) const noexcept;

  // === Non-member functions === //
  friend bool operator==(const char* lhs, const string& rhs) noexcept;
  friend bool operator!=(const char* lhs, const string& rhs) noexcept;
  friend std::ostream&
  operator<<(std::ostream& out, const string& rhs) noexcept;
};

} // namespace ds

#endif

