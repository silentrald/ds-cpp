/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-27
 *===============================*/

#ifndef DS_HASH_HPP
#define DS_HASH_HPP

#include "../hash/murmur3.h"
#include "./string.hpp"
#include "./type_traits.hpp"
#include "./types.hpp"
#include <cstring>

namespace ds {

const int SEED = 69420;

template <
    typename Derived, typename T, typename Return = i32,
    typename = is_int<Return>>
class base_hash {
public:
  friend Derived;
};

template <typename T, typename Return = i32>
class hash : public base_hash<hash<T, Return>, T, Return> {
public:
  using hash_return = Return;

  Return operator()(T data) const noexcept;
};

template <> class hash<string> : public base_hash<hash<string>, string> {
public:
  using hash_return = i32;

  i32 operator()(const string& data) const noexcept {
    i32 out = 0;
    MurmurHash3_x86_32(data.c_str(), data.length(), SEED, &out);
    return out;
  }

  i32 operator()(const char* data) const noexcept {
    i32 out = 0;
    MurmurHash3_x86_32(data, std::strlen(data), SEED, &out);
    return out;
  }
};

} // namespace ds

#endif

