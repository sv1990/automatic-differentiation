#include "ad/ad.hh"

template <typename T, typename U>
constexpr bool same_type(T, U) noexcept {
  return std::is_same_v<T, U>;
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

  static_assert(same_type(ad::exp(ad::sin(x)).derive(x),
                          ad::cos(x) * ad::exp(ad::sin(x))));
  static_assert(same_type((ad::exp(x) * ad::sin(x)).derive(x),
                          ad::exp(x) * ad::sin(x) + ad::exp(x) * ad::cos(x)));
}
