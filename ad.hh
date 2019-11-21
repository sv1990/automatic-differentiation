#ifndef AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
#define AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_

#include <type_traits>

#include <cmath>

namespace ad {
namespace detail {
template <std::size_t N>
struct variable;
struct zero;
struct unity;
struct constant;
template <typename L, typename R>
struct addition;
template <typename L, typename R>
struct subtraction;
template <typename L, typename R>
struct multiplication;
template <typename L, typename R>
struct division;
template <typename T>
struct negation;
template <typename T>
struct exponential;
template <typename T>
struct logarithm;
template <typename T>
struct square_root;
template <typename L, typename R>
struct power;

template <typename T>
struct is_constant : std::false_type {};

template <typename T>
constexpr bool is_constant_v = is_constant<T>::value;

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

template <typename T>
constexpr bool is_expr_v = std::is_base_of_v<expression_base<T>, T>;

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
  if constexpr (I >= sizeof...(Ts) + 1) {
    static_assert(dependent_false<Ts...>,
                  "Too few arguments passed! Maybe you meant to use ad::_0 "
                  "instead of ad::_1.");
  }
  else if constexpr (I == 0) {
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

template <typename T>
constexpr auto make_exponential(T x) noexcept {
  return exponential<T>(x);
}

template <typename T>
constexpr auto make_exponential(logarithm<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct exponential : expression_base<exponential<T>> {
  using expression_base<exponential>::derive;
  [[no_unique_address]] T arg;
  constexpr explicit exponential(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::exp(arg(xs...));
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_multiplication(arg.template derive<I>(), *this);
  }
};

template <typename T, std::enable_if_t<is_expr_v<T>>* = nullptr>
constexpr auto exp(T x) noexcept {
  return make_exponential(x);
}

template <typename T>
constexpr auto make_square_root(T x) noexcept {
  return square_root<T>(x);
}

template <typename T>
struct square_root : expression_base<square_root<T>> {
  using expression_base<square_root>::derive;
  [[no_unique_address]] T arg;
  constexpr explicit square_root(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::sqrt(arg(xs...));
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_division(arg.template derive<I>(),
                         make_multiplication(constant{2.0}, *this));
  }
};

template <typename T, std::enable_if_t<is_expr_v<T>>* = nullptr>
constexpr auto sqrt(T x) noexcept {
  return make_square_root(x);
}

template <typename T>
constexpr auto make_logarithm(T x) noexcept {
  return logarithm<T>(x);
}

template <typename T>
constexpr auto make_logarithm(exponential<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct logarithm : expression_base<logarithm<T>> {
  using expression_base<logarithm>::derive;
  [[no_unique_address]] T arg;
  constexpr explicit logarithm(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::log(arg(xs...));
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_division(arg.template derive<I>(), arg);
  }
};

template <typename T, std::enable_if_t<is_expr_v<T>>* = nullptr>
constexpr auto log(T x) noexcept {
  return make_logarithm(x);
}

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

template <typename L, std::enable_if_t<!std::is_same_v<L, unity>>* = nullptr>
constexpr L make_multiplication(L l, unity) noexcept {
  return l;
}

template <typename R>
constexpr R make_multiplication(unity, R r) noexcept {
  return r;
}

template <typename L, typename R>
constexpr auto make_multiplication(negation<L> l, negation<R> r) noexcept {
  return make_multiplication(l.arg, r.arg);
}

template <typename L, typename R>
constexpr auto make_multiplication(exponential<L> l,
                                   exponential<R> r) noexcept {
  return make_exponential(make_addition(l.arg, r.arg));
}

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> || is_expr_v<R>>* = nullptr>
constexpr auto operator*(L l, R r) noexcept {
  return make_multiplication(as_expr(l), as_expr(r));
}

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
constexpr auto make_division(negation<L> l, negation<R> r) noexcept {
  return make_division(l.arg, r.arg);
}

template <typename L, typename R>
constexpr auto make_division(exponential<L> l, exponential<R> r) noexcept {
  return make_exponential(make_subtraction(l.arg, r.arg));
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

template <typename L, typename R>
constexpr auto make_power(L l, R r) noexcept {
  return power<L, R>(l, r);
}

template <typename T>
constexpr auto make_power(zero, T) noexcept {
  return zero{};
}

template <typename T>
constexpr auto make_power(unity, T) noexcept {
  return unity{};
}

template <typename T>
constexpr auto make_power(T, zero) noexcept {
  return zero{};
}

template <typename T>
constexpr auto make_power(T x, unity) noexcept {
  return x;
}

constexpr auto make_power(zero, zero) noexcept {
  return unity{};
}

template <typename L, typename R>
struct power : expression_base<power<L, R>> {
  using expression_base<power>::derive;
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr power(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::pow(lhs(xs...), rhs(xs...));
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_multiplication(
        *this, make_addition(
                   make_division(
                       make_multiplication(lhs.template derive<I>(), rhs), lhs),
                   make_multiplication(log(lhs), rhs.template derive<I>())));
  }
};

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> || is_expr_v<R>>* = nullptr>
constexpr auto pow(L l, R r) noexcept {
  return make_power(as_expr(l), as_expr(r));
}

template <typename T, std::enable_if_t<is_expr_v<T>>* = nullptr>
constexpr auto operator+(T x) noexcept {
  return x;
}

template <typename T>
constexpr auto make_negation(T x) noexcept {
  return negation<T>(x);
}

template <typename T>
constexpr auto make_negation(negation<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct negation : expression_base<negation<T>> {
  using expression_base<negation>::derive;
  [[no_unique_address]] T arg;
  constexpr explicit negation(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return -arg(xs...);
  }
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    return make_negation(arg.template derive<I>());
  }
};

template <typename T, std::enable_if_t<is_expr_v<T>>* = nullptr>
constexpr auto operator-(T x) noexcept {
  return make_negation(x);
}

} // namespace detail

using detail::constant;
using detail::variable;

using detail::exp;
using detail::log;
using detail::pow;
using detail::sqrt;

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
