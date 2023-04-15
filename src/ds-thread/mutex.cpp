/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *===============================*/

#include "./mutex.hpp"
#include "ds/error.hpp"
#include "ds/macro.hpp"
#include "ds/optional.hpp"
#include <pthread.h>
#include <utility>

namespace ds {

opt_err mutex::init() noexcept {
  // TODO: Can use the error code to make finer error msg
  if (pthread_mutex_init(&this->_mutex, nullptr) != 0) {
    return error{"Could not create mutex", def_err_vals};
  }
  return null;
}

mutex::~mutex() noexcept {
  // TODO: Not safe to destroy if locked
  pthread_mutex_destroy(&this->_mutex);
}

opt_err mutex::lock() noexcept {
  if (pthread_mutex_lock(&this->_mutex) != 0) {
    return error{"Lock error", def_err_vals};
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

