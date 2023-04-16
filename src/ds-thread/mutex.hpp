/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *===============================*/

#ifndef DS_THREAD_MUTEX_HPP
#define DS_THREAD_MUTEX_HPP

#include "ds-error/types.hpp"
#include <pthread.h>

// Linux threading

namespace ds {

class mutex {
private:
  pthread_mutex_t _mutex{};

public:
  mutex() noexcept = default;
  mutex(const mutex&) noexcept = delete;
  mutex& operator=(const mutex&) noexcept = delete;
  mutex(mutex&&) noexcept = delete;
  mutex& operator=(mutex&&) noexcept = delete;

  // === Constructor === //

  opt_err init() noexcept;
  ~mutex() noexcept;

  // === Functions === //

  opt_err lock() noexcept;
  void try_lock() noexcept;
  void unlock() noexcept;
};

} // namespace ds

#endif

