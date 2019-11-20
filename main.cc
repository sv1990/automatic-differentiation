#include "ad.hh"

#include <iomanip>
#include <iostream>

int main() {
  using namespace ad::literals;
  using ad::var::x;

  const auto f   = x * (x + 1_c);
  const auto df  = f.derive();
  const auto d2f = df.derive();

  const int colwidth = 7;
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
}
