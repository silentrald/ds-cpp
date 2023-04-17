/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_TYPES_HPP
#define DS_TYPES_HPP

#include "ds-error/expected.hpp"
#include <cstdint>
#include <functional>

namespace ds {

using u8 = std::uint8_t;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = std::int8_t;
using i16 = short;
using i32 = int;
using i64 = long long;

using f32 = float;
using f64 = double;

using err_code = i32;
template <typename T> using exp_err_code = expected<T, i32>;
template <typename T> using exp_ptr_err_code = expected_ptr<T, i32>;

template <typename Signature> using function = std::function<Signature>;

namespace ec {

const i32 SUCCESS = 0;

const i32 NOT_IMPLEMENTED = -1;

const i32 BAD_ALLOC = 1;
const i32 EMPTY = 2;
const i32 FULL = 3;
const i32 OUT_OF_RANGE = 4;
const i32 NOT_FOUND = 5;
const i32 ALREADY_SET = 6;
const i32 SIZE = 7;
const i32 THREAD_ERR = 8;

} // namespace ec

} // namespace ds

#endif

