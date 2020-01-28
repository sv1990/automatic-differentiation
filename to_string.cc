#include "to_string.hh"
#include "ad.hh"

#include <iostream>

int main() {
  using namespace ad::literals;
  const auto x = ad::_0;
  const auto f = (ad::sin(x) + 1_c) * (ad::pow(x, 2_c) - 1_c);
  std::cout << ad::to_string(f) << '\n';
  std::cout << ad::to_string(f.derive()) << '\n';
}