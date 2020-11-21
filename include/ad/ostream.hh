#ifndef AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_
#define AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_

#include "ad.hh"

#include <ostream>
#include <sstream>

namespace ad {
namespace detail {
template <typename... Ts>
std::string concat(const Ts&... xs) {
  std::ostringstream oss;
  (oss << ... << xs);
  return std::move(oss).str();
}
} // namespace detail

template <std::size_t N>
struct format_variable {
  static inline const std::string rep = detail::concat("x", N);
};

namespace detail {
template <template <typename, typename> typename Op>
struct is_operator {
  template <typename T>
  bool operator()(const T&) const noexcept {
    return false;
  }
  template <typename L, typename R>
  bool operator()(const Op<L, R>&) const noexcept {
    return true;
  }
};
is_operator<addition> is_addition;
is_operator<multiplication> is_multiplication;

template <typename T>
inline constexpr bool is_binary_operator_v = false;

template <typename L, typename R>
inline constexpr bool is_binary_operator_v<addition<L, R>> = true;

template <typename L, typename R>
inline constexpr bool is_binary_operator_v<subtraction<L, R>> = true;

template <typename L, typename R>
inline constexpr bool is_binary_operator_v<multiplication<L, R>> = true;

template <typename L, typename R>
inline constexpr bool is_binary_operator_v<division<L, R>> = true;

template <typename L, typename R>
inline constexpr bool is_binary_operator_v<power<L, R>> = true;

template <typename T>
concept constant = is_constant_v<T>;

struct print_impl {
  static void print(std::ostream& os, const constant auto& x) {
    os << x.value();
  }
  template <std::size_t N>
  static void print(std::ostream& os, const variable<N>&) {
    os << format_variable<N>::rep;
  }

private:
  template <typename T>
  static void print_function(std::ostream& os, std::string_view op,
                             const T& x) {
    os << op << '(';
    print(os, x.arg);
    os << ')';
  }
  template <typename T>
  static void print_with_brackets(std::ostream& os, bool needs_brackets,
                                  const T& x) {
    if (needs_brackets) {
      os << '(';
    }
    print(os, x);
    if (needs_brackets) {
      os << ')';
    }
  }

  template <typename T>
  static void print_binary_operator(std::ostream& os, std::string_view op,
                                    const T& x) {
    const auto& lhs = x.lhs;
    const auto& rhs = x.rhs;

    const bool lhs_needs_brackets = precedence(x) > precedence(lhs);
    const bool rhs_needs_brackets =
        precedence(x) > precedence(rhs)
        || (precedence(x) == precedence(rhs)
            && !((is_addition(x) && is_addition(rhs))
                 || (is_multiplication(x) && is_multiplication(rhs))));

    print_with_brackets(os, lhs_needs_brackets, lhs);
    os << ' ' << op << ' ';
    print_with_brackets(os, rhs_needs_brackets, rhs);
  }

  template <typename L, typename R>
  static constexpr int precedence(const addition<L, R>&) {
    return 1;
  }
  template <typename L, typename R>
  static constexpr int precedence(const subtraction<L, R>&) {
    return 1;
  }
  template <typename L, typename R>
  static constexpr int precedence(const multiplication<L, R>&) {
    return 2;
  }
  template <typename L, typename R>
  static constexpr int precedence(const division<L, R>&) {
    return 2;
  }
  template <typename L, typename R>
  static constexpr int precedence(const power<L, R>&) {
    return 3;
  }
  static constexpr int precedence(...) { return 4; }

public:
  template <typename L, typename R>
  static void print(std::ostream& os, const addition<L, R>& x) {
    print_binary_operator(os, "+", x);
  }
  template <typename L, typename R>
  static void print(std::ostream& os, const subtraction<L, R>& x) {
    print_binary_operator(os, "-", x);
  }
  template <typename L, typename R>
  static void print(std::ostream& os, const multiplication<L, R>& x) {
    print_binary_operator(os, "*", x);
  }
  template <typename L, typename R>
  static void print(std::ostream& os, const division<L, R>& x) {
    print_binary_operator(os, "/", x);
  }
  template <typename L, typename R>
  static void print(std::ostream& os, const power<L, R>& x) {
    print_binary_operator(os, "**", x);
  }

  template <typename T>
  static void print(std::ostream& os, const sinus<T>& x) {
    print_function(os, "sin", x);
  }
  template <typename T>
  static void print(std::ostream& os, const cosinus<T>& x) {
    print_function(os, "cos", x);
  }
  template <typename T>
  static void print(std::ostream& os, const tangens<T>& x) {
    print_function(os, "tan", x);
  }
  template <typename T>
  static void print(std::ostream& os, const arcus_sinus<T>& x) {
    print_function(os, "asin", x);
  }
  template <typename T>
  static void print(std::ostream& os, const arcus_cosinus<T>& x) {
    print_function(os, "acos", x);
  }
  template <typename T>
  static void print(std::ostream& os, const arcus_tangens<T>& x) {
    print_function(os, "atan", x);
  }
  template <typename T>
  static void print(std::ostream& os, const sinus_hyperbolicus<T>& x) {
    print_function(os, "sinh", x);
  }
  template <typename T>
  static void print(std::ostream& os, const cosinus_hyperbolicus<T>& x) {
    print_function(os, "cosh", x);
  }
  template <typename T>
  static void print(std::ostream& os, const tangens_hyperbolicus<T>& x) {
    print_function(os, "sin", x);
  }
  template <typename T>
  static void print(std::ostream& os, const area_sinus_hyperbolicus<T>& x) {
    print_function(os, "asinh", x);
  }
  template <typename T>
  static void print(std::ostream& os, const area_cosinus_hyperbolicus<T>& x) {
    print_function(os, "acosh", x);
  }
  template <typename T>
  static void print(std::ostream& os, const area_tangens_hyperbolicus<T>& x) {
    print_function(os, "atanh", x);
  }
  template <typename T>
  static void print(std::ostream& os, const exponential<T>& x) {
    print_function(os, "exp", x);
  }
  template <typename T>
  static void print(std::ostream& os, const logarithm<T>& x) {
    print_function(os, "log", x);
  }
  template <typename T>
  static void print(std::ostream& os, const square_root<T>& x) {
    print_function(os, "sqrt", x);
  }

  template <typename T>
  static void print(std::ostream& os, const negation<T>& x) {
    if constexpr (detail::is_binary_operator_v<std::decay_t<decltype(x.arg)>>) {
      os << "-(";
      print(os, x.arg);
      os << ')';
    }
    else {
      os << '-';
      print(os, x.arg);
    }
  }
};

template <typename T>
concept expression_like = is_expression_v<T>;

std::ostream& operator<<(std::ostream& os, const expression_like auto& x) {
  print_impl::print(os, x);
  return os;
}
} // namespace detail

std::string to_string(const detail::expression_like auto& x) {
  std::ostringstream oss;
  oss << x;
  return std::move(oss).str();
}
} // namespace ad

#endif // AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_
