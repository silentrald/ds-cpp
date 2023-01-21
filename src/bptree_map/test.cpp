/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-xx-xx
 * Updated: 2023-xx-xx
 *===============================*/

#include "../types.hpp"
#include "./def.hpp"

int main() {
  ds::bptree_map<ds::i32, ds::i32> map{};

  auto error = map.insert(100, 200);
  error = map.insert(200, 200);

  for (auto it = map.cbegin(); it != map.cend(); ++it) {}

  return 0;
}

