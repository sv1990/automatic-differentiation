#include "ad/ostream.hh"
#include "ad/ad.hh"

#include <iostream>

template <>
struct ad::format_variable<0> {
  static inline const std::string rep = "x";
};

int main() {
  using namespace ad::literals;
  const auto x = ad::_0;
  const auto f = (-ad::sin(2 * ad::pi * x) + 1_c) * -(ad::pow(x, 2_c) - 1_c);
  std::cout << "f (" << x << ") = " << f << '\n';
  std::cout << "f'(" << x << ") = " << f.derive() << '\n';
}
