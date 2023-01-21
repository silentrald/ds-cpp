
#include "./unique_ptr/def.hpp"
#include "error.hpp"
#include "macro.hpp"
#include "optional/def.hpp"
#include <iostream>

ds::opt_error wrapper() {
  ds::unique_ptr<int> ptr{};
  try_opt(ptr.set(100));
  ptr.reset();
  try_opt(ptr.set(1));

  if (ptr) {
    std::cout << *ptr << '\n';
  } else {
    std::cout << "empty\n";
  }

  return ds::null;
}

int main() {
  wrapper();
  return 0;
}
