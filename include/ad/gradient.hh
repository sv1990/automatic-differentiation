#ifndef AUTOMATIC_DIFFERENTIATION_INCLUDE_AD_GRADIENT_HH_1580461051500054384_
#define AUTOMATIC_DIFFERENTIATION_INCLUDE_AD_GRADIENT_HH_1580461051500054384_

#include "ad.hh"
#include "ostream.hh"

namespace ad {
namespace detail {
struct dim_impl {
  template <typename T, std::enable_if_t<is_constant_v<T>>* = nullptr>
  static constexpr int dim(const T&) {
    return 0;
  }
  template <std::size_t N>
  static constexpr std::size_t dim(const variable<N>&) {
    return N;
  }

private:
  template <typename T>
  static constexpr std::size_t unary_dim(const T& x) {
    return dim(x.arg);
  }
  template <typename T>
  static constexpr std::size_t binary_dim(const T& x) {
    return std::max(dim(x.lhs), dim(x.rhs));
  }

public:
  template <typename L, typename R>
  static constexpr std::size_t dim(const addition<L, R>& x) {
    return std::max(dim(x.lhs), dim(x.rhs));
  }
  template <typename L, typename R>
  static constexpr std::size_t dim(const subtraction<L, R>& x) {
    return std::max(dim(x.lhs), dim(x.rhs));
  }
  template <typename L, typename R>
  static constexpr std::size_t dim(const multiplication<L, R>& x) {
    return std::max(dim(x.lhs), dim(x.rhs));
  }
  template <typename L, typename R>
  static constexpr std::size_t dim(const division<L, R>& x) {
    return std::max(dim(x.lhs), dim(x.rhs));
  }
  template <typename L, typename R>
  static constexpr std::size_t dim(const power<L, R>& x) {
    return std::max(dim(x.lhs), dim(x.rhs));
  }

  template <typename T>
  static constexpr std::size_t dim(const sinus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const cosinus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const tangens<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const arcus_sinus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const arcus_cosinus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const arcus_tangens<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const sinus_hyperbolicus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const cosinus_hyperbolicus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const tangens_hyperbolicus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const area_sinus_hyperbolicus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const area_cosinus_hyperbolicus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const area_tangens_hyperbolicus<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const exponential<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const logarithm<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const square_root<T>& x) {
    return unary_dim(x);
  }
  template <typename T>
  static constexpr std::size_t dim(const negation<T>& x) {
    return unary_dim(x);
  }
};
} // namespace detail
template <typename T,
          std::enable_if_t<ad::detail::is_expression_v<T>>* = nullptr>
constexpr std::size_t dim(const T& x) {
  return 1 + detail::dim_impl::dim(x);
}

namespace detail {
template <typename F, std::size_t... Is>
auto derivatives_impl(const F& f, std::index_sequence<Is...>) {
  return std::make_tuple(f.template derive<Is>()...);
}
template <typename F>
auto derivatives(const F& f) {
  return derivatives_impl(f, std::make_index_sequence<dim(f)>{});
}

template <typename T>
void print_pack(std::ostream& os, const T& x) {
  os << x;
}
template <typename T, typename... Ts>
void print_pack(std::ostream& os, const T& x, const Ts&... xs) {
  os << x << ", ";
  print_pack(os, xs...);
}
} // namespace detail

template <typename Tuple>
class gradient_t {
  Tuple _derivatives;

  constexpr gradient_t(Tuple t) : _derivatives(std::move(t)) {}

public:
  template <typename F>
  friend constexpr auto gradient(const F& f);

  template <typename... Ts>
  constexpr auto operator()(Ts... xs) const noexcept {
    return std::apply(
        [xs...](auto... derivatives) {
          return std::array(derivatives(xs...)...);
        },
        _derivatives);
  }

  template <typename T>
  friend std::ostream& operator<<(std::ostream& os, const gradient_t<T>& g) {
    os << '(';
    std::apply(
        [&os](const auto&... xs) mutable { detail::print_pack(os, xs...); },
        g._derivatives);
    os << ')';
    return os;
  }
};

template <typename F>
constexpr auto gradient(const F& f) {
  return gradient_t(detail::derivatives(f));
}

} // namespace ad

#endif // AUTOMATIC_DIFFERENTIATION_INCLUDE_AD_GRADIENT_HH_1580461051500054384_
