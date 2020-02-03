#ifndef AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_
#define AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_

#include "ad.hh"

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

template <typename T>
struct format_variable;
template <std::size_t N>
struct format_variable<variable<N>> {
  static inline std::string rep = detail::concat("x", N);
};

namespace detail {
struct to_string_impl {
  template <typename T, std::enable_if_t<is_constant_v<T>>* = nullptr>
  static std::string to_string(const T& x) {
    return concat(x.value());
  }
  template <std::size_t N>
  static std::string to_string(const variable<N>&) {
    return format_variable<variable<N>>::rep;
  }

private:
  template <typename T>
  static std::string format_function(std::string_view op, const T& x) {
    return concat(op, '(', to_string(x.arg), ')');
  }
  template <typename T>
  static std::string format_binary_operator(std::string_view op, const T& x) {
    const auto& lhs = x.lhs;
    const auto& rhs = x.rhs;

    const bool lhs_needs_brackets = precedence(x) > precedence(lhs);
    const bool rhs_needs_brackets = precedence(x) > precedence(rhs);
    // clang-format off
    return concat(lhs_needs_brackets ? "(" : "", to_string(lhs), lhs_needs_brackets ? ")" : "",
                  ' ', op, ' ',
                  rhs_needs_brackets ? "(" : "", to_string(rhs), rhs_needs_brackets ? ")" : "");
    // clang-format on
  }

  template <typename L, typename R>
  static int precedence(const addition<L, R>&) {
    return 1;
  }
  template <typename L, typename R>
  static int precedence(const subtraction<L, R>&) {
    return 1;
  }
  template <typename L, typename R>
  static int precedence(const multiplication<L, R>&) {
    return 2;
  }
  template <typename L, typename R>
  static int precedence(const division<L, R>&) {
    return 2;
  }
  template <typename L, typename R>
  static int precedence(const power<L, R>&) {
    return 3;
  }
  static int precedence(...) { return 4; }

public:
  template <typename L, typename R>
  static std::string to_string(const addition<L, R>& x) {
    return format_binary_operator("+", x);
  }
  template <typename L, typename R>
  static std::string to_string(const subtraction<L, R>& x) {
    return format_binary_operator("-", x);
  }
  template <typename L, typename R>
  static std::string to_string(const multiplication<L, R>& x) {
    return format_binary_operator("*", x);
  }
  template <typename L, typename R>
  static std::string to_string(const division<L, R>& x) {
    return format_binary_operator("/", x);
  }
  template <typename L, typename R>
  static std::string to_string(const power<L, R>& x) {
    return format_binary_operator("^", x);
  }

  template <typename T>
  static std::string to_string(const sinus<T>& x) {
    return format_function("sin", x);
  }
  template <typename T>
  static std::string to_string(const cosinus<T>& x) {
    return format_function("cos", x);
  }
  template <typename T>
  static std::string to_string(const tangens<T>& x) {
    return format_function("tan", x);
  }
  template <typename T>
  static std::string to_string(const arcus_sinus<T>& x) {
    return format_function("asin", x);
  }
  template <typename T>
  static std::string to_string(const arcus_cosinus<T>& x) {
    return format_function("acos", x);
  }
  template <typename T>
  static std::string to_string(const arcus_tangens<T>& x) {
    return format_function("atan", x);
  }
  template <typename T>
  static std::string to_string(const sinus_hyperbolicus<T>& x) {
    return format_function("sinh", x);
  }
  template <typename T>
  static std::string to_string(const cosinus_hyperbolicus<T>& x) {
    return format_function("cosh", x);
  }
  template <typename T>
  static std::string to_string(const tangens_hyperbolicus<T>& x) {
    return format_function("sin", x);
  }
  template <typename T>
  static std::string to_string(const area_sinus_hyperbolicus<T>& x) {
    return format_function("asinh", x);
  }
  template <typename T>
  static std::string to_string(const area_cosinus_hyperbolicus<T>& x) {
    return format_function("acosh", x);
  }
  template <typename T>
  static std::string to_string(const area_tangens_hyperbolicus<T>& x) {
    return format_function("atanh", x);
  }
  template <typename T>
  static std::string to_string(const exponential<T>& x) {
    return format_function("exp", x);
  }
  template <typename T>
  static std::string to_string(const logarithm<T>& x) {
    return format_function("log", x);
  }
  template <typename T>
  static std::string to_string(const square_root<T>& x) {
    return format_function("sqrt", x);
  }

private:
  struct format_negate {
    template <typename L, typename R>
    static std::string format(const addition<L, R>& x) {
      return concat("-(", to_string(x), ')');
    }
    template <typename L, typename R>
    static std::string format(const subtraction<L, R>& x) {
      return concat("-(", to_string(x), ')');
    }
    template <typename L, typename R>
    static std::string format(const multiplication<L, R>& x) {
      return concat("-(", to_string(x), ')');
    }
    template <typename L, typename R>
    static std::string format(const division<L, R>& x) {
      return concat("-(", to_string(x), ')');
    }
    template <typename L, typename R>
    static std::string format(const power<L, R>& x) {
      return concat("-(", to_string(x), ')');
    }
    template <typename T>
    static std::string format(const T& x) {
      return concat('-', to_string(x));
    }
  };

public:
  template <typename T>
  static std::string to_string(const negation<T>& x) {
    return format_negate::format(x.arg);
  }
};
} // namespace detail

template <typename T, std::enable_if_t<detail::is_expression_v<T>>* = nullptr>
std::string to_string(const T& x) {
  return detail::to_string_impl::to_string(x);
}
} // namespace ad

#endif // AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_
