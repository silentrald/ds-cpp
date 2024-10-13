/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-10-13
 *===============================*/

#ifndef DS_ALLOCATOR_HPP
#define DS_ALLOCATOR_HPP

#include "ds/types.hpp"
#include <cstdlib>

namespace ds {

template <typename T> struct allocator {
  [[nodiscard]] inline void* allocate(usize size) const noexcept {
    return std::malloc(size); // NOLINT
  }

  [[nodiscard]] inline void*
  reallocate(void* pointer, usize size) const noexcept {
    return std::realloc(pointer, size); // NOLINT
  }

  inline void deallocate(void* pointer) const noexcept {
    std::free(pointer); // NOLINT
  }
};

} // namespace ds

#endif
