#include "ad/ad.hh"
#include "ad/ostream.hh"

#include <cassert>

#undef NDEBUG

template <typename T, typename U>
constexpr bool same_type(T, U) noexcept {
  return std::is_same_v<T, U>;
}

template <typename T>
constexpr bool is_static_expression(T) {
  return ad::detail::is_static_v<T>;
}

int main() {
  using namespace ad::literals;
  constexpr auto x = ad::_0;
  constexpr auto y = ad::_1;

  static_assert(same_type(123_c, ad::static_constant<123>{}));

  static_assert(same_type(x.derive(x), 1_c));
  static_assert(same_type(x.derive(y), 0_c));
  static_assert(same_type(y.derive(x), 0_c));
  static_assert(same_type(y.derive(y), 1_c));

  static_assert(same_type(x.derive(x, x), 0_c));
  static_assert(same_type((x * y).derive(x, y), 1_c));

  static_assert(same_type(ad::log(x).derive(x), 1_c / x));
  static_assert(same_type(ad::exp(x).derive(x), ad::exp(x)));

  static_assert(same_type(ad::sin(x).derive(x), ad::cos(x)));
  static_assert(same_type(ad::sin(x).derive(x, x), -ad::sin(x)));
  static_assert(same_type(ad::sin(x).derive(x, x, x), -ad::cos(x)));
  static_assert(same_type(ad::sin(x).derive(x, x, x, x), ad::sin(x)));

  static_assert(
      same_type(ad::exp(ad::sin(x)).derive(x), ad::cos(x) * ad::exp(ad::sin(x)))
  );
  static_assert(same_type(
      (ad::exp(x) * ad::sin(x)).derive(x),
      ad::exp(x) * ad::sin(x) + ad::exp(x) * ad::cos(x)
  ));

  static_assert(same_type(-(-x * 1_c), x));
  static_assert(same_type(-0_c, 0_c));
  static_assert(same_type(ad::pow(1_c / x, -1_c), x));
  static_assert(same_type(ad::exp(1_c) * ad::exp(x), ad::exp(1_c + x)));

  static_assert(same_type(ad::pow(ad::exp(x), 2_c), ad::exp(x * 2_c)));
  static_assert(same_type(ad::pow(ad::pow(x, 2_c), 2_c), ad::pow(x, 4_c)));

#if __has_attribute(no_unique_address)
  static_assert(sizeof(ad::exp(x)) == 1);
  static_assert(sizeof(x + 1_c) == 1);
  static_assert(sizeof((x + 1_c) * (x - 1_c)) <= 2);
#endif

  assert(ad::to_string(1_c / (x * ad::exp(x))) == "1 / (x0 * exp(x0))");
  assert(ad::to_string(1_c - (x + 1_c)) == "1 - (x0 + 1)");
  assert(ad::to_string(1_c + (x + 1_c)) == "1 + x0 + 1");
  assert(ad::to_string(2_c * (x * 2_c)) == "2 * x0 * 2");
  assert(ad::to_string(-ad::exp(x)) == "-exp(x0)");
  assert(ad::to_string(-(x + 2)) == "-(x0 + 2)");
  assert(ad::to_string(-(x - 2)) == "-(x0 - 2)");

  static_assert(is_static_expression(x));
  static_assert(is_static_expression(x + 1_c));
  static_assert(!is_static_expression(x + 1));

  static_assert(same_type(ad::sin(x) - ad::sin(x), 0_c));
  static_assert(same_type(ad::sin(x) / ad::sin(x), 1_c));
}
