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

#if _WIN32 || _WIN64
#if _WIN64
#define BIT_64
#else
#define BIT_64
#endif
#endif

#if __GNUC__
#if __x86_64__ || __ppc64__
#define BIT_64
#else
#define BIT_32
#endif
#endif

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

template <typename Signature> using function = std::function<Signature>;

} // namespace ds

#endif

