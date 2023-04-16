/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-16
 *===============================*/

#include "ds-error/error.hpp"
#include "ds-error/expected.hpp"
#include "ds/optional.hpp"

namespace ds {

using opt_err = optional<error>;
template <typename T> using exp_err = expected<T, error>;
template <typename T> using exp_ptr_err = expected_ptr<T, error>;

} // namespace ds

