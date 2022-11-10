#include "ultima_ratio.h"

using namespace ultima_ratio;

// Comparison with integer
template<std::integral T>
using int_comparable_ratio = ratio<T, make_int_comparable>;
static_assert(int_comparable_ratio<int>::is_int_comparable);
static_assert(int_comparable_ratio(2, 1) == 2);
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
   expect_throw<ur_ex_remainder>([]() { const auto x = ratio{ 3,2 } *1; });
   expect_throw<ur_ex_remainder>([]() { const auto x = 1 * ratio{ 3,2 }; });
   expect_throw<ur_ex_remainder>([]() { const auto x = 4 / ratio{ 3,2 }; });
   expect_throw<ur_ex_remainder>([]() { const auto x = ratio{ 4,2 } / 3; });

   return 0;
}
