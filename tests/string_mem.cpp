/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2024-11-12
 *===============================*/

// NOTE: Mainly for valgrind checking

#include "ds/string.hpp"
#include "ds/types.hpp"
#include <cstdio>
#include <cstring>

int main() {
  ds::error_code error{};
  {
    ds::string str{};
    const ds::c8* const WORD = "Hello World";
    error = str.copy(WORD);
    if (ds::is_error(error)) {
      printf("Can't copy string: %u\n", error);
      return -1;
    }

    if (str.get_size() != strlen(WORD)) {
      printf(
          "String sizes don't match: " USIZE_FORMAT " != %lu\n", str.get_size(),
          strlen(WORD)
      );
      return -1;
    }

    if (!(str == WORD) || str != WORD) {
      printf("String comparison is incorrect\n");
      return -1;
    }
  }

  {
    ds::string str{};
    error = str.copy("hello world", 4);
    if (ds::is_error(error)) {
      printf("Can't copy string: %u\n", error);
      return -1;
    }

    if (str.get_size() != 4) {
      printf(
          "String size is incorrect: " USIZE_FORMAT " != 4\n", str.get_size()
      );
      return -1;
    }

    if (!(str == "hell") || str != "hell") {
      printf("String comparison is incorrect\n");
      return -1;
    }
  }
}
