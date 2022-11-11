# Ultima ratio
An origin story: You have a simple problem:
```c++
constexpr int base_value = 4;
int factor = 2;
int value = factor * base_value;
```

At some point you want the factor to be <1, maybe 1/2. Changing from multiplication to division is correct, but annoying. You could use a float and cast your way to the result. But you know it's dangerous as that only works for a small subset of values.

*Surely* the C++ committee solved this everyday issue gracefully. You leave confused after coming across [`std::ratio`](https://en.cppreference.com/w/cpp/numeric/ratio/ratio).
You decide to take matters into your own hands and write your own `ratio` class. You spend some time thinking about edge cases, convenience functions, making everything `constexpr`, comparisons, error handling and all those goodies.

It's the **ultima ratio**.

## Details
[`ultima_ratio.h`](ultima_ratio.h) is a single-header C++20 library. It provides the type `ratio`. **All** functions are `constexpr`.

```c++
#include <ultima_ratio.h>
using namespace ultima_ratio;

// Construction by two integral values. Their type dictates the value_type
constexpr ratio half{1, 2};
static_assert(std::same_as<decltype(half)::value_type, int>);

// The type is immutable so you can only _read_ the numerator and denominator
constexpr auto numerator = half.num();
constexpr auto denominator = half.denom();

// If you really want, you can get a floating point representation
static_assert(half.get_fp<double>() == 0.5);
```

Arithmetic operations work as expected
```c++
// Multiplication and division with integers, both ways. Returns integer type
static_assert(half * 4 == 2);
static_assert(4 * half == 2);
static_assert(4 / half == 8);
static_assert(ratio(3,1) / 3 == 1);

// But: These operations don't always go without a remainder. This is caught and an exception thrown!
try { ratio{3,2} * 1; }
catch(const ur_ex_remainder&){ }

// Multiplication and division with floating point values works as expected
static_assert(half * 2.0f == 1.0f);
static_assert(2.0 / half == 4.0);

// Multiplication between ratios themselves work and yields another ratio
static_assert(ratio(4,3) * ratio(1,2) == ratio(4,6));
```

But there's even more. You can customize the types behavior:
```c++
// By default, comparisons only work between two ratio objects of the same value_type
static_assert(ratio(3, 2) > ratio(2, 2));

// If you want to compare ratios of different value_type, go ham:
template<std::integral T>
using hetero_comparable_ratio = ratio<T, make_hetero_comparable>;
static_assert(hetero_comparable_ratio(1, 1) == hetero_comparable_ratio(1ul, 1ul));

// Use make_int_comparable to allow equality comparison with ints
template<std::integral T>
using int_comparable_ratio = ratio<T, make_int_comparable>;
static_assert(int_comparable_ratio(4, 2) == 2);

// Use make_fp_comparable to allow comparisons with floating point types
template<std::integral T>
using fp_comparable_ratio = ratio<T, make_fp_comparable>;
static_assert(fp_comparable_ratio(3, 6) == 0.5);
static_assert(fp_comparable_ratio(1, 3) < 0.5);
static_assert(fp_comparable_ratio(1, 2) <= 0.5);
static_assert(0.5 < fp_comparable_ratio(7, 3));
static_assert(0.5 <= fp_comparable_ratio(7, 3));

// Numerator and denominator can be reduced if desired:
template<std::integral T>
using reduced_ratio = ratio<T, make_reduced>;
static_assert(reduced_ratio(4, 2).num() == 2 && reduced_ratio(4, 2).denom() == 1);

// Ratios can also be made implicitly convertible to floating point types
template<std::integral T>
using converting_ratio = ratio<T, make_implicit_convertible>;
const float f = converting_ratio(1, 2);

// You can mix all these properties to construct your dream type
namespace my_namespace
{
   template<std::integral T>
   using ratio = ultima_ratio::ratio<T, make_reduced, make_int_comparable, make_fp_comparable>;
}
```

Oh and you can also construct a `ratio` from a `std::ratio`. Just be aware that its integer type is carried over, which is a `std::intmax_t`.

## Error handling
There's a couple of things that get caught:
- A denominator of zero is illegal, your teacher was right. Throws `ultima_ratio::denom_zero_error`
- Neither numerator nor denominator can be negative. Throws `ultima_ratio::negative_error`
- Multiplication and division with integers can leave a remainder. This is considered an error and throws `ultima_ratio::remainder_error`
- All exceptions are inherited from `ultima_ratio::error` which is inherited from `std::runtime_error`
