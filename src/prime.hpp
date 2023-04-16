/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-26
 *===============================*/

#ifndef DS_PRIME_HPP
#define DS_PRIME_HPP

#include "ds/types.hpp"
#include <climits>

namespace ds {

// NOLINTNEXTLINE
static const i32 primes_i32[] = {
    11,        23,         47,       97,        199,       //
    409,       823,        1741,     3469,      6949,      //
    14033,     28411,      57557,    116731,    236897,    //
    480881,    976369,     1982627,  4026031,   8175383,   //
    16601593,  33712729,   68460391, 139022417, 282312799, //
    573292817, 1164186217, INT_MAX};

i32 next_greater_prime_i32(i32 n) noexcept;

} // namespace ds

#endif

