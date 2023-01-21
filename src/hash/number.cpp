/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 * Updated: 2022-12-27
 *===============================*/

#include "./def.hpp"

namespace ds {

/**
 * Link:
 * https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
 **/
template <>
i32 hash<i32>::operator()(i32 data) const noexcept {
  data = ((data >> 16) ^ data) * 0x119de1f3;
  data = ((data >> 16) ^ data) * 0x119de1f3;
  data = (data >> 16) ^ data;
  return data;
}

/**
 * Link:
 * https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
 **/
template <>
i32 hash<u32>::operator()(u32 data) const noexcept {
  data = ((data >> 16) ^ data) * 0x119de1f3;
  data = ((data >> 16) ^ data) * 0x119de1f3;
  data = (data >> 16) ^ data;
  return data;
}

/**
 * Link:
 * https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
 **/
template <>
i32 hash<i64>::operator()(i64 data) const noexcept {
  data = (data ^ (data >> 30)) * INT64_C(0xbf58476d1ce4e5b9);
  data = (data ^ (data >> 27)) * INT64_C(0x94d049bb133111eb);
  data = data ^ (data >> 31);
  return data;
}

/**
 * Link:
 * https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
 **/
template <>
i32 hash<u64>::operator()(u64 data) const noexcept {
  data = (data ^ (data >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  data = (data ^ (data >> 27)) * UINT64_C(0x94d049bb133111eb);
  data = data ^ (data >> 31);
  return data;
}

template <>
i32 hash<void*>::operator()(void *data) const noexcept {
  u64 value = (u64)data;
  value = (value ^ (value >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  value = (value ^ (value >> 27)) * UINT64_C(0x94d049bb133111eb);
  value = value ^ (value >> 31);
  return value;
}

} // namespace ds

