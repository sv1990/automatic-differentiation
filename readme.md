# AD: Automatic differentiation

This library allows you to take the exact derivative of any function at compile time.

## Examples

The library is most easily described by the following two examples.

### One-dimensional

```c++
// Define a variable name
constexpr auto x = ad::_0;

// Define the function
constexpr auto f = (x + 1) * (x - 1);

// Take the first two derivatives
constexpr auto df  = f.derive();
constexpr auto d2f = df.derive();

std::cout << d2f(5) << '\n';
```

It is also possible to use the functions

* `ad::cos`
* `ad::exp`
* `ad::pow`
* `ad::sin`
* `ad::sqrt`
* `ad::tan`
* `ad::log`

### Multidimensional

```C++
// Define a variable names
constexpr auto x = ad::0;
constexpr auto y = ad::1;

// Define the function
constexpr auto f = x * x * y;

// Take the first derivatives
constexpr auto dxf = f.derive(x);
constexpr auto dyf = f.derive(y); // Alternatively f.derive<1>()

// Take the mixed sedond derivative (Syntax works only with gcc)
constexpr auto dxyf = f.derive(x, y);
```

