#include <iomanip>
#include <iostream>
#include <type_traits>

namespace ad {
template <typename T>
struct is_expr : std::false_type {};

template <typename T>
constexpr bool is_expr_v = is_expr<T>::value;

struct zero {
  constexpr double operator()(double) const noexcept { return 0; }
  constexpr auto derive() const noexcept { return zero{}; }
};

struct unity {
  constexpr double operator()(double) const noexcept { return 1; }
  constexpr auto derive() const noexcept { return zero{}; }
};

template <>
struct is_expr<zero> : std::true_type {};

template <>
struct is_expr<unity> : std::true_type {};

struct constant {
  double value;
  constexpr double operator()(double) const noexcept { return value; }
  constexpr auto derive() const noexcept { return zero{}; }
};

template <>
struct is_expr<constant> : std::true_type {};

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

template <typename L, typename R>
addition<L, R> make_addition(L l, R r) noexcept;

template <typename L, typename R>
struct addition {
  L lhs;
  R rhs;
  constexpr addition(L lhs_, R rhs_) noexcept : lhs(lhs_), rhs(rhs_) {}
  constexpr double operator()(double x) const noexcept {
    return lhs(x) + rhs(x);
  }
  constexpr auto derive() const noexcept {
    return make_addition(lhs.derive(), rhs.derive());
  }
};

template <typename L, typename R>
addition<L, R> make_addition(L l, R r) noexcept {
  return addition<L, R>(l, r);
}

template <typename L>
L make_addition(L l, zero) noexcept {
  return l;
}

template <typename R>
R make_addition(zero, R r) noexcept {
  return r;
}

template <typename L, typename R>
struct is_expr<addition<L, R>> : std::true_type {};

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> && is_expr_v<R>>* = nullptr>
auto operator+(L l, R r) noexcept {
  return addition<L, R>{l, r};
}

template <typename L, typename R>
struct multiplication {
  L lhs;
  R rhs;
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
multiplication<L, R> make_multiplication(L l, R r) noexcept {
  return multiplication<L, R>(l, r);
}

template <typename L>
zero make_multiplication(L, zero) noexcept {
  return zero{};
}

template <typename R>
zero make_multiplication(zero, R) noexcept {
  return zero{};
}

template <typename L>
L make_multiplication(L l, unity) noexcept {
  return l;
}

template <typename R>
R make_multiplication(unity, R r) noexcept {
  return r;
}

template <typename L, typename R>
struct is_expr<multiplication<L, R>> : std::true_type {};

template <typename L, typename R,
          std::enable_if_t<is_expr_v<L> && is_expr_v<R>>* = nullptr>
auto operator*(L l, R r) noexcept {
  return multiplication<L, R>{l, r};
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

int main() {
  using namespace ad::literals;
  using ad::var::x;

  const auto f   = x * (x + 1_c);
  const auto df  = f.derive();
  const auto d2f = df.derive();

  const int colwidth = 7;
  std::cout << std::setw(colwidth) << std::right << "x" << ' '     //
            << std::setw(colwidth) << std::right << "f(x)" << ' '  //
            << std::setw(colwidth) << std::right << "df(x)" << ' ' //
            << std::setw(colwidth) << std::right << "d2f(x)" << '\n';
  std::cout << std::setw(colwidth) << std::right << "size" << ' '     //
            << std::setw(colwidth) << std::right << sizeof(f) << ' '  //
            << std::setw(colwidth) << std::right << sizeof(df) << ' ' //
            << std::setw(colwidth) << std::right << sizeof(d2f) << '\n';
  for (double x = 0; x <= 10; x += 1) {
    std::cout << std::setw(colwidth) << std::right << x << ' '     //
              << std::setw(colwidth) << std::right << f(x) << ' '  //
              << std::setw(colwidth) << std::right << df(x) << ' ' //
              << std::setw(colwidth) << std::right << d2f(x) << '\n';
  }
}
