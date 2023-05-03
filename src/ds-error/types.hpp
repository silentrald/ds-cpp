/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-16
 *===============================*/

#ifndef DS_ERROR_TYPES_HPP
#define DS_ERROR_TYPES_HPP

#include "ds-error/error.hpp"
#include "ds-error/expected.hpp"
#include "ds/optional.hpp"

namespace ds {

using opt_err = optional<error>;
template <typename T> using exp_err = expected<T, error>;
template <typename T> using exp_ptr_err = expected_ptr<T, error>;

const char* const DS_ERROR_BAD_ALLOC = "bad alloc";
const char* const DS_ERROR_OUT_OF_RANGE = "out of range";
const char* const DS_ERROR_EMPTY = "empty";
const char* const DS_ERROR_FULL = "full";
const char* const DS_ERROR_NOT_FOUND = "not found";
const char* const DS_ERROR_SIZE = "size";
const char* const DS_ERROR_ALREADY_SET = "already set";
const char* const DS_ERROR_NOT_IMPLEMENTED = "not implemented";
const char* const DS_ERROR_THREAD = "thread error";

} // namespace ds

#endif

