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
struct to_string_impl {
  template <typename T, std::enable_if_t<is_constant_v<T>>* = nullptr>
  static std::string to_string(const T& x) {
    return std::to_string(x.value());
  }
  template <std::size_t N>
  static std::string to_string(const variable<N>&) {
    return concat("x", N);
  }

private:
  template <typename T>
  static std::string un_op(std::string_view op, const T x) {
    return concat('(', op, ' ', to_string(x), ')');
  }
  template <typename L, typename R>
  static std::string bin_op(std::string_view op, const L l, const R& r) {
    return concat('(', op, ' ', to_string(l), ' ', to_string(r), ')');
  }

public:
  template <typename L, typename R>
  static std::string to_string(const addition<L, R>& x) {
    return bin_op("+", x.lhs, x.rhs);
  }
  template <typename L, typename R>
  static std::string to_string(const subtraction<L, R>& x) {
    return bin_op("-", x.lhs, x.rhs);
  }
  template <typename L, typename R>
  static std::string to_string(const multiplication<L, R>& x) {
    return bin_op("*", x.lhs, x.rhs);
  }
  template <typename L, typename R>
  static std::string to_string(const division<L, R>& x) {
    return bin_op("/", x.lhs, x.rhs);
  }
  template <typename L, typename R>
  static std::string to_string(const power<L, R>& x) {
    return bin_op("^", x.lhs, x.rhs);
  }

  template <typename T>
  static std::string to_string(const sinus<T>& x) {
    return un_op("sin", x.arg);
  }
  template <typename T>
  static std::string to_string(const cosinus<T>& x) {
    return un_op("cos", x.arg);
  }
  template <typename T>
  static std::string to_string(const tangens<T>& x) {
    return un_op("tan", x.arg);
  }
  template <typename T>
  static std::string to_string(const arcus_sinus<T>& x) {
    return un_op("asin", x.arg);
  }
  template <typename T>
  static std::string to_string(const arcus_cosinus<T>& x) {
    return un_op("acos", x.arg);
  }
  template <typename T>
  static std::string to_string(const arcus_tangens<T>& x) {
    return un_op("atan", x.arg);
  }
  template <typename T>
  static std::string to_string(const sinus_hyperbolicus<T>& x) {
    return un_op("sinh", x.arg);
  }
  template <typename T>
  static std::string to_string(const cosinus_hyperbolicus<T>& x) {
    return un_op("cosh", x.arg);
  }
  template <typename T>
  static std::string to_string(const tangens_hyperbolicus<T>& x) {
    return un_op("sin", x.arg);
  }
  template <typename T>
  static std::string to_string(const area_sinus_hyperbolicus<T>& x) {
    return un_op("asinh", x.arg);
  }
  template <typename T>
  static std::string to_string(const area_cosinus_hyperbolicus<T>& x) {
    return un_op("acosh", x.arg);
  }
  template <typename T>
  static std::string to_string(const area_tangens_hyperbolicus<T>& x) {
    return un_op("atanh", x.arg);
  }
  template <typename T>
  static std::string to_string(const exponential<T>& x) {
    return un_op("exp", x.arg);
  }
  template <typename T>
  static std::string to_string(const logarithm<T>& x) {
    return un_op("log", x.arg);
  }
  template <typename T>
  static std::string to_string(const square_root<T>& x) {
    return un_op("sqrt", x.arg);
  }
};
} // namespace detail

template <typename T, std::enable_if_t<detail::is_expression_v<T>>* = nullptr>
std::string to_string(const T& x) {
  // TODO: Transform to infix notation
  return detail::to_string_impl::to_string(x);
}
} // namespace ad

#endif // AUTOMATIC_DIFFERENTIATION_TO_STRING_HH_1580212136098641559_
