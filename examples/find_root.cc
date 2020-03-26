#include "ad/ad.hh"

#include <iostream>

// Fond root using Newton method https://en.wikipedia.org/wiki/Newton%27s_method

template <typename F>
constexpr double find_root(F f, double start = 1.0, double eps = 1e-6) {
  const auto df = f.derive();
  double next   = start - f(start) / df(start);
  while (std::abs(start - next) / start > eps) {
    start = next;
    next  = start - f(start) / df(start);
  }
  return 0.5 * (start + next);
}

int main() {
  using namespace ad::literals;
  const auto x = ad::_0;

  const auto f = ad::pow(x - 2_c, 2_c) * ad::sqrt(x);
  std::cout << find_root(f) << '\n';

  const auto g = ad::cos(x);
  std::cout << 2 * find_root(g) << '\n';
}
