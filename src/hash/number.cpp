/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.1
 * Created: 2022-12-27
 * Updated: 2024-10-01
 *   - use trivial copy instead
 *===============================*/

#include "ds/hash.hpp"

namespace ds {

/**
 * Might be helpful in the future
 *
 * Link:
 * https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
 **/

template <> i32 hash<i32>::operator()(i32 data) const noexcept {
  return data;
}

template <> i32 hash<u32>::operator()(u32 data) const noexcept {
  return data;
}

template <> i32 hash<i64>::operator()(i64 data) const noexcept {
  return data;
}

template <> i32 hash<u64>::operator()(u64 data) const noexcept {
  return data;
}

template <> i32 hash<void*>::operator()(void* data) const noexcept {
  return (i64)data;
}

} // namespace ds

