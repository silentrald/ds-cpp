/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *===============================*/

#ifndef DS_THREAD_SEMAPHORE_HPP
#define DS_THREAD_SEMAPHORE_HPP

#include "ds/error.hpp"
#include "ds/types.hpp"
#include <semaphore.h>

namespace ds {

class semaphore {
private:
  sem_t sem{};

public:
  semaphore() noexcept = default;
  semaphore(const semaphore&) noexcept = delete;
  semaphore& operator=(const semaphore&) noexcept = delete;
  semaphore(semaphore&&) noexcept = delete;
  semaphore& operator=(semaphore&&) noexcept = delete;

  // === Constructor === //

  opt_err init(u32 max) noexcept;

  ~semaphore() noexcept;

  // === Functions === //

  opt_err acquire() noexcept;
  void release() noexcept;
};

} // namespace ds

#endif

