#include "ad/to_string.hh"
#include "ad/ad.hh"

#include <iostream>

namespace ad {
template <>
struct format_variable<variable<0>> {
  static inline std::string rep = "x";
};
} // namespace ad

namespace ad { namespace detail {
template <typename E, std::enable_if_t<is_expression_v<E>>* = nullptr>
std::ostream& operator<<(std::ostream& os, const E& e) {
  return os << to_string(e);
}
}} // namespace ad::detail

int main() {
  using namespace ad::literals;
  const auto x = ad::_0;
  const auto f = (-ad::sin(x) + 1_c) * -(ad::pow(x, 2_c) - 1_c);
  std::cout << "f (" << x << ") = " << f << '\n';
  std::cout << "f'(" << x << ") = " << f.derive() << '\n';
}
