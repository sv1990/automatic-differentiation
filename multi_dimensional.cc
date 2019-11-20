#include "ad.hh"

#include <iomanip>
#include <iostream>

int main() {
  const auto x = ad::_0;
  const auto y = ad::_1;

  const auto f = x * y;

  std::cout << f(1, 1) << '\n';
  std::cout << f(1, 2) << '\n';

  const auto dxf = f.derive<0>();
  const auto dyf = f.derive<1>();

  std::cout << dxf(1, 0) << ' ' << dxf(0, 1) << '\n';
  std::cout << dyf(1, 0) << ' ' << dyf(0, 1) << '\n';
}
