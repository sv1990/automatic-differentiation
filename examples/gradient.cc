#include "ad/gradient.hh"

#include <iostream>

template <>
struct ad::format_variable<ad::variable<0>> {
  static inline std::string rep = "x";
};
template <>
struct ad::format_variable<ad::variable<1>> {
  static inline std::string rep = "y";
};
template <>
struct ad::format_variable<ad::variable<2>> {
  static inline std::string rep = "z";
};

int main() {
  constexpr auto x = ad::_0;

  [[maybe_unused]] constexpr auto y = ad::_1;

  constexpr auto z = ad::_2;

  constexpr auto f  = ad::sin(x) * ad::exp(z);
  const auto grad_f = ad::gradient(f);
  std::cout << grad_f << '\n';
}
