/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-26
 *===============================*/

#ifndef DS_PRIME_HPP
#define DS_PRIME_HPP

#include "./types.hpp"
#include <climits>

namespace ds {

// For container use only, not a prime checker

// NOLINTNEXTLINE
inline const ds::usize primes[] = {
    11,        23,         47,           97,        199,       //
    409,       823,        1741,         3469,      6949,      //
    14033,     28411,      57557,        116731,    236897,    //
    480881,    976369,     1982627,      4026031,   8175383,   //
    16601593,  33712729,   68460391,     139022417, 282312799, //
    573292817, 1164186217, ds::USIZE_MAX
};

inline ds::usize get_first_prime() noexcept {
  return primes[0];
}

inline ds::usize next_greater_prime(ds::usize num) noexcept {
  // NOLINTNEXTLINE
  for (i32 i = 0; i < 27; ++i) {
    if (num < primes[i]) {
      return primes[i];
    }
  }

  return primes[27];
}

} // namespace ds

#endif
