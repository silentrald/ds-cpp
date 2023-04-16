/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2022-12-08
 *===============================*/

#ifndef DS_FIXED_DEQUE_HPP
#define DS_FIXED_DEQUE_HPP

#include "./fixed_deque_iterator.hpp"
#include "ds-error/types.hpp"
#include "ds/macro.hpp"
#include "ds/type_traits.hpp"
#include "ds/types.hpp"
#include <cstdlib>

namespace ds {

const char* const FDEQUE_BAD_ALLOC = "fixed_deque:bad_alloc";
const char* const FDEQUE_OUT_OF_RANGE = "fixed_deque:out_of_range";
const char* const FDEQUE_EMPTY = "fixed_deque:empty";
const char* const FDEQUE_FULL = "fixed_deque:full";
const char* const FDEQUE_NEGATIVE_CAP = "fixed_deque:negative_capacity";
const char* const FDEQUE_REMOVE = "fixed_deque:too_many_to_remove";

/**
 * A deque but does not grow in pushing back/front
 * Can still be resized but the capacity will be fixed when pushing
 *
 * This uses a circular array with n + 1 size, +1 is a sentinel value that can
 * be used for the iterator
 *
 * Legend:
 * x -> data
 * H -> head
 * T -> tail
 * _ -> empty
 **/
template <typename T> class fixed_deque {
public:
  using value = T;
  using ref = T&;
  using cref = const T&;
  using rref = T&&;
  using pointer = T*;

  using iterator = fixed_deque_iterator<fixed_deque<T>>;
  using citerator = fixed_deque_iterator<const fixed_deque<T>>;

  void destroy() noexcept;

private:
  pointer arr = nullptr;
  i32 head = -1;
  i32 tail = 0;
  i32 _capacity = 0;

  // === Memory ===
  /**
   * Allocates a memory block for the fixed deque
   *
   * @errors
   *  - Could not allocate the size
   **/
  [[nodiscard]] opt_err allocate(i32 capacity) noexcept;

  /**
   * Reallocates the fixed deque
   *
   * @errors
   *  - Could not resize to the new size
   *  - Init / Copy of custom classes
   **/
  [[nodiscard]] opt_err reallocate(i32 capacity) noexcept;

  // * Helper functions for reallocate *
  /**
   * Reallocating by just increasing the size of the deque
   **/
  [[nodiscard]] opt_err reallocate_grow(i32 capacity) noexcept;

  /**
   * Reallocates by shifting the data
   *
   * Visualization:
   *  +------+    +------+
   *  |__HxT_| -> |HxT___| -> Resizing Process
   *  +------+    +------+
   **/
  [[nodiscard]] opt_err reallocate_shift(i32 capacity) noexcept;

  /**
   * Reallocates by moving the data to a new container
   *
   * Visualization:
   *  +-----+    +---------+
   *  |xT_Hx| -> |HxxT_____|
   *  +-----+    +---------+
   *
   *  +-------+    +-----+
   *  |xT___Hx| -> |HxxT_|
   *  +-------+    +-----+
   **/
  [[nodiscard]] opt_err reallocate_move(i32 capacity) noexcept;

  // === Copy Helper ===
  /**
   * Tries to copy the data from the center to this fdeque
   *
   * State: head <= tails
   *  +-----------+
   *  |__HxxxxT___|
   *  +-----------+
   **/
  [[nodiscard]] opt_err copy_center(const fixed_deque& rhs) noexcept;

  /**
   * Tries to copy the data from both ends to this fdeque
   *
   * State: head > tails
   *  +-----------+
   *  |xxT____Hxxx|
   *  +-----------+
   **/
  [[nodiscard]] opt_err copy_ends(const fixed_deque& rhs) noexcept;

public:
  fixed_deque() noexcept = default;
  fixed_deque(const fixed_deque&) = delete;
  fixed_deque& operator=(const fixed_deque&) = delete;

  // === Copy === //
  /**
   * Copies the passed deque object
   *
   * @errors
   *   - Bad allocation
   **/
  [[nodiscard]] opt_err copy(const fixed_deque& other) noexcept;

  // === Move === //
  fixed_deque(fixed_deque&& rhs) noexcept;
  fixed_deque& operator=(fixed_deque&& rhs) noexcept;

  // === Destructor === //
  ~fixed_deque() noexcept;

  // === Element Access === //
  /**
   * Safe index accessing
   *
   * @errors
   *  - Deque is empty
   *  - Index out of bounds
   *
   * @return exp_ptr_err<value>
   **/
  [[nodiscard]] exp_ptr_err<T> at(i32 index) noexcept;

  /**
   * Unsafe index accessing
   *
   * @return ref
   **/
  ref operator[](i32 index) noexcept;

  /**
   * Returns the front of the fdeque
   *
   * @errors
   *  - Deque is empty
   *
   * @return exp_ptr_err<value>
   **/
  [[nodiscard]] exp_ptr_err<T> front() noexcept;

  /**
   * Returns the back of the fdeque
   *
   * @errors
   *  - Deque is empty
   *
   * @return exp_ptr_err<value>
   **/
  [[nodiscard]] exp_ptr_err<T> back() noexcept;

  // === Iterators === //

  /**
   * This points to the head
   *
   * Visualization:
   *  +-------+  +------+
   *  |xxT_Hxx|  |HxxxT_|
   *  +-------+  +------+
   *       ^      ^
   **/
  [[nodiscard]] iterator begin() noexcept;

  /**
   * This points to the head
   *
   * Visualization:
   *  +-------+  +------+
   *  |xxT_Hxx|  |HxxxT_|
   *  +-------+  +------+
   *       ^      ^
   **/
  [[nodiscard]] citerator cbegin() const noexcept;

  /**
   * This points to tail + 1
   *
   * There is always one empty space so that an iterator can be used.
   *
   * Visualization:
   *  +-------+  +------+
   *  |xxT_Hxx|  |HxxxT_|
   *  +-------+  +------+
   *     ^            ^
   **/
  [[nodiscard]] iterator end() noexcept;

  /**
   * This points to tail + 1
   *
   * There is always one empty space so that an iterator can be used.
   *
   * Visualization:
   *  +-------+  +------+
   *  |xxT_Hxx|  |HxxxT_|
   *  +-------+  +------+
   *     ^            ^
   **/
  [[nodiscard]] citerator cend() const noexcept;

  // === Capacity === //
  /**
   * Checks if the fdeque is empty
   *
   * The general state is shown below:
   * +------+
   * |______| H = -1 ; T = 0
   * +------+
   */
  [[nodiscard]] bool is_empty() const noexcept;

  /**
   * Checks if the fdeque is full
   *
   * Sample States:
   * +------+ +------+ +------+
   * |HxxxT_| |_HxxxT| |xxH_Tx|
   * +------+ +------+ +------+
   * Rule: At least 1 space is not occupied
   */
  [[nodiscard]] bool is_full() const noexcept;

  /**
   * Returns the current size of the fdeque
   *
   * @returns i32
   **/
  [[nodiscard]] i32 size() const noexcept;

  /**
   * Returns the storage capacity of the fdeque
   *
   * @returns i32
   **/
  [[nodiscard]] i32 capacity() const noexcept;

  // === Modifiers === //
  /**
   * Clears the fdeque
   **/
  void clear() noexcept;

  /**
   * Pushes data to the front of the fdeque
   *
   * @errors
   *  - Deque is full
   */
  [[nodiscard]] opt_err push_front(rref element) noexcept;

  /**
   * Pushes data to the front of the fdeque
   *
   * @errors
   *  - Deque is full
   *  - Bad allocation
   */
  [[nodiscard]] opt_err push_front(cref element) noexcept;

  /**
   * Pushes data to the back of the fdeque
   *
   * @errors
   *  - Deque is full
   */
  [[nodiscard]] opt_err push_back(rref element) noexcept;

  /**
   * Pushes data to the back of the fdeque
   *
   * @errors
   *  - Deque is full
   *  - Bad allocation
   */
  [[nodiscard]] opt_err push_back(cref element) noexcept;

  /**
   * Pops data out of the fdeque from the front
   *
   * @errors
   *  - Deque is empty
   */
  [[nodiscard]] exp_err<T> pop_front() noexcept;

  /**
   * Pops data out of the fdeque from the front but discards the result
   *
   * @errors
   *  - Deque is empty
   **/
  [[nodiscard]] opt_err pop_front_disc() noexcept;

  /**
   * Pops data out of the fdeque from the back
   *
   * @errors
   *  - Deque is empty
   */
  [[nodiscard]] exp_err<T> pop_back() noexcept;

  /**
   * Pops data out of the fdeque from the back but discards the element
   *
   * @errors
   *  - Deque is empty
   **/
  [[nodiscard]] opt_err pop_back_disc() noexcept;

  /**
   * Removes the first n elements of the fdeque
   *
   * Errors
   *  - Deque is empty
   *  - Negative n
   *  - n is bigger than the size
   **/
  [[nodiscard]] opt_err remove_front(i32 n) noexcept;

  /**
   * Removes the last n elements of the fdeque
   *
   * Errors
   *  - Deque is empty
   *  - Negative n
   *  - n is bigger than the size
   **/
  [[nodiscard]] opt_err remove_back(i32 n) noexcept;

  /**
   * Resizes the fdeque
   *
   * @errors
   *  - Bad allocation
   **/
  [[nodiscard]] opt_err resize(i32 size) noexcept;
};

} // namespace ds

#ifndef DS_FIXED_DEQUE_TPP
#include "./fixed_deque.tpp"
#endif

#endif

