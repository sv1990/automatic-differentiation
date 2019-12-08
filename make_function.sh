if [ -z "$1" ]; then
  exit
fi

if [ -z "$2" ]; then
  exit
fi

struct=$1
func=$2

echo "template <typename T>"
echo "constexpr auto make_$struct(T x) noexcept {"
echo "  return $struct<T>(x);"
echo "}"

echo "template <typename T>"
echo "struct $struct : function_expression<$struct<T>> {"
echo "  using function_expression<$struct>::derive;"
echo "  friend struct function_expression<$struct<T>>;"
echo "  [[no_unique_address]] T arg;"
echo "  constexpr explicit $struct(T x) noexcept : arg(x) {}"
echo "  template <typename... Ts>"
echo "  constexpr double operator()(Ts... xs) const noexcept {"
echo "    return std::$func(arg(xs...));"
echo "  }"

echo "private:"
echo "  constexpr auto derive_outer() const noexcept { return /*TODO*/ arg; }"
echo "};"

echo "template <typename T>"
echo "constexpr auto $func(T x) noexcept {"
echo "  return make_$struct(x);"
echo "}"
