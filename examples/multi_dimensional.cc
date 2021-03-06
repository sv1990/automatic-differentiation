#include "ad/ad.hh"

#include <iomanip>
#include <iostream>

int main() {
  const auto x = ad::_0;
  const auto y = ad::_1;

  const auto f = x * x * y;

  const auto dxf = f.derive(x);
  const auto dyf = f.derive(y);

  [[maybe_unused]] const auto dxyf = f.derive(x, y);

  int colwidth = 3;
  for (double x = -5; x <= 5; x += 1) {
    for (double y = -5; y <= 5; y += 1) {
      std::cout << '(' //
                << std::setw(colwidth) << std::right << dxf(x, y) << ", "
                << std::setw(colwidth) << std::right << dyf(x, y) << ") ";
    }
    std::cout << '\n';
  }
}
