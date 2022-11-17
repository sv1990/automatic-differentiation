#include "ad/ad.hh"

#include <iomanip>
#include <iostream>

int main() {
  using namespace ad::literals;
  constexpr auto x = ad::_0;

  const auto f   = (x + 1_c) * (x - 1_c);
  const auto df  = f.derive();
  const auto d2f = df.derive();

  static_assert(sizeof(f) < sizeof((x + 1) * (x - 1)));
  static_assert(std::is_same_v<
                std::decay_t<decltype(d2f)>,
                std::decay_t<decltype(2_c)>>);

  int colwidth = 7;
  std::cout << std::setw(colwidth) << std::right << "x" << ' '     //
            << std::setw(colwidth) << std::right << "f(x)" << ' '  //
            << std::setw(colwidth) << std::right << "df(x)" << ' ' //
            << std::setw(colwidth) << std::right << "d2f(x)" << '\n';
  std::cout << std::setw(colwidth) << std::right << "size" << ' '     //
            << std::setw(colwidth) << std::right << sizeof(f) << ' '  //
            << std::setw(colwidth) << std::right << sizeof(df) << ' ' //
            << std::setw(colwidth) << std::right << sizeof(d2f) << '\n';
  for (double x = 0; x <= 10; x += 1) {
    std::cout << std::setw(colwidth) << std::right << x << ' '     //
              << std::setw(colwidth) << std::right << f(x) << ' '  //
              << std::setw(colwidth) << std::right << df(x) << ' ' //
              << std::setw(colwidth) << std::right << d2f(x) << '\n';
  }

  const auto g   = ad::log(x);
  const auto dg  = g.derive();
  const auto d2g = dg.derive();

  colwidth = 14;
  std::cout << std::setw(colwidth) << std::right << "x" << ' '     //
            << std::setw(colwidth) << std::right << "g(x)" << ' '  //
            << std::setw(colwidth) << std::right << "dg(x)" << ' ' //
            << std::setw(colwidth) << std::right << "d2g(x)" << '\n';
  std::cout << std::setw(colwidth) << std::right << "size" << ' '     //
            << std::setw(colwidth) << std::right << sizeof(g) << ' '  //
            << std::setw(colwidth) << std::right << sizeof(dg) << ' ' //
            << std::setw(colwidth) << std::right << sizeof(d2g) << '\n';
  for (double x = 1; x <= 10; x += 1) {
    std::cout << std::setw(colwidth) << std::right << x << ' '     //
              << std::setw(colwidth) << std::right << g(x) << ' '  //
              << std::setw(colwidth) << std::right << dg(x) << ' ' //
              << std::setw(colwidth) << std::right << d2g(x) << '\n';
  }
}
