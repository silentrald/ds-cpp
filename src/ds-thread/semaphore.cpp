/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *===============================*/

#include "./semaphore.hpp"
#include "ds/error.hpp"
#include "ds/macro.hpp"
#include "ds/optional.hpp"
#include <semaphore.h>

namespace ds {

opt_err semaphore::init(u32 max) noexcept {
  if (sem_init(&this->sem, 0, max) != 0) {
    return error{"Could not create semaphore", def_err_vals};
  }
  return null;
}

semaphore::~semaphore() noexcept {
  sem_destroy(&this->sem);
}

opt_err semaphore::acquire() noexcept {
  if (sem_wait(&this->sem) != 0) {
    return error{"Could not acquire semaphore", def_err_vals};
  }
  return null;
}

void semaphore::release() noexcept {
  sem_post(&this->sem);
}

} // namespace ds

