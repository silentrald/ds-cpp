/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-09
 *===============================*/

#ifndef DS_SCOPE_HPP
#define DS_SCOPE_HPP

#include "ds/types.hpp"
#include "ds/vector.hpp"

namespace ds {

/**
 * A class that handles atomic operations.
 * If the function succeeds, then the commit function needs to be called
 * to clear the rollbacks vector,
 * Else if the function fails, the destructor will call all the callback
 * in the rollbacks vector.
 **/
class scope {
private:
  vector<function<void()>> rollbacks{};

  // === Handling === //

  /**
   * Executes all the callbacks pushed into the rollbacks vector.
   * If the scope::commit() was called, then this will do
   * nothing.
   **/
  void rollback() noexcept;

public:
  scope() noexcept = default;
  scope(const scope&) = delete;
  scope& operator=(const scope&) = delete;

  // === Move === //

  scope(scope&&) noexcept = default;
  scope& operator=(scope&&) noexcept = default;

  // === Destructor === //

  ~scope() noexcept;

  // === Handling === //

  /**
   * Pushes a callback. This will be only called if the function fails
   * to call scope::commit().
   *
   * @errors
   *  - bad allocation in resizing the rollback vector
   **/
  opt_err push_fail(function<void()>&& callback) noexcept;

  /**
   * Successful scope. This clears the rollbacks vector.
   **/
  void commit() noexcept;
};

} // namespace ds

#endif

