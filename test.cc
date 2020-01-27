#include "ad.hh"

#include <show/all.hh>

template <typename T, typename U>
constexpr bool same_type(T, U) noexcept {
  return std::is_same_v<T, U>;
}

int main() {
  using namespace ad::literals;
  constexpr auto x = ad::_0;
  constexpr auto y = ad::_1;

  static_assert(
      std::is_same_v<std::decay_t<decltype(123_c)>, ad::static_constant<123>>);

  static_assert(same_type(x.derive(x), 1_c));
  static_assert(same_type(x.derive(y), 0_c));
  static_assert(same_type(y.derive(x), 0_c));
  static_assert(same_type(y.derive(y), 1_c));
}
