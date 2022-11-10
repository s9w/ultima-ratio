#pragma once

#include <concepts>
#include <numeric>
#include <ratio>
#include <stdexcept>


namespace ultima_ratio
{
   namespace details
   {
      template<typename T, typename ... types>
      concept t_in_types = (std::same_as<T, types> || ...);

      struct reduce_tag{};
      struct dont_reduce_tag{};
      template<bool do_normalize>
      inline static constexpr auto init_tag_v = std::conditional_t<do_normalize, reduce_tag, dont_reduce_tag>{};
   }

   // Modifiers
   struct make_int_comparable{};
   struct make_fp_comparable {};
   struct make_hetero_comparable{};
   struct make_reduced{};
   struct make_implicit_convertible{};

   // Exception types
   struct error : std::runtime_error { using runtime_error::runtime_error; };
   struct denom_zero_error : error { using error::error; };
   struct negative_error : error { using error::error; };
   struct remainder_error : error { using error::error; };

   // The main star
   template<std::integral T, typename ... modifiers>
   struct ratio
   {
   private:
      T m_num = static_cast<T>(1);
      T m_denom = static_cast<T>(1);

   public:
      using value_type = T;
      constexpr static inline bool is_int_comparable = details::t_in_types<make_int_comparable, modifiers...>;
      constexpr static inline bool is_fp_comparable = details::t_in_types<make_fp_comparable, modifiers...>;
      constexpr static inline bool is_hetero_comparable = details::t_in_types<make_hetero_comparable, modifiers...>;
      constexpr static inline bool is_reduced = details::t_in_types<make_reduced, modifiers...>;
      constexpr static inline bool is_implicit_convertible = details::t_in_types<make_implicit_convertible, modifiers...>;

      constexpr explicit ratio() = default;

      constexpr explicit ratio(const T num, const T denom)
         : ratio(details::init_tag_v<is_reduced>, num, denom)
      {
         if (denom == static_cast<T>(0))
         {
            throw denom_zero_error{"denominator is zero"};
         }
         if (num < static_cast<T>(0) || denom < static_cast<T>(0))
         {
            throw negative_error{ "Fraction is negative" };
         }
      }


      template<std::intmax_t num, std::intmax_t denom>
      constexpr explicit ratio(const std::ratio<num, denom>)
         : ratio(num, denom)
      {}


      // Implicit conversion operators
      constexpr operator float() const requires(is_implicit_convertible)
      {
         return this->get_fp<float>();
      }
      constexpr operator double() const requires(is_implicit_convertible)
      {
         return this->get_fp<double>();
      }


      template<std::floating_point fp_type>
      [[nodiscard]] constexpr auto get_fp() const -> fp_type // TODO: noexcept
      {
         return static_cast<fp_type>(m_num) / static_cast<fp_type>(m_denom);
      }

      [[nodiscard]] constexpr auto num() const noexcept -> T { return m_num; }
      [[nodiscard]] constexpr auto denom() const noexcept -> T { return m_denom; }

   private:
      constexpr explicit ratio(details::reduce_tag, const T num, const T denom)
         : m_num(num / std::gcd(num, denom))
         , m_denom(denom / std::gcd(num, denom))
      { }
      constexpr explicit ratio(details::dont_reduce_tag, const T num, const T denom)
         : m_num(num)
         , m_denom(denom)
      { }
   };

   // Trait/concept for ratio
   template<typename T>
   struct is_ratio : std::false_type {};
   template<std::integral T, typename ...Args>
   struct is_ratio<ratio<T, Args...>> : std::true_type {};
   template<typename T>
   inline constexpr bool is_ratio_v = is_ratio<T>::value;
   template<typename T>
   concept ratio_c = is_ratio_v<T>;


   // Multiplication with integer
   template<ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& left, const other_type right) -> other_type
   {
      if ((right * left.num()) % left.denom() != 0)
      {
         throw remainder_error{ "Multiplication with integer leaves a remainder" };
      }
      return right * left.num() / left.denom();
   }
   template<ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const other_type left, const ratio_type& right) -> other_type
   {
      return right * left;
   }


   // Multiplication with floating points
   template<ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& left, const other_type right) -> other_type
   {
      return left.template get_fp<other_type>() * right;
   }
   template<ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const other_type left, const ratio_type& right) -> other_type
   {
      return right * left;
   }


   // Division with floating points
   template<ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator/(const ratio_type& left, const other_type right) -> other_type
   {
      return left.template get_fp<other_type>() / right;
   }
   template<ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator/(const other_type left, const ratio_type& right) -> other_type
   {
      return left / right.template get_fp<other_type>();
   }


   // Multiplication between ratios
   template<ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& left, const ratio_type& right) -> ratio_type
   {
      return ratio(left.num() * right.num(), left.denom() * right.denom());
   }



   // Division with integer
   template<ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator/(const other_type left, const ratio_type& right) -> other_type
   {
      if ((left * right.denom()) % right.num() != 0)
      {
         throw remainder_error{ "Integer division leaves a remainder" };
      }
      return left * right.denom() / right.num();
   }
   template<ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator/(const ratio_type& left, const other_type right) -> other_type
   {
      return right / left;
   }


   // Comparison between ratios
   template<ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator==(const ratio_type& left, const ratio_type& right) -> bool
   {
      return left.num() == right.num() && left.denom() == right.denom();
   }
   template<ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator<(const ratio_type& left, const ratio_type& right) -> bool
   {
      const auto lcm = std::lcm(left.denom(), right.denom());
      const auto left_normalized = left.num() * lcm / left.denom();
      const auto right_normalized = right.num() * lcm / right.denom();
      return left_normalized < right_normalized;
   }
   template<ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator>(const ratio_type& left, const ratio_type& right) -> bool
   {
      return right < left;
   }
   template<ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator<=(const ratio_type& left, const ratio_type& right) -> bool
   {
      return left < right || left == right;
   }
   template<ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator>=(const ratio_type& left, const ratio_type& right) -> bool
   {
      return left > right || left == right;
   }


   // Comparison between ratios of different type
   template<ratio_c ratio_type_a, ratio_c ratio_type_b>
   requires(ratio_type_a::is_hetero_comparable && ratio_type_b::is_hetero_comparable)
   [[nodiscard]] constexpr auto operator==(const ratio_type_a& left, const ratio_type_b& right) -> bool
   {
      return static_cast<int>(left.num()) == static_cast<int>(right.num()) &&
         static_cast<int>(right.denom()) == static_cast<int>(right.denom());
   }


   // Comparison with integer
   template<ratio_c ratio_type, std::integral other_type>
   requires(ratio_type::is_int_comparable)
   [[nodiscard]] constexpr auto operator==(const ratio_type& left, const other_type right) -> bool
   {
      // A/B == C  <=> A == B * C
      return left.num() == left.denom() * right;
   }


   // Comparison with floating point types
   template<ratio_c ratio_type, std::floating_point other_type>
   requires(ratio_type::is_fp_comparable)
   [[nodiscard]] constexpr auto operator==(const ratio_type& left, const other_type right) -> bool
   {
      return left.get_fp<other_type>() == right;
   }
   template<ratio_c ratio_type, std::floating_point other_type>
   requires(ratio_type::is_fp_comparable)
   [[nodiscard]] constexpr auto operator<(const ratio_type& left, const other_type right) -> bool
   {
      return left.get_fp<other_type>() < right;
   }
   template<ratio_c ratio_type, std::floating_point other_type>
   requires(ratio_type::is_fp_comparable)
   [[nodiscard]] constexpr auto operator<=(const ratio_type& left, const other_type right) -> bool
   {
      return left.get_fp<other_type>() <= right;
   }
   template<ratio_c ratio_type, std::floating_point other_type>
   requires(ratio_type::is_fp_comparable)
   [[nodiscard]] constexpr auto operator<(const other_type left, const ratio_type& right) -> bool
   {
      return left < right.get_fp<other_type>();
   }
   template<ratio_c ratio_type, std::floating_point other_type>
   requires(ratio_type::is_fp_comparable)
   [[nodiscard]] constexpr auto operator<=(const other_type left, const ratio_type& right) -> bool
   {
      return left <= right.get_fp<other_type>();
   }


   // CDAT
   ratio() -> ratio<int>;
   template<std::intmax_t num, std::intmax_t denom>
   ratio(const std::ratio<num, denom>) -> ratio<std::intmax_t>;

} // namespace ultima_ratio

// TODO: tiny types edge cases. maybe special paths for one- components?
// TODO: godbolt conformance check
