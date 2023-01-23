/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-xx-xx
 * Updated: 2023-xx-xx
 *===============================*/

#include "ds/bptree_map.hpp"
#include "ds/error.hpp"
#include "ds/optional.hpp"
#include "ds/types.hpp"

struct Data {
  ds::i64 x = 0;
  ds::i64 y = 0;
  ds::i64 z = 0;
  ds::i64 w = 0;
};

int main() {
  ds::bptree_map<ds::i32, Data> map{};
  ds::opt_err error{};

  const ds::i32 end = 100;
  for (ds::i32 i = 0; i < end; ++i) {
    error = map.insert(i, {});
    if (error) {
      break;
    }
  }

  for (auto it = map.cbegin(); it != map.cend(); ++it) {
  }

  return 0;
}

