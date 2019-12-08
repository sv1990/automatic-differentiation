#ifndef AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
#define AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_

#include <type_traits>

#include <cmath>

namespace ad {
namespace detail {
template <std::size_t N>
struct variable;
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
struct sinus;
template <typename T>
struct cosinus;
template <typename T>
struct tangens;
template <typename T>
struct sinus_hyperbolicus;
template <typename T>
struct cosinus_hyperbolicus;
template <typename T>
struct tangens_hyperbolicus;

template <typename T>
struct is_constant : std::false_type {};

template <typename T>
inline constexpr bool is_constant_v = is_constant<T>::value;

template <typename T>
struct is_variable : std::false_type {};

template <std::size_t N>
struct is_variable<variable<N>> : std::true_type {};

template <typename T>
inline constexpr bool is_variable_v = is_variable<T>::value;

template <typename Derived>
struct expression_base {
#ifdef __clang__
  template <typename T, std::enable_if_t<is_variable_v<T>>* = nullptr>
  constexpr auto derive(T) const noexcept {
    return static_cast<const Derived&>(*this).template derive<T::value>();
  }
#else
  template <typename... Ts,
            std::enable_if_t<(is_variable_v<Ts> && ...)>* = nullptr>
  constexpr auto derive(Ts...) const noexcept {
    return static_cast<const Derived&>(*this).template derive<Ts::value...>();
  }
  template <std::size_t I, std::size_t... Is>
  constexpr auto derive() const noexcept {
    return static_cast<const Derived&>(*this)
        .template derive<I>()
        .template derive<Is...>();
  }
#endif
};

template <typename Derived>
struct function_expression : expression_base<function_expression<Derived>> {
  using expression_base<function_expression<Derived>>::derive;
  template <std::size_t I = 0>
  constexpr auto derive() const noexcept {
    const Derived& derived = static_cast<const Derived&>(*this);
    return make_multiplication(derived.arg.template derive<I>(),
                               derived.derive_outer());
  }
};

template <typename T>
inline constexpr bool is_expression_v =
    std::is_base_of_v<expression_base<T>, T> ||
    std::is_base_of_v<function_expression<T>, T>;

template <long N>
struct integral_constant : expression_base<integral_constant<N>> {
  using expression_base<integral_constant<N>>::derive;
  constexpr double value() const noexcept { return static_cast<double>(N); }
  template <typename... Ts>
  constexpr double operator()(Ts...) const noexcept {
    return value();
  }
  template <std::size_t = 0>
  constexpr auto derive() const noexcept {
    return integral_constant<0>{};
  }
};

template <long N>
struct is_constant<integral_constant<N>> : std::true_type {};

template <typename T>
struct is_integral_constant : std::false_type {};

template <long N>
struct is_integral_constant<integral_constant<N>> : std::true_type {};

template <typename T>
inline constexpr bool is_integral_constant_v = is_integral_constant<T>::value;

using zero  = integral_constant<0>;
using unity = integral_constant<1>;

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
constexpr auto as_expression(T x) noexcept {
  if constexpr (is_expression_v<T>) {
    return x;
  }
  else if constexpr (std::is_arithmetic_v<T>) {
    return constant{static_cast<double>(x)};
  }
  else {
    static_assert(dependent_false<T>,
                  "Wrong type passed! ad expects arithmetic types!");
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
struct exponential : function_expression<exponential<T>> {
  using function_expression<exponential>::derive;
  friend struct function_expression<exponential<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit exponential(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::exp(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept { return *this; }
};

template <typename T>
constexpr auto exp(T x) noexcept {
  return make_exponential(x);
}

template <typename T>
constexpr auto make_square_root(T x) noexcept {
  return square_root<T>(x);
}

template <typename T>
struct square_root : function_expression<square_root<T>> {
  using function_expression<square_root>::derive;
  friend struct function_expression<square_root<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit square_root(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::sqrt(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(unity{},
                         make_multiplication(integral_constant<2>{}, *this));
  }
};

template <typename T>
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
struct logarithm : function_expression<logarithm<T>> {
  using function_expression<logarithm>::derive;
  friend struct function_expression<logarithm<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit logarithm(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::log(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(unity{}, arg);
  }
};

template <typename T>
constexpr auto log(T x) noexcept {
  return make_logarithm(x);
}

template <typename T>
constexpr auto make_sinus(T x) noexcept {
  return sinus<T>(x);
}

template <typename T>
struct sinus : function_expression<sinus<T>> {
  using function_expression<sinus>::derive;
  friend struct function_expression<sinus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit sinus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::sin(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept { return make_cosinus(arg); }
};

template <typename T>
constexpr auto sin(T x) noexcept {
  return make_sinus(x);
}

template <typename T>
constexpr auto make_cosinus(T x) noexcept {
  return cosinus<T>(x);
}

template <typename T>
struct cosinus : function_expression<cosinus<T>> {
  using function_expression<cosinus>::derive;
  friend struct function_expression<cosinus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit cosinus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::cos(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_negation(make_sinus(arg));
  }
};

template <typename T>
constexpr auto cos(T x) noexcept {
  return make_cosinus(x);
}

template <typename T>
constexpr auto make_tangens(T x) noexcept {
  return tangens<T>(x);
}

template <typename T>
struct tangens : function_expression<tangens<T>> {
  using function_expression<tangens>::derive;
  friend struct function_expression<tangens<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit tangens(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::tan(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_addition(unity{}, make_multiplication(*this, *this));
  }
};

template <typename T>
constexpr auto tan(T x) noexcept {
  return make_tangens(x);
}

template <typename T>
constexpr auto make_sinus_hyperbolicus(T x) noexcept {
  return sinus_hyperbolicus<T>(x);
}
template <typename T>
struct sinus_hyperbolicus : function_expression<sinus_hyperbolicus<T>> {
  using function_expression<sinus_hyperbolicus>::derive;
  friend struct function_expression<sinus_hyperbolicus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit sinus_hyperbolicus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::sinh(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_cosinus_hyperbolicus(arg);
  }
};
template <typename T>
constexpr auto sinh(T x) noexcept {
  return make_sinus_hyperbolicus(x);
}

template <typename T>
constexpr auto make_cosinus_hyperbolicus(T x) noexcept {
  return cosinus_hyperbolicus<T>(x);
}
template <typename T>
struct cosinus_hyperbolicus : function_expression<cosinus_hyperbolicus<T>> {
  using function_expression<cosinus_hyperbolicus>::derive;
  friend struct function_expression<cosinus_hyperbolicus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit cosinus_hyperbolicus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::cosh(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_sinus_hyperbolicus(arg);
  }
};
template <typename T>
constexpr auto cosh(T x) noexcept {
  return make_cosinus_hyperbolicus(x);
}

template <typename T>
constexpr auto make_tangens_hyperbolicus(T x) noexcept {
  return tangens_hyperbolicus<T>(x);
}

template <typename T>
struct tanges_hyperbolicus : function_expression<tanges_hyperbolicus<T>> {
  using function_expression<tanges_hyperbolicus>::derive;
  friend struct function_expression<tanges_hyperbolicus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit tanges_hyperbolicus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::tanh(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_subtraction(unity{}, make_multiplication(*this, *this));
  }
};
template <typename T>
constexpr auto tanh(T x) noexcept {
  return make_tangens_hyperbolicus(x);
}

template <typename L, typename R,
          std::enable_if_t<!(is_constant_v<L> && is_constant_v<R>)>* = nullptr>
constexpr addition<L, R> make_addition(L l, R r) noexcept {
  return addition<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R> &&
                           (!is_integral_constant_v<L> ||
                            !is_integral_constant_v<R>)>* = nullptr>
constexpr constant make_addition(L l, R r) noexcept {
  return constant{l.value() + r.value()};
}

template <long N1, long N2>
constexpr auto make_addition(integral_constant<N1>,
                             integral_constant<N2>) noexcept {
  return integral_constant<N1 + N2>{};
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr auto make_addition(zero, T x) noexcept {
  return x;
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr auto make_addition(T x, zero) noexcept {
  return x;
}

template <typename L, typename R>
constexpr auto operator+(L l, R r) noexcept {
  return make_addition(as_expression(l), as_expression(r));
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
          std::enable_if_t<is_constant_v<L> && is_constant_v<R> &&
                           (!is_integral_constant_v<L> ||
                            !is_integral_constant_v<R>)>* = nullptr>
constexpr constant make_subtraction(L l, R r) noexcept {
  return constant{l.value() - r.value()};
}

template <long N1, long N2>
constexpr auto make_subtraction(integral_constant<N1>,
                                integral_constant<N2>) noexcept {
  return integral_constant<N1 - N2>{};
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr T make_subtraction(T x, zero) noexcept {
  return x;
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr T make_subtraction(zero, T x) noexcept {
  return make_negation(x);
}

template <typename L, typename R>
constexpr auto operator-(L l, R r) noexcept {
  return make_subtraction(as_expression(l), as_expression(r));
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

template <typename L, typename R,
          std::enable_if_t<!is_constant_v<L> || !is_constant_v<R>>* = nullptr>
constexpr multiplication<L, R> make_multiplication(L l, R r) noexcept {
  return multiplication<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R> &&
                           (!is_integral_constant_v<L> ||
                            !is_integral_constant_v<R>)>* = nullptr>
constexpr constant make_multiplication(L l, R r) noexcept {
  return constant{l.value() * r.value()};
}

template <long N1, long N2>
constexpr auto make_multiplication(integral_constant<N1>,
                                   integral_constant<N2>) noexcept {
  return integral_constant<N1 * N2>{};
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr zero make_multiplication(T, zero) noexcept {
  return zero{};
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr zero make_multiplication(zero, T) noexcept {
  return zero{};
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr T make_multiplication(T x, unity) noexcept {
  return x;
}

template <typename T, std::enable_if_t<!is_integral_constant_v<T>>* = nullptr>
constexpr T make_multiplication(unity, T x) noexcept {
  return x;
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
          std::enable_if_t<!std::is_same_v<L, unity>>* = nullptr>
constexpr auto make_multiplication(L l, division<unity, R> r) noexcept {
  return make_division(l, r.rhs);
}

template <typename L, typename R,
          std::enable_if_t<!std::is_same_v<R, unity>>* = nullptr>
constexpr auto make_multiplication(division<unity, L> l, R r) noexcept {
  return make_division(r, l.rhs);
}

template <typename L, typename R>
constexpr auto operator*(L l, R r) noexcept {
  return make_multiplication(as_expression(l), as_expression(r));
}

template <typename L, typename R,
          std::enable_if_t<!is_constant_v<L> || !is_constant_v<R>>* = nullptr>
constexpr division<L, R> make_division(L l, R r) noexcept {
  return division<L, R>(l, r);
}

// TODO: Overload for integral_constant?
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

template <typename T, typename L, typename R>
constexpr auto make_division(T l, division<L, R> r) noexcept {
  return make_division(make_multiplication(l, r.rhs), r.lhs);
}

template <typename L, typename R, typename T>
constexpr auto make_division(division<L, R> l, T r) noexcept {
  return make_division(l.lhs, make_multiplication(l.rhs, r));
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
    if constexpr (is_constant_v<R>) {
      return make_division(lhs.template derive<I>(), rhs);
    }
    else {
      return make_division(
          make_subtraction(make_multiplication(lhs.template derive<I>(), rhs),
                           make_multiplication(lhs, rhs.template derive<I>())),
          make_multiplication(rhs, rhs));
    }
  }
};

template <typename L, typename R>
constexpr auto operator/(L l, R r) noexcept {
  return make_division(as_expression(l), as_expression(r));
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
    if constexpr (is_constant_v<R>) {
      return make_multiplication(
          rhs, make_power(lhs, make_subtraction(rhs, unity{})));
    }
    else {
      return make_multiplication(
          *this, make_addition(make_division(make_multiplication(
                                                 lhs.template derive<I>(), rhs),
                                             lhs),
                               make_multiplication(make_logarithm(lhs),
                                                   rhs.template derive<I>())));
    }
  }
};

template <typename L, typename R>
constexpr auto pow(L l, R r) noexcept {
  return make_power(as_expression(l), as_expression(r));
}

template <typename T>
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

template <typename T>
constexpr auto operator-(T x) noexcept {
  return make_negation(x);
}

constexpr long parse_integral(const char* s, std::ptrdiff_t n) noexcept {
  long res = 0;
  for (std::ptrdiff_t i = n - 1; i >= 0; --i) {
    res *= 10;
    res += s[i] - '0';
  }
  return res;
}
} // namespace detail

inline namespace literals {
constexpr auto operator""_c(long double x) noexcept {
  return detail::constant{static_cast<double>(x)};
}

template <char... cs>
constexpr auto operator""_c() noexcept {
  constexpr const char s[] = {cs...};
  return detail::integral_constant<detail::parse_integral(s, sizeof...(cs))>{};
}
} // namespace literals

using detail::constant;
using detail::variable;

using detail::cos;
using detail::cosh;
using detail::exp;
using detail::log;
using detail::pow;
using detail::sin;
using detail::sinh;
using detail::sqrt;
using detail::tan;
using detail::tanh;

inline namespace vars {
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
} // namespace vars

} // namespace ad

#endif // AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
