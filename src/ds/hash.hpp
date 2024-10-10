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
#include "types.hpp"
#include <cstring>

namespace ds {

// TODO: Configurable in cmake
const i32 SEED = 69'420;

template <typename T> class hash {
public:
  usize operator()(T data) const noexcept {
    return (usize)data;
  }
};

template <> class hash<string> {
public:
  usize operator()(const string& data) const noexcept {
    usize out = 0U;
    MurmurHash3_x86_32(data.c_str(), data.get_size(), SEED, &out);
    return out;
  }

  usize operator()(const c8* data) const noexcept {
    usize out = 0U;
    MurmurHash3_x86_32(data, std::strlen(data), SEED, &out);
    return out;
  }
};

} // namespace ds

#endif
