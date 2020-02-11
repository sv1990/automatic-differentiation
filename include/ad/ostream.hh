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

template <typename T>
struct format_variable;
template <std::size_t N>
struct format_variable<variable<N>> {
  static inline std::string rep = detail::concat("x", N);
};

namespace detail {
struct print_impl {
  template <typename T, std::enable_if_t<is_constant_v<T>>* = nullptr>
  static void print(std::ostream& os, const T& x) {
    os << x.value();
  }
  template <std::size_t N>
  static void print(std::ostream& os, const variable<N>&) {
    os << format_variable<variable<N>>::rep;
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
    const bool rhs_needs_brackets = precedence(x) > precedence(rhs);

    print_with_brackets(os, lhs_needs_brackets, lhs);
    os << ' ' << op << ' ';
    print_with_brackets(os, rhs_needs_brackets, rhs);
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

private:
  struct print_negate {
    template <typename T>
    static void print_with_brackets(std::ostream& os, const T& x) {
      os << "-(";
      print(os, x);
      os << ')';
    }
    template <typename L, typename R>
    static void do_print(std::ostream& os, const addition<L, R>& x) {
      print_with_brackets(os, x);
    }
    template <typename L, typename R>
    static void do_print(std::ostream& os, const subtraction<L, R>& x) {
      print_with_brackets(os, x);
    }
    template <typename L, typename R>
    static void do_print(std::ostream& os, const multiplication<L, R>& x) {
      print_with_brackets(os, x);
    }
    template <typename L, typename R>
    static void do_print(std::ostream& os, const division<L, R>& x) {
      print_with_brackets(os, x);
    }
    template <typename L, typename R>
    static void do_print(std::ostream& os, const power<L, R>& x) {
      print_with_brackets(os, x);
    }
    template <typename T>
    static void do_print(std::ostream& os, const T& x) {
      os << '-';
      print(os, x);
    }
  };

public:
  template <typename T>
  static void print(std::ostream& os, const negation<T>& x) {
    print_negate::do_print(os, x.arg);
  }
};

template <typename E, std::enable_if_t<is_expression_v<E>>* = nullptr>
std::ostream& operator<<(std::ostream& os, const E& x) {
  print_impl::print(os, x);
  return os;
}
} // namespace detail
} // namespace ad

#endif // AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_
