/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *===============================*/

#include "./mutex.hpp"
#include "ds-error/error.hpp"
#include "ds/macro.hpp"
#include "ds/optional.hpp"
#include "ds/types.hpp"
#include <pthread.h>
#include <utility>

namespace ds {

mutex::~mutex() noexcept {
  // TODO: Not safe to destroy if locked
  pthread_mutex_destroy(&this->_mutex);
}

opt_err mutex::lock() noexcept {
  if (pthread_mutex_lock(&this->_mutex) != 0) {
    return error{DS_ERROR_THREAD, def_err_vals};
  }
  return null;
}

void mutex::try_lock() noexcept {
  pthread_mutex_trylock(&this->_mutex); // NOLINT
}

void mutex::unlock() noexcept {
  pthread_mutex_unlock(&this->_mutex);
}

} // namespace ds

