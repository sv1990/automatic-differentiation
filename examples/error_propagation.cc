#include "ad/ad.hh"

#include <iostream>
#include <numeric>

#include <cmath>

struct udouble {
  double val;
  double stddev;
  operator double() const { return val; }
};
std::ostream& operator<<(std::ostream& os, const udouble& ud) {
  return os << ud.val << " +/- " << ud.stddev;
}

template <typename E, typename... Ts, std::size_t... Is>
std::array<double, sizeof...(Is)>
take_derivatives(E e, std::index_sequence<Is...>, Ts... xs) {
  return {e.template derive<Is>()(xs...)...};
}

template <typename Func,
          std::enable_if_t<ad::detail::is_expression_v<Func>>* = nullptr>
auto std_dev(Func func) {
  return [func](auto... xs) {
    std::array derivatives = take_derivatives(
        func, std::make_index_sequence<sizeof...(xs)>{}, xs.val...);
    std::array stddevs{xs.stddev...};
    return std::sqrt(std::inner_product(
        begin(derivatives), end(derivatives), begin(stddevs), 0.0, std::plus{},
        [](double l, double r) { return l * l * r * r; }));
  };
}

template <typename Func,
          std::enable_if_t<ad::detail::is_expression_v<Func>>* = nullptr>
auto make_ufunction(Func func) {
  return [func, ufunc = std_dev(func)](auto... xs) {
    return udouble{func(xs...), ufunc(xs...)};
  };
}

int main() {
  using namespace ad::literals;
  const auto x  = ad::_0;
  const auto y  = ad::_1;
  const auto f  = x * y;

  const auto uf = make_ufunction(f);
  udouble ux{1, 0.1};
  udouble uy{1, 0.5};

  std::cout << uf(ux, uy) << '\n';
}
