#pragma once

#include <stdexcept>
#include <ratio>
#include <numeric>
#include <concepts>


namespace rat
{
   template<typename T, typename ... types>
   concept t_in_types = (std::same_as<T, types> || ...); // TODO in detail ns

   struct int_comparable;

   struct ur_exception final : std::runtime_error { using runtime_error::runtime_error; };

   template<std::integral T, typename ... modifiers>
   struct ratio
   {
   private:
      T m_num = static_cast<T>(1);
      T m_denom = static_cast<T>(1);

   public:
      using value_type = T;

      constexpr explicit ratio() = default;

      constexpr explicit ratio(const T num, const T denom)
         : m_num(num / std::gcd(num, denom))
         , m_denom(denom / std::gcd(num, denom))
      {
         if (denom == static_cast<T>(0))
         {
            throw ur_exception{"denominator is zero"};
         }
         if (denom < static_cast<T>(0))
         {
            throw ur_exception{ "denominator is negative" };
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
   template<rat::ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& ratio, const other_type other) -> other_type
   {
      if ((other * ratio.num()) % ratio.denom() != 0)
      {
         throw ur_exception{ "Multiplication with integer leaves a remainder" };
      }
      return other * ratio.num() / ratio.denom();
   }
   template<rat::ratio_c ratio_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const ratio_type& ratio) -> other_type
   {
      return ratio * other;
   }

   // Multiplication with floating points
   template<rat::ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& ratio, const other_type other) -> other_type
   {
      return ratio.template get_fp<other_type>() * other;
   }
   template<rat::ratio_c ratio_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const ratio_type& ratio) -> other_type
   {
      return ratio * other;
   }

   // Multiplication between ratios
   template<rat::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator*(const ratio_type& a, const ratio_type& b) -> ratio_type
   {
      return ratio(a.num() * b.num(), a.denom() * b.denom());
   }

   // Comparison between ratios
   template<rat::ratio_c ratio_type>
   [[nodiscard]] constexpr auto operator==(const ratio_type& a, const ratio_type& b) -> bool
   {
      return a.num() == b.num() && b.denom() == b.denom();
   }

   // Comparison with integer
   template<std::integral other_type, std::integral T, typename ... modifiers>
   requires(t_in_types<int_comparable, modifiers...>)
   [[nodiscard]] constexpr auto operator==(const ratio<T, modifiers...>& a, const other_type other) -> bool
   {
      return a.denom() == static_cast<T>(1) && a.num() == other;
   }

   // CDAT
   ratio() -> ratio<int>;
   template<std::intmax_t num, std::intmax_t denom>
   ratio(const std::ratio<num, denom>) -> ratio<std::intmax_t>;
}
