/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_STRING_BUILD_HPP
#define DS_STRING_BUILD_HPP

#include "./expected.hpp"
#include "./types.hpp"

namespace ds {

/**
 * NOTE: Only handles 8 bit characters for now
 * NOTE: No small string optimization yet
 **/
class string {
public:
  string() noexcept = default;
  string(const string& other) = delete;
  string& operator=(const string& rhs) = delete;

  // === Copy === //

  /**
   * Copies the `other` to this string
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const string& other) noexcept;

  /**
   * Copies the `str` to this string.
   *
   * NOTE: Can crash the system if string does not have a null terminator '\0'.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const c8* str) noexcept;

  /**
   * Copies the `str` with length `size` to this string.
   *
   * NOTE: This does not set the '\0' at the end of the string.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code copy(const c8* str, usize size) noexcept;

  // === Move === //

  string(string&& other) noexcept;
  string& operator=(string&& rhs) noexcept;

  // === Destructor === //

  ~string() noexcept;

  // === Element Access === //

  /**
   * @returns pointer to the index
   * @errors
   *  - error::NULL - if the string has not been initialized yet
   *  - error::INDEX_OUT_OF_BOUNDS - out of bounds access
   **/
  [[nodiscard]] expected<c8*, error_code> at_ptr(usize index) noexcept;

  /**
   * @returns character value on the index
   * @errors
   *  - error::INDEX_OUT_OF_BOUNDS - out of bounds access
   **/
  [[nodiscard]] expected<c8, error_code> at(usize index) noexcept;

  [[nodiscard]] c8& operator[](usize index) const noexcept;

  /**
   * @returns pointer to the start of the string.
   *  If the string is empty, this will return nullptr.
   **/
  [[nodiscard]] c8* front_ptr() noexcept;

  /**
   * @returns character at the start of the string.
   *  If the string is empty, this will return '\0'.
   **/
  [[nodiscard]] c8 front() noexcept;

  /**
   * @returns pointer to the end of the string
   *  If the string is empty, this will return nullptr,
   **/
  [[nodiscard]] c8* back_ptr() noexcept;

  /**
   * @returns character at the start of the string.
   *  If the string is empty, this will return '\0'.
   **/
  [[nodiscard]] c8 back() noexcept;
  [[nodiscard]] c8* data() noexcept;
  [[nodiscard]] const c8* c_str() const noexcept;

  // === Iterators === //

  // === Capacity === //

  /**
   * If the string is empty = ""
   **/
  [[nodiscard]] bool is_empty() const noexcept;

  /**
   * Current character count of the string.
   **/
  [[nodiscard]] usize get_size() const noexcept;

  /**
   *
   **/
  [[nodiscard]] usize get_capacity() const noexcept;

  /**
   * Reserves memory space.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code reserve(usize size) noexcept;

  /**
   * Reserves memory space and set the size. String memory will not be set
   *
   * @errors
   *   - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code resize(usize size) noexcept;

  // === Modifiers === //

  /**
   * Removes all characters, similar to setting this to an empty string ""
   **/
  void clear() noexcept;

  /**
   * Adds the character to the end/back of the string.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code push(c8 c) noexcept;

  /**
   * Removes a character from the end/back of the string.
   * Does not check if the string is empty.
   *
   * @returns character at the end/back of the string.
   **/
  [[nodiscard]] c8 pop() noexcept;

  /**
   * Removes a character from the end/back of the string.
   *
   * @returns character at the end/back of the string.
   * @errors
   *  - error::CONTAINER_EMPTY -
   *      if the current string is empty, manually check
   *      the string by calling #is_empty() function
   **/
  [[nodiscard]] expected<c8, error_code> pop_safe() noexcept;

  /**
   * Concatenates the `str` to the end of this string
   *
   * NOTE: Can crash the system if string does not have a null terminator '\0'.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code append(const c8* str) noexcept;

  /**
   * Concatenates the `str` with `size` characters to the end of this string
   *
   * NOTE: This does not set the '\0' at the end of the string.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code append(const c8* str, usize size) noexcept;

  /**
   * Concatenates the `str` to the end of this string
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code append(const string& str) noexcept;

  // === View === //

  /**
   * Creates a new substring from `start` (inclusive) to the end of the string.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   *  - error::INDEX_OUT_OF_BOUNDS
   **/
  [[nodiscard]] expected<string, error_code> substring(usize start
  ) const noexcept;

  /**
   * Creates a new substring from `start` (inclusive) to `end` (exclusive) of
   *the string.
   *
   * @errors
   *  - error::BAD_ALLOCATION
   *  - error::INDEX_OUT_OF_BOUNDS
   **/
  [[nodiscard]] expected<string, error_code>
  substring(usize start, usize end) const noexcept;

  // === Operators === //

  /**
   * Check if the 2 strings are equal.
   **/
  [[nodiscard]] bool operator==(const string& rhs) const noexcept;

  /**
   * Check if the 2 strings are equal.
   **/
  [[nodiscard]] bool operator==(const c8* rhs) const noexcept;

  /**
   * Check if the 2 strings are not equal.
   */
  [[nodiscard]] bool operator!=(const string& rhs) const noexcept;

  /**
   * Check if the 2 strings are not equal.
   **/
  [[nodiscard]] bool operator!=(const c8* rhs) const noexcept;

  // === Non-member functions === //

  /**
   * Check if the 2 strings are equal.
   **/
  friend bool operator==(const c8* lhs, const string& rhs) noexcept;

  /**
   * Check if the 2 strings are not equal.
   **/
  friend bool operator!=(const c8* lhs, const string& rhs) noexcept;

private:
  c8* str = nullptr;
  usize size = 0;
  usize capacity = 0;

  // === Memory === //

  /**
   * Allocates the memory space of the string
   *
   * @param new_capacity - in bytes
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code allocate(usize new_capacity) noexcept;

  /**
   * Reallocates the memory space of the string.
   *
   * @param new_capacity - in bytes
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code reallocate(usize new_capacity) noexcept;

  /**
   * Initializes the string and copies the value of `str`
   *
   * @errors
   *  - error::BAD_ALLOCATION
   **/
  [[nodiscard]] error_code init(const c8* str) noexcept;
};

} // namespace ds

#endif
