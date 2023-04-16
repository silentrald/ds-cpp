/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-26
 *===============================*/

#include "./prime.hpp"

namespace ds {

i32 next_greater_prime_i32(i32 n) noexcept {
  // NOLINTNEXTLINE
  for (i32 i = 0; i < 27; ++i) {
    if (n < primes_i32[i]) {
      return primes_i32[i];
    }
  }

  return primes_i32[27];
}

} // namespace ds
