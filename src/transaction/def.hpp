/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-01-09
 * Updated: 2023-01-09
 *===============================*/

#ifndef DS_TRANSACTION_DEF_HPP
#define DS_TRANSACTION_DEF_HPP

#include "../error.hpp"
#include "../types.hpp"
#include "../vector/def.hpp"

namespace ds {

/**
 * A class that handles atomic operations.
 * If the function succeeds, then the commit function needs to be called
 * to clear the rollbacks vector,
 * Else if the function fails, the destructor will call all the callback
 * in the rollbacks vector.
 **/
class transaction {
private:
  vector<function<void()>> rollbacks{};

  // === Handling === //

  /**
   * Executes all the callbacks pushed into the rollbacks vector.
   * If the transaction::commit() was called, then this will do
   * nothing.
   **/
  void rollback() noexcept;

public:
  transaction() noexcept = default;
  transaction(const transaction&) = delete;
  transaction& operator=(const transaction&) = delete;

  // === Move === //

  transaction(transaction&&) noexcept = default;
  transaction& operator=(transaction&&) noexcept = default;

  // === Destructor === //

  ~transaction() noexcept;

  // === Handling === //

  /**
   * Pushes a callback. This will be only called if the function fails
   * to call transaction::commit().
   *
   * @errors
   *  - bad allocation in resizing the rollback vector
   **/
  opt_err push_fail(function<void()>&& callback) noexcept;

  /**
   * Successful transaction. This clears the rollbacks vector.
   **/
  void commit() noexcept;
};

} // namespace ds

#endif

