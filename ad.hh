#ifndef AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
#define AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_

#include <type_traits>

namespace ad {
template <typename T>
struct is_expr : std::false_type {};

template <typename T>
constexpr bool is_expr_v = is_expr<T>::value;

template <typename T>
struct is_constant : std::false_type {};

template <typename T>
constexpr bool is_constant_v = is_constant<T>::value;

struct zero {
  constexpr double value() const noexcept { return 0; }
  constexpr double operator()(double) const noexcept { return 0; }
  constexpr auto derive() const noexcept { return zero{}; }
};

struct unity {
  constexpr double value() const noexcept { return 1; }
  constexpr double operator()(double) const noexcept { return 1; }
  constexpr auto derive() const noexcept { return zero{}; }
};

template <>
struct is_expr<zero> : std::true_type {};

template <>
struct is_expr<unity> : std::true_type {};

template <>
struct is_constant<zero> : std::true_type {};

template <>
struct is_constant<unity> : std::true_type {};

struct constant {
  double _value;
  constexpr double value() const noexcept { return _value; }
  constexpr double operator()(double) const noexcept { return _value; }
  constexpr auto derive() const noexcept { return zero{}; }
};

template <>
struct is_expr<constant> : std::true_type {};

template <>
struct is_constant<constant> : std::true_type {};

template <char... cs>
struct variable {
  constexpr double operator()(double x) const noexcept { return x; }
  constexpr auto derive() const noexcept { return unity{}; }
};

template <char... cs>
struct is_expr<variable<cs...>> : std::true_type {};

namespace var {
const variable<'x'> x;
}

template <typename L, typename R>
struct addition;

template <typename L, typename R,
          std::enable_if_t<!(is_constant_v<L> && is_constant_v<R>)>* = nullptr>
constexpr addition<L, R> make_addition(L l, R r) noexcept {
  return addition<L, R>(l, r);
}

template <typename L, typename R,
          std::enable_if_t<is_constant_v<L> && is_constant_v<R>>* = nullptr>
constexpr constant make_addition(L l, R r) noexcept {
  return constant{l.value() + r.value()};
}

template <typename L, typename R>
struct addition {
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr addition(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  constexpr double operator()(double x) const noexcept {
    return lhs(x) + rhs(x);
  }
  constexpr auto derive() const noexcept {
    return make_addition(lhs.derive(), rhs.derive());
  }
};

template <typename L>
constexpr L make_addition(L l, zero) noexcept {
  return l;
}

template <typename R>
constexpr R make_addition(zero, R r) noexcept {
  return r;
}

template <typename L, typename R>
struct is_expr<addition<L, R>> : std::true_type {};

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> && is_expr_v<R>>* = nullptr>
constexpr auto operator+(L l, R r) noexcept {
  return make_addition(l, r);
}

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

template <typename L, typename R>
struct subtraction {
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr subtraction(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  constexpr double operator()(double x) const noexcept {
    return lhs(x) - rhs(x);
  }
  constexpr auto derive() const noexcept {
    return make_subtraction(lhs.derive(), rhs.derive());
  }
};

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> && is_expr_v<R>>* = nullptr>
constexpr auto operator-(L l, R r) noexcept {
  return make_subtraction(l, r);
}

template <typename L, typename R>
struct multiplication {
  [[no_unique_address]] L lhs;
  [[no_unique_address]] R rhs;
  constexpr multiplication(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  constexpr double operator()(double x) const noexcept {
    return lhs(x) * rhs(x);
  }
  constexpr auto derive() const noexcept {
    return make_addition(make_multiplication(lhs.derive(), rhs),
                         make_multiplication(lhs, rhs.derive()));
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
          std::enable_if_t<is_expr_v<L> && is_expr_v<R>>* = nullptr>
constexpr auto operator*(L l, R r) noexcept {
  return make_multiplication(l, r);
}

namespace literals {
constexpr constant operator""_c(long double x) {
  return constant{static_cast<double>(x)};
}
constexpr constant operator""_c(unsigned long long x) {
  return constant{static_cast<double>(x)};
}

} // namespace literals
} // namespace ad

#endif // AUTOMATICDIFFERENTIATION_AD_HH_1574234361739842350_
