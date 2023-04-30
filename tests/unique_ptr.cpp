/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-06
 *===============================*/

#include "ds/unique_ptr.hpp"
#include "./main.hpp"
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "ds/shared_ptr.hpp"
#include <iostream>

using namespace ds_test;

class Base {
public:
  virtual ~Base() noexcept {
    std::cout << "Deleting Base\n";
  };
};

class Derived : public Base {
public:
  int i;

  ~Derived() noexcept override {
    std::cout << "Deleting Derived\n";
  }
};

// NOLINTNEXTLINE
TEST_CASE("unique_ptr", "ds") {
  ds::unique_ptr<Derived> derived{};
  ds::opt_err err = derived.init();
  derived->i = 100;

  ds::unique_ptr<Base> base = std::move(derived);
  std::cout << "Derived: " << base.get_ref_unsafe<Derived>().i << '\n';
}

