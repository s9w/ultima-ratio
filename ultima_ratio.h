#pragma once

#include <concepts>
#include <numeric>
#include <ratio>
#include <stdexcept>


namespace ultima_ratio
{
   template<typename T, typename ... types>
   concept t_in_types = (std::same_as<T, types> || ...); // TODO in detail ns

   struct int_comparable;
   struct hetero_comparable;

   struct ur_exception : std::runtime_error { using runtime_error::runtime_error; };
   struct ur_ex_denom_zero : ur_exception { using ur_exception::ur_exception;   };
   struct ur_ex_negative : ur_exception { using ur_exception::ur_exception;   };
   struct ur_ex_remainder : ur_exception { using ur_exception::ur_exception;   };

   template<std::integral T, typename ... modifiers>
   struct ratio
   {
   private:
      T m_num = static_cast<T>(1);
      T m_denom = static_cast<T>(1);

   public:
      using value_type = T;
      constexpr static inline bool is_int_comparable = t_in_types<int_comparable, modifiers...>;
      constexpr static inline bool is_hetero_comparable = t_in_types<hetero_comparable, modifiers...>;

      constexpr explicit ratio() = default;

      constexpr explicit ratio(const T num, const T denom)
         : m_num(num / std::gcd(num, denom))
         , m_denom(denom / std::gcd(num, denom))
      {
         if (denom == static_cast<T>(0))
         {
            throw ur_ex_denom_zero{"denominator is zero"};
         }
         if (num < static_cast<T>(0) || denom < static_cast<T>(0))
         {
            throw ur_ex_negative{ "denominator is negative" };
         }
      }


      template<std::intmax_t num, std::intmax_t denom>
      constexpr explicit ratio(const std::ratio<num, denom>)
         : ratio(num, denom)
      {}


      template<std::floating_point fp_type>
      [[nodiscard]] constexpr auto get_fp() const -> fp_type // TODO: noexcept
      {
         return static_cast<fp_type>(m_num) / static_cast<fp_type>(m_denom);
      }


      [[nodiscard]] constexpr auto num() const noexcept -> T { return m_num; }
      [[nodiscard]] constexpr auto denom() const noexcept -> T { return m_denom; }
   };


   template<typename T>
   struct is_ratio : std::false_type {};
   template<std::integral T, typename ...Args>
   struct is_ratio<ratio<T, Args...>> : std::true_type {};
   template<typename T>
   inline constexpr bool is_ratio_v = is_ratio<T>::value;
   template<typename T>
   concept ratio_c = is_ratio_v<T>;


   // Multiplication with integer
   template<ultima_ratio::ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& ratio, const other_type other) -> other_type
   {
      if ((other * ratio.num()) % ratio.denom() != 0)
      {
         throw ur_ex_remainder{ "Multiplication with integer leaves a remainder" };
      }
      return other * ratio.num() / ratio.denom();
   }
   template<ultima_ratio::ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const ratio_type& ratio) -> other_type
   {
      return ratio * other;
   }


   // Division with integer
   template<ultima_ratio::ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator/(const other_type other, const ratio_type& ratio) -> other_type
   {
      if ((other * ratio.denom()) % ratio.num() != 0)
      {
         throw ur_ex_remainder{ "Integer division leaves a remainder" };
      }
      return other * ratio.denom() / ratio.num();
   }
   template<ultima_ratio::ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator/(const ratio_type& ratio, const other_type other) -> other_type
   {
      return other / ratio;
   }


   // Multiplication with floating points
   template<ultima_ratio::ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& ratio, const other_type other) -> other_type
   {
      return ratio.template get_fp<other_type>() * other;
   }
   template<ultima_ratio::ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const ratio_type& ratio) -> other_type
   {
      return ratio * other;
   }


   // Multiplication between ratios
   template<ultima_ratio::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& a, const ratio_type& b) -> ratio_type
   {
      return ratio(a.num() * b.num(), a.denom() * b.denom());
   }


   // Comparison between ratios
   template<ultima_ratio::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator==(const ratio_type& left, const ratio_type& right) -> bool
   {
      return left.num() == right.num() && right.denom() == right.denom();
   }
   template<ultima_ratio::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator<(const ratio_type& left, const ratio_type& right) -> bool
   {
      const auto lcm = std::lcm(left.denom(), right.denom());
      const auto left_normalized = left.num() * lcm / left.denom();
      const auto right_normalized = right.num() * lcm / right.denom();
      return left_normalized < right_normalized;
   }
   template<ultima_ratio::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator>(const ratio_type& left, const ratio_type& right) -> bool
   {
      return right < left;
   }
   template<ultima_ratio::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator<=(const ratio_type& left, const ratio_type& right) -> bool
   {
      return left < right || left == right;
   }
   template<ultima_ratio::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator>=(const ratio_type& left, const ratio_type& right) -> bool
   {
      return left > right || left == right;
   }


   // Comparison between ratios of different type
   template<ultima_ratio::ratio_c ratio_type_a, ultima_ratio::ratio_c ratio_type_b>
   requires(ratio_type_a::is_hetero_comparable && ratio_type_b::is_hetero_comparable)
   [[nodiscard]] constexpr auto operator==(const ratio_type_a& a, const ratio_type_b& b) -> bool
   {
      return static_cast<int>(a.num()) == static_cast<int>(b.num()) && static_cast<int>(b.denom()) == static_cast<int>(b.denom());
   }


   // Comparison with integer
   template<ultima_ratio::ratio_c ratio_type, std::integral other_type>
   requires(ratio_type::is_int_comparable)
   [[nodiscard]] constexpr auto operator==(const ratio_type& a, const other_type other) -> bool
   {
      return a.denom() == static_cast<ratio_type::value_type>(1) && a.num() == other;
   }


   // CDAT
   ratio() -> ratio<int>;
   template<std::intmax_t num, std::intmax_t denom>
   ratio(const std::ratio<num, denom>) -> ratio<std::intmax_t>;
}

// todo: construction from fp types if possible
