/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-12-02
 *===============================*/

#ifndef DS_SORT_HPP
#define DS_SORT_HPP

#include "./compare.hpp"
#include "./types.hpp"
#include "./vector.hpp"
#include <algorithm>
#include <utility>

namespace ds {

template <typename T, typename Compare>
void bubble_sort(T* array, usize size) noexcept;

namespace { // NOLINT

template <typename T, typename Compare = compare<T>>
void _merge_once(
    T* destination, T* source, const usize size, usize middle
) noexcept {
  isize comparison = 0;
  usize index = 0U;
  usize left = 0U;
  usize right = middle;

  while (left < middle && right < size) {
    comparison = Compare{}(source[left], source[right]);
    if (comparison <= 0) {
      destination[index++] = source[left++];
    }

    if (comparison >= 0) {
      destination[index++] = source[right++];
    }
  }

  for (; left < middle; ++left, ++index) {
    destination[index] = source[left];
  }

  for (; right < size; ++right, ++index) {
    destination[index] = source[right];
  }
}

template <typename T, typename Compare = compare<T>>
void _merge(T* destination, T* source, const usize size, usize batch) noexcept {
  usize sorted = batch >> 1;
  usize offset = 0;
  usize last_offset = 0;

  for (; offset + sorted < size; offset += batch) {
    last_offset += std::min(batch, size - offset);
    _merge_once(
        destination + offset, source + offset, std::min(batch, size - offset),
        sorted
    );
  }

  // Copy the remaining elements to the destination array
  if (last_offset < size) {
    for (; last_offset < size; ++last_offset) {
      destination[last_offset] = source[last_offset];
    }
  }
}

template <typename T, typename Compare = compare<T>>
bool _merge_sort_impl(
    T* array, const usize size, vector<T>&& auxillary
) noexcept {
  bool to_auxillary = true;

  // Force this to 4, 8, 16
  usize batch = std::max(4ULL, 64ULL / sizeof(T));
  batch |= batch >> 1;
  batch |= batch >> 2;
  batch |= batch >> 4;
  batch = (batch + 1) >> 1;

  for (usize index = 0; index < size; index += batch) {
    // TODO: Change to quicksort
    bubble_sort(array + index, std::min(size - index, batch));
  }

  for (batch <<= 1; batch < size; batch <<= 1) {
    if (to_auxillary) {
      _merge(auxillary.get_data(), array, size, batch);
    } else {
      _merge(array, auxillary.get_data(), size, batch);
    }
    to_auxillary = !to_auxillary;
  }

  if (batch < size) {
    return to_auxillary;
  }

  if (to_auxillary) {
    _merge_once(auxillary.get_data(), array, size, batch >> 1);
  } else {
    _merge_once(array, auxillary.get_data(), size, batch >> 1);
  }
  return !to_auxillary;
}

} // namespace

template <typename T, typename Compare = compare<T>>
[[nodiscard]] bool is_sorted(T* array, const usize size) noexcept {
  for (usize i = 1; i < size; ++i) {
    if (Compare{}(array[i], array[i - 1]) < 0) {
      return false;
    }
  }
  return true;
}

template <typename T, typename Compare = compare<T>>
void bubble_sort(T* array, const usize size) noexcept {
  bool swapped = false;

  for (usize i = 1; i < size; ++i) {
    for (usize j = 1; j <= size - i; ++j) {
      if (Compare{}(array[j], array[j - 1]) < 0) {
        std::swap(array[j], array[j - 1]);
        swapped = true;
      }
    }

    if (!swapped) {
      return;
    }
    swapped = false;
  }
}

template <typename T> error_code merge_sort(vector<T>& vec) noexcept {
  if (is_sorted(vec.get_data(), vec.get_size())) {
    return error::OK;
  }

  vector<T> auxillary{};
  TRY(auxillary.resize(vec.get_capacity()));

  if (!_merge_sort_impl(vec.get_data(), vec.get_size(), std::move(auxillary))) {
    vec = std::move(auxillary);
  }

  return error::OK;
}

template <typename T, typename Compare = compare<T>>
error_code merge_sort(T* array, const usize size) noexcept {
  if (is_sorted(array, size)) {
    return error::OK;
  }

  vector<T> auxillary{};
  TRY(auxillary.resize(size));

  if (!_merge_sort_impl(array, size, std::move(auxillary))) {
    for (usize i = 0; i < size; ++i) {
      array[i] = auxillary[i];
    }
  }

  return error::OK;
}

} // namespace ds

#endif
