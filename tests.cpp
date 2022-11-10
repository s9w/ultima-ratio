#include "ultima_ratio.h"

using namespace ultima_ratio;

// Comparison with integer
template<std::integral T>
using int_comparable_ratio = ratio<T, make_int_comparable>;
static_assert(int_comparable_ratio<int>::is_int_comparable);
static_assert(int_comparable_ratio(2, 1) == 2);
static_assert(int_comparable_ratio(4, 2) == 2);
static_assert(2 == int_comparable_ratio(2, 1));

// Comparison between ratios of different type
template<std::integral T>
using hetero_comparable_ratio = ratio<T, make_hetero_comparable>;
static_assert(hetero_comparable_ratio<int>::is_hetero_comparable);
static_assert(hetero_comparable_ratio(1, 1) == hetero_comparable_ratio(1ul, 1ul));

// Traits and concepts
static_assert(std::is_trivially_copyable_v<ratio<int>>);
static_assert(is_ratio_v<ratio<int>>);
static_assert(is_ratio_v<int> == false);

// Default ctor is int and one
static_assert(std::same_as<decltype(ratio{})::value_type, int > );
static_assert(ratio{} == ratio{1,1});

// Types of ctor parameters are used
static_assert(std::same_as<typename decltype(ratio{ 1ul, 1ul })::value_type, unsigned long > );

// get_fp()
static_assert(ratio{ 1, 2 }.get_fp<float>() == 0.5f);

// Make sure elements are minimal when requested
static_assert(ratio{ 8,2 }.num() == 8);
static_assert(ratio{ 8,2 }.denom() == 2);
template<std::integral T>
using normalized_ratio = ratio<T, make_normalized>;
static_assert(normalized_ratio{ 8,2 }.num() == 4);
static_assert(normalized_ratio{ 8,2 }.denom() == 1);

// Multiplication with int -> int
static_assert(ratio{ 2,1 } *5 == 10);
static_assert(5 * ratio{ 2,1 } == 10);
static_assert(ratio{ 1, 2 } *10 == 5);

// Multiplication of two ratios
static_assert(ratio(2, 1)* ratio(2, 1) == ratio(4, 1));
static_assert(ratio(6, 3)* ratio(6, 2) == ratio(36, 6));

// Multiplication with fp -> fp
static_assert(ratio(2, 1) * 2.0f == 4.0f);
static_assert(2.0f * ratio(2, 1) == 4.0f);

// Integer Division
static_assert(4 / ratio(2,1) == 2);
static_assert(ratio(2,1) / 2 == 1);

// Comparisons
static_assert(ratio(2, 1) != ratio(2,2));
static_assert(ratio(1,4) < ratio(1,3));
static_assert(ratio(1,3) > ratio(1,4));
static_assert(ratio(1,3) >= ratio(1,4));
static_assert(ratio(1,3) >= ratio(1,3));
static_assert(ratio(1,4) < ratio(1,4) == false);
static_assert(ratio(1,4) > ratio(1,4) == false);

// Implicit conversion to floating point types
constexpr auto accept_float(const float value) -> float
{
   return value;
}
constexpr auto accept_double(const double value) -> double
{
   return value;
}
static_assert(std::is_convertible_v<ratio<int>, float> == false);
static_assert(std::is_convertible_v<ratio<int>, double> == false);
static_assert(std::is_convertible_v<ratio<int, make_implicit_convertible>, float> == true);
static_assert(std::is_convertible_v<ratio<int, make_implicit_convertible>, double> == true);
static_assert(accept_float(ratio<int, make_implicit_convertible>(1,2)) == 0.5f);
static_assert(accept_double(ratio<int, make_implicit_convertible>(1,2)) == 0.5);

// Zero tests
static_assert(ratio(0, 1).num() == 0);
static_assert(ratio(0, 1).get_fp<float>() == 0.0f);
static_assert(ratio(0, 3).denom() == 3);
static_assert(normalized_ratio(0, 3).denom() == 1);


template<typename ex_type, typename fun_type>
auto expect_throw(const fun_type& fun) -> void
{
   bool has_thrown = false;
   try
   {
      fun();
   }
   catch (const ex_type&)
   {
      has_thrown = true;
   }
   if (has_thrown == false)
      std::terminate();
}


int main()
{
   constexpr ratio half{1, 2};

   // The type is immutable so you can only _read_ the numerator and denominator
   constexpr auto numerator = half.num();
   constexpr auto denominator = half.denom();

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

   expect_throw<ur_ex_remainder>([]() { const auto x = ratio{ 3,2 } *1; });
   expect_throw<ur_ex_remainder>([]() { const auto x = 1 * ratio{ 3,2 }; });
   expect_throw<ur_ex_remainder>([]() { const auto x = 4 / ratio{ 3,2 }; });
   expect_throw<ur_ex_remainder>([]() { const auto x = ratio{ 4,2 } / 3; });

   return 0;
}
