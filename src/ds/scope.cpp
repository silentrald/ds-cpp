/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-09
 *===============================*/

#include "./scope.hpp"

namespace ds {

// === Destructor === //
scope::~scope() noexcept {
  this->rollback();
}

// === Handling === //
void scope::rollback() noexcept {
  for (auto& cb : this->rollbacks) {
    cb();
  }
  this->rollbacks.clear();
}

opt_err scope::push_fail(function<void()>&& callback) noexcept {
  auto error = this->rollbacks.push_back(nullptr);
  if (error) {
    callback();
    this->rollback();
    return error;
  }

  // Should not throw an error
  this->rollbacks[this->rollbacks.size() - 1] = std::move(callback);
  return null;
}

void scope::commit() noexcept {
  this->rollbacks.clear();
}

} // namespace ds

