#ifndef AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
#define AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_

#include <type_traits>

namespace ad {
namespace detail {
template <typename T>
struct is_expr : std::false_type {};

template <typename T>
constexpr bool is_expr_v = is_expr<T>::value;

template <typename T>
struct is_constant : std::false_type {};

template <typename T>
constexpr bool is_constant_v = is_constant<T>::value;

template <std::size_t N>
struct variable;

template <typename T>
struct is_variable : std::false_type {};

template <std::size_t N>
struct is_variable<variable<N>> : std::true_type {};

template <typename T>
inline constexpr bool is_variable_v = is_variable<T>::value;

template <typename Derived>
struct expression_base {
  template <typename T, std::enable_if_t<is_variable_v<T>>* = nullptr>
  constexpr auto derive(T) const noexcept {
    return static_cast<const Derived&>(*this).template derive<T::value>();
  }
};

struct zero : expression_base<zero> {
  using expression_base<zero>::derive;
  constexpr double value() const noexcept { return 0; }
  template <typename... Ts>
  constexpr double operator()(Ts...) const noexcept {
    return 0;
  }
  template <std::size_t = 0>
  constexpr auto derive() const noexcept {
    return zero{};
  }
};

struct unity : expression_base<unity> {
  using expression_base<unity>::derive;
  constexpr double value() const noexcept { return 1; }
  template <typename... Ts>
  constexpr double operator()(Ts...) const noexcept {
    return 1;
  }
  template <std::size_t = 0>
  constexpr auto derive() const noexcept {
    return zero{};
  }
};

template <>
struct is_expr<zero> : std::true_type {};

template <>
struct is_expr<unity> : std::true_type {};

template <>
struct is_constant<zero> : std::true_type {};

template <>
struct is_constant<unity> : std::true_type {};

struct constant : expression_base<constant> {
  using expression_base<constant>::derive;
  double _value;
  constexpr explicit constant(double value) noexcept : _value(value) {}
  constexpr double value() const noexcept { return _value; }
  template <typename... Ts>
  constexpr double operator()(Ts...) const noexcept {
    return _value;
  }
  template <std::size_t = 0>
  constexpr auto derive() const noexcept {
    return zero{};
  }
};

template <>
struct is_expr<constant> : std::true_type {};

template <>
struct is_constant<constant> : std::true_type {};

template <typename...>
inline constexpr bool dependent_false = false;

template <typename T>
constexpr auto as_expr(T x) noexcept {
  if constexpr (is_expr_v<T>) {
    return x;
  }
  else if constexpr (std::is_arithmetic_v<T>) {
    return constant{static_cast<double>(x)};
  }
  else {
    static_assert(dependent_false<T>, "ad expects arithmetic types!");
  }
}

template <std::size_t I, typename T, typename... Ts>
constexpr auto get([[maybe_unused]] T x, Ts... xs) noexcept {
  static_assert(I < sizeof...(Ts) + 1,
                "Too few arguments passed! Maybe you meant to use ad::_0 "
                "instead of ad::_1.");
  if constexpr (I == 0) {
    return x;
  }
  else {
    return get<I - 1>(xs...);
  }
}

template <std::size_t N>
struct variable : expression_base<variable<N>> {
  using expression_base<variable>::derive;
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return static_cast<double>(get<N>(xs...));
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    if constexpr (I == N) {
      return unity{};
    }
    else {
      return zero{};
    }
  }
  inline static constexpr std::size_t value = N;
};

template <std::size_t n>
struct is_expr<variable<n>> : std::true_type {};

template <typename L, typename R>
struct addition;

template <typename L, typename R>
struct is_expr<addition<L, R>> : std::true_type {};

template <typename L, typename R,
          std::enable_if_t<!(is_constant_v<L> && is_constant_v<R>)>* = nullptr>
constexpr addition<L, R> make_addition(L l, R r) noexcept {
  return addition<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R> &&
                           (!std::is_same_v<L, zero> ||
                            !std::is_same_v<R, zero>)>* = nullptr>
constexpr constant make_addition(L l, R r) noexcept {
  return constant{l.value() + r.value()};
}

template <typename L, std::enable_if_t<!std::is_same_v<L, zero>>* = nullptr>
constexpr L make_addition(L l, zero) noexcept {
  return l;
}

template <typename R>
constexpr R make_addition(zero, R r) noexcept {
  return r;
}

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> || is_expr_v<R>>* = nullptr>
constexpr auto operator+(L l, R r) noexcept {
  return make_addition(as_expr(l), as_expr(r));
}

template <typename L, typename R>
struct addition : expression_base<addition<L, R>> {
  using expression_base<addition>::derive;
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr addition(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return lhs(xs...) + rhs(xs...);
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_addition(lhs.template derive<I>(), rhs.template derive<I>());
  }
};

template <typename L, typename R>
struct subtraction;

template <typename L, typename R>
struct is_expr<subtraction<L, R>> : std::true_type {};

template <typename L, typename R,
          std::enable_if_t<!(is_constant_v<L> && is_constant_v<R>)>* = nullptr>
constexpr subtraction<L, R> make_subtraction(L l, R r) noexcept {
  return subtraction<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R>>* = nullptr>
constexpr constant make_subtraction(L l, R r) noexcept {
  return constant{l.value() - r.value()};
}

template <typename L>
constexpr L make_subtraction(L l, zero) noexcept {
  return l;
}

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> || is_expr_v<R>>* = nullptr>
constexpr auto operator-(L l, R r) noexcept {
  return make_subtraction(as_expr(l), as_expr(r));
}

template <typename L, typename R>
struct subtraction : expression_base<subtraction<L, R>> {
  using expression_base<subtraction>::derive;
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr subtraction(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return lhs(xs...) - rhs(xs...);
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_subtraction(lhs.template derive<I>(), rhs.template derive<I>());
  }
};

template <typename L, typename R>
struct multiplication : expression_base<multiplication<L, R>> {
  using expression_base<multiplication>::derive;
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr multiplication(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return lhs(xs...) * rhs(xs...);
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_addition(make_multiplication(lhs.template derive<I>(), rhs),
                         make_multiplication(lhs, rhs.template derive<I>()));
  }
};

template <typename L, typename R>
constexpr multiplication<L, R> make_multiplication(L l, R r) noexcept {
  return multiplication<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R>>* = nullptr>
constexpr constant make_multiplication(L l, R r) noexcept {
  return constant{l.value() * r.value()};
}

template <typename L>
constexpr zero make_multiplication(L, zero) noexcept {
  return zero{};
}

template <typename R>
constexpr zero make_multiplication(zero, R) noexcept {
  return zero{};
}

template <typename L>
constexpr L make_multiplication(L l, unity) noexcept {
  return l;
}

template <typename R>
constexpr R make_multiplication(unity, R r) noexcept {
  return r;
}

template <typename L, typename R>
struct is_expr<multiplication<L, R>> : std::true_type {};

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> || is_expr_v<R>>* = nullptr>
constexpr auto operator*(L l, R r) noexcept {
  return make_multiplication(as_expr(l), as_expr(r));
}

template <typename L, typename R>
struct division;

template <typename L, typename R>
struct is_expr<division<L, R>> : std::true_type {};

template <typename L, typename R>
constexpr division<L, R> make_division(L l, R r) noexcept {
  return division<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R>>* = nullptr>
constexpr constant make_division(L l, R r) noexcept {
  return constant{l.value() / r.value()};
}

template <typename R>
constexpr zero make_division(zero, R) noexcept {
  return zero{};
}

template <typename L>
constexpr L make_division(L l, unity) noexcept {
  return l;
}

template <typename L, typename R>
struct division : expression_base<division<L, R>> {
  using expression_base<division>::derive;
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr division(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return lhs(xs...) / rhs(xs...);
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_division(
        make_subtraction(make_multiplication(lhs.template derive<I>(), rhs),
                         make_multiplication(lhs, rhs.template derive<I>())),
        make_multiplication(rhs, rhs));
  }
};

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> || is_expr_v<R>>* = nullptr>
constexpr auto operator/(L l, R r) noexcept {
  return make_division(as_expr(l), as_expr(r));
}

} // namespace detail

using detail::constant;
using detail::variable;

inline namespace var {
inline constexpr variable<0> _0;
inline constexpr variable<1> _1;
inline constexpr variable<2> _2;
inline constexpr variable<3> _3;
inline constexpr variable<4> _4;
inline constexpr variable<5> _5;
inline constexpr variable<6> _6;
inline constexpr variable<7> _7;
inline constexpr variable<8> _8;
inline constexpr variable<9> _9;
} // namespace var

inline namespace literals {
constexpr constant operator""_c(long double x) {
  return constant{static_cast<double>(x)};
}
constexpr constant operator""_c(unsigned long long x) {
  return constant{static_cast<double>(x)};
}

} // namespace literals
} // namespace ad

#endif // AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
