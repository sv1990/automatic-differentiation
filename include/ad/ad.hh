#ifndef AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
#define AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_

#include <tuple>
#include <type_traits>

#include <cmath>

namespace ad {
namespace detail {
template <std::size_t N>
struct variable;
struct runtime_constant;
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
struct arcus_sinus;
template <typename T>
struct arcus_cosinus;
template <typename T>
struct arcus_tangens;
template <typename T>
struct area_sinus_hyperbolicus;
template <typename T>
struct area_cosinus_hyperbolicus;
template <typename T>
struct area_tangens_hyperbolicus;

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
  template <typename... Ts,
            std::enable_if_t<std::conjunction_v<is_variable<Ts>...>>* = nullptr>
  constexpr auto derive(Ts...) const noexcept {
    return derive<Ts::value...>();
  }
#if __clang__
  // workaround for
  // https://www.reddit.com/r/cpp_questions/comments/ev4657/possible_bug_in_clangs_template_lookup/
  template <std::size_t I, std::size_t... Is>
  constexpr auto derive_helper() const noexcept {
    const Derived& derived = static_cast<const Derived&>(*this);
    if constexpr (sizeof...(Is) == 1) {
      return derived            //
          .template derive<I>() //
          .template derive<Is...>();
    }
    else {
      return derived            //
          .template derive<I>() //
          .template derive_helper<Is...>();
    }
  }

  template <std::size_t I, std::size_t... Is>
  constexpr auto derive() const noexcept {
    const Derived& derived = static_cast<const Derived&>(*this);
    if constexpr (sizeof...(Is) == 0) {
      return derived.template derive<I>();
    }
    else if constexpr (sizeof...(Is) == 1) {
      return derived            //
          .template derive<I>() //
          .template derive<Is...>();
    }
    else {
      return derived            //
          .template derive<I>() //
          .template derive_helper<Is...>();
    }
  }
#else
  template <std::size_t I, std::size_t... Is>
  constexpr auto derive() const noexcept {
    const Derived& derived = static_cast<const Derived&>(*this);
    if constexpr (sizeof...(Is) > 0) {
      return derived            //
          .template derive<I>() //
          .template derive<Is...>();
    }
    else {
      return derived.template derive<I>();
    }
  }
#endif
};

template <typename Derived>
struct unary_function : expression_base<unary_function<Derived>> {
  using expression_base<unary_function>::derive;
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
    std::is_base_of_v<unary_function<T>, T>;

template <long N>
struct static_constant : expression_base<static_constant<N>> {
  using expression_base<static_constant>::derive;
  constexpr double value() const noexcept { return static_cast<double>(N); }
  template <typename... Ts>
  constexpr double operator()(Ts...) const noexcept {
    return value();
  }
  template <std::size_t = 0>
  constexpr auto derive() const noexcept {
    return static_constant<0>{};
  }
};

template <long N>
struct is_constant<static_constant<N>> : std::true_type {};

template <typename T>
struct is_static_constant : std::false_type {};

template <long N>
struct is_static_constant<static_constant<N>> : std::true_type {};

template <typename T>
inline constexpr bool is_static_constant_v = is_static_constant<T>::value;

using zero  = static_constant<0>;
using unity = static_constant<1>;

struct runtime_constant : expression_base<runtime_constant> {
  using expression_base<runtime_constant>::derive;
  double _value;
  constexpr explicit runtime_constant(double value) noexcept : _value(value) {}
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
struct is_constant<runtime_constant> : std::true_type {};

template <typename...>
inline constexpr bool dependent_false = false;

template <typename T>
constexpr auto as_expression(T x) noexcept {
  if constexpr (is_expression_v<T>) {
    return x;
  }
  else if constexpr (std::is_arithmetic_v<T>) {
    return runtime_constant{static_cast<double>(x)};
  }
  else {
    static_assert(dependent_false<T>,
                  "Wrong type passed! ad expects arithmetic types!");
  }
}

template <std::size_t I, typename... Ts>
constexpr auto get(Ts... xs) noexcept {
  if constexpr (I >= sizeof...(Ts)) {
    static_assert(dependent_false<Ts...>,
                  "Too few arguments passed! Maybe you meant to use ad::_0 "
                  "instead of ad::_1.");
  }
  else {
    return std::get<I>(std::make_tuple(xs...));
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
struct exponential : unary_function<exponential<T>> {
  using unary_function<exponential>::derive;
  friend struct unary_function<exponential<T>>;
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
struct square_root : unary_function<square_root<T>> {
  using unary_function<square_root>::derive;
  friend struct unary_function<square_root<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit square_root(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::sqrt(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(unity{},
                         make_multiplication(static_constant<2>{}, *this));
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
struct logarithm : unary_function<logarithm<T>> {
  using unary_function<logarithm>::derive;
  friend struct unary_function<logarithm<T>>;
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
constexpr auto make_sinus(arcus_sinus<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct sinus : unary_function<sinus<T>> {
  using unary_function<sinus>::derive;
  friend struct unary_function<sinus<T>>;
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
constexpr auto make_cosinus(arcus_cosinus<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct cosinus : unary_function<cosinus<T>> {
  using unary_function<cosinus>::derive;
  friend struct unary_function<cosinus<T>>;
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
constexpr auto make_tangens(arcus_tangens<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct tangens : unary_function<tangens<T>> {
  using unary_function<tangens>::derive;
  friend struct unary_function<tangens<T>>;
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
constexpr auto make_sinus_hyperbolicus(area_sinus_hyperbolicus<T> x) noexcept {
  return x.arg;
}
template <typename T>
struct sinus_hyperbolicus : unary_function<sinus_hyperbolicus<T>> {
  using unary_function<sinus_hyperbolicus>::derive;
  friend struct unary_function<sinus_hyperbolicus<T>>;
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
constexpr auto
make_cosinus_hyperbolicus(area_cosinus_hyperbolicus<T> x) noexcept {
  return x.arg;
}
template <typename T>
struct cosinus_hyperbolicus : unary_function<cosinus_hyperbolicus<T>> {
  using unary_function<cosinus_hyperbolicus>::derive;
  friend struct unary_function<cosinus_hyperbolicus<T>>;
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
constexpr auto
make_tangens_hyperbolicus(area_tangens_hyperbolicus<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct tangens_hyperbolicus : unary_function<tangens_hyperbolicus<T>> {
  using unary_function<tangens_hyperbolicus>::derive;
  friend struct unary_function<tangens_hyperbolicus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit tangens_hyperbolicus(T x) noexcept : arg(x) {}
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

template <typename T>
constexpr auto make_arcus_sinus(T x) noexcept {
  return arcus_sinus<T>(x);
}
template <typename T>
constexpr auto make_arcus_sinus(sinus<T> x) noexcept {
  return x.arg;
}
template <typename T>
struct arcus_sinus : unary_function<arcus_sinus<T>> {
  using unary_function<arcus_sinus>::derive;
  friend struct unary_function<arcus_sinus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit arcus_sinus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::asin(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(unity{}, make_square_root(make_subtraction(
                                      unity{}, make_multiplication(arg, arg))));
  }
};
template <typename T>
constexpr auto asin(T x) noexcept {
  return make_arcus_sinus(x);
}

template <typename T>
constexpr auto make_arcus_cosinus(T x) noexcept {
  return arcus_cosinus<T>(x);
}
template <typename T>
constexpr auto make_arcus_cosinus(cosinus<T> x) noexcept {
  return x.arg;
}
template <typename T>
struct arcus_cosinus : unary_function<arcus_cosinus<T>> {
  using unary_function<arcus_cosinus>::derive;
  friend struct unary_function<arcus_cosinus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit arcus_cosinus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::acos(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_negation(
        make_division(unity{}, make_square_root(make_subtraction(
                                   unity{}, make_multiplication(arg, arg)))));
  }
};
template <typename T>
constexpr auto acos(T x) noexcept {
  return make_arcus_cosinus(x);
}

template <typename T>
constexpr auto make_arcus_tangens(T x) noexcept {
  return arcus_tangens<T>(x);
}
template <typename T>
constexpr auto make_arcus_tangens(tangens<T> x) noexcept {
  return x.arg;
}
template <typename T>
struct arcus_tangens : unary_function<arcus_tangens<T>> {
  using unary_function<arcus_tangens>::derive;
  friend struct unary_function<arcus_tangens<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit arcus_tangens(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::atan(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(unity{},
                         make_addition(unity{}, make_multiplication(arg, arg)));
  }
};
template <typename T>
constexpr auto atan(T x) noexcept {
  return make_arcus_tangens(x);
}

template <typename T>
constexpr auto make_area_sinus_hyperbolicus(T x) noexcept {
  return area_sinus_hyperbolicus<T>(x);
}
template <typename T>
constexpr auto make_area_sinus_hyperbolicus(sinus_hyperbolicus<T> x) noexcept {
  return x.arg;
}
template <typename T>
struct area_sinus_hyperbolicus : unary_function<area_sinus_hyperbolicus<T>> {
  using unary_function<area_sinus_hyperbolicus>::derive;
  friend struct unary_function<area_sinus_hyperbolicus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit area_sinus_hyperbolicus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::asinh(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(unity{}, make_square_root(make_addition(
                                      unity{}, make_multiplication(arg, arg))));
  }
};
template <typename T>
constexpr auto asinh(T x) noexcept {
  return make_area_sinus_hyperbolicus(x);
}
template <typename T>
constexpr auto make_area_cosinus_hyperbolicus(T x) noexcept {
  return area_cosinus_hyperbolicus<T>(x);
}
template <typename T>
constexpr auto
make_area_cosinus_hyperbolicus(cosinus_hyperbolicus<T> x) noexcept {
  return x.arg;
}

template <typename T>
struct area_cosinus_hyperbolicus
    : unary_function<area_cosinus_hyperbolicus<T>> {
  using unary_function<area_cosinus_hyperbolicus>::derive;
  friend struct unary_function<area_cosinus_hyperbolicus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit area_cosinus_hyperbolicus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::acosh(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(
        unity{},
        make_multiplication(make_square_root(make_subtraction(arg, unity{})),
                            make_square_root(make_addition(unity{}, arg))));
  }
};
template <typename T>
constexpr auto acosh(T x) noexcept {
  return make_area_cosinus_hyperbolicus(x);
}
template <typename T>
constexpr auto make_area_tangens_hyperbolicus(T x) noexcept {
  return area_tangens_hyperbolicus<T>(x);
}
template <typename T>
constexpr auto
make_area_tangens_hyperbolicus(tangens_hyperbolicus<T> x) noexcept {
  return x.arg;
}
template <typename T>
struct area_tangens_hyperbolicus
    : unary_function<area_tangens_hyperbolicus<T>> {
  using unary_function<area_tangens_hyperbolicus>::derive;
  friend struct unary_function<area_tangens_hyperbolicus<T>>;
  [[no_unique_address]] T arg;
  constexpr explicit area_tangens_hyperbolicus(T x) noexcept : arg(x) {}
  template <typename... Ts>
  constexpr double operator()(Ts... xs) const noexcept {
    return std::atanh(arg(xs...));
  }

private:
  constexpr auto derive_outer() const noexcept {
    return make_division(
        unity{}, make_subtraction(unity{}, make_multiplication(arg, arg)));
  }
};
template <typename T>
constexpr auto atanh(T x) noexcept {
  return make_area_tangens_hyperbolicus(x);
}

template <typename L, typename R,
          std::enable_if_t<!(is_constant_v<L> && is_constant_v<R>)>* = nullptr>
constexpr auto make_addition(L l, R r) noexcept {
  return addition<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R> &&
                           (!is_static_constant_v<L> ||
                            !is_static_constant_v<R>)>* = nullptr>
constexpr auto make_addition(L l, R r) noexcept {
  return runtime_constant{l.value() + r.value()};
}

template <long N1, long N2>
constexpr auto make_addition(static_constant<N1>,
                             static_constant<N2>) noexcept {
  return static_constant<N1 + N2>{};
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
constexpr auto make_addition(zero, T x) noexcept {
  return x;
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
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
  constexpr explicit addition(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
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
constexpr auto make_subtraction(L l, R r) noexcept {
  return subtraction<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R> &&
                           (!is_static_constant_v<L> ||
                            !is_static_constant_v<R>)>* = nullptr>
constexpr auto make_subtraction(L l, R r) noexcept {
  return runtime_constant{l.value() - r.value()};
}

template <long N1, long N2>
constexpr auto make_subtraction(static_constant<N1>,
                                static_constant<N2>) noexcept {
  return static_constant<N1 - N2>{};
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
constexpr auto make_subtraction(T x, zero) noexcept {
  return x;
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
constexpr auto make_subtraction(zero, T x) noexcept {
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
  constexpr explicit subtraction(L lhs_, R rhs_) noexcept
      : lhs(lhs_), rhs(rhs_) {}
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
  constexpr explicit multiplication(L lhs_, R rhs_) noexcept
      : lhs(lhs_), rhs(rhs_) {}
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
constexpr auto make_multiplication(L l, R r) noexcept {
  return multiplication<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R> &&
                           (!is_static_constant_v<L> ||
                            !is_static_constant_v<R>)>* = nullptr>
constexpr auto make_multiplication(L l, R r) noexcept {
  return runtime_constant{l.value() * r.value()};
}

template <long N1, long N2>
constexpr auto make_multiplication(static_constant<N1>,
                                   static_constant<N2>) noexcept {
  return static_constant<N1 * N2>{};
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
constexpr auto make_multiplication(T, zero) noexcept {
  return zero{};
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
constexpr auto make_multiplication(zero, T) noexcept {
  return zero{};
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
constexpr auto make_multiplication(T x, unity) noexcept {
  return x;
}

template <typename T, std::enable_if_t<!is_static_constant_v<T>>* = nullptr>
constexpr auto make_multiplication(unity, T x) noexcept {
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
constexpr auto make_division(L l, R r) noexcept {
  return division<L, R>(l, r);
}

// TODO: Overload for static_constant?
template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R>>* = nullptr>
constexpr auto make_division(L l, R r) noexcept {
  return runtime_constant{l.value() / r.value()};
}

template <typename R>
constexpr auto make_division(zero, R) noexcept {
  return zero{};
}

template <typename L>
constexpr auto make_division(L l, unity) noexcept {
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
  constexpr explicit division(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
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
  constexpr explicit power(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
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

constexpr auto make_negation(zero) noexcept {
  return zero{};
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

constexpr long parse_integral(const char* s, std::size_t n) noexcept {
  long res = 0;
  for (std::size_t i = 0; i < n; ++i) {
    res *= 10;
    res += s[i] - '0';
  }
  return res;
}
} // namespace detail

inline namespace literals {
constexpr auto operator""_c(long double x) noexcept {
  return detail::runtime_constant{static_cast<double>(x)};
}

template <char... cs>
constexpr auto operator""_c() noexcept {
  constexpr const char s[] = {cs...};
  return detail::static_constant<detail::parse_integral(s, sizeof...(cs))>{};
}
} // namespace literals

using detail::runtime_constant;
using detail::static_constant;
using detail::variable;

using detail::acos;
using detail::acosh;
using detail::asin;
using detail::asinh;
using detail::atan;
using detail::atanh;
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
