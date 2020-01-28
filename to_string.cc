#include "to_string.hh"
#include "ad.hh"

#include <iostream>

int main() {
  using namespace ad::literals;
  const auto x = ad::_0;
  const auto f = (ad::sin(x) + 1) * (ad::pow(x, 2) - 1);
  std::cout << ad::to_string(f) << '\n';
}
