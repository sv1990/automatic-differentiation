#include "ad.hh"

#include <iomanip>
#include <iostream>

int main() {
  using namespace ad::literals;
  using ad::var::x;

  const auto f   = (x + 1) * (x - 1);
  const auto df  = f.derive();
  const auto d2f = df.derive();

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

  const auto g   = 1 / x;
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
