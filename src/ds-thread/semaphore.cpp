/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *===============================*/

#include "./semaphore.hpp"
#include "ds/macro.hpp"
#include "ds/optional.hpp"
#include "ds/types.hpp"
#include <semaphore.h>

namespace ds {

err_code semaphore::init(u32 max) noexcept {
  if (sem_init(&this->sem, 0, max) != 0) {
    return ec::THREAD_ERR;
  }
  return ec::SUCCESS;
}

semaphore::~semaphore() noexcept {
  sem_destroy(&this->sem);
}

err_code semaphore::acquire() noexcept {
  if (sem_wait(&this->sem) != 0) {
    return ec::THREAD_ERR;
  }
  return ec::SUCCESS;
}

void semaphore::release() noexcept {
  sem_post(&this->sem);
}

} // namespace ds

