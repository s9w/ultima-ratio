#pragma once

#include <ratio>
#include <concepts>

namespace frac
{
   template<std::integral T>
   struct fraction
   {
   private:
      T m_num = static_cast<T>(1);
      T m_denom = static_cast<T>(1);

   public:
      using value_type = T;

      constexpr explicit fraction() = default;
      constexpr explicit fraction(const T num, const T denom)
         : m_num(num)
         , m_denom(denom)
      {
         if (denom == static_cast<T>(0))
         {
            // TODO
         }
         if (denom < static_cast<T>(0))
         {
            // TODO
         }
      }

      template<std::intmax_t num, std::intmax_t denom>
      constexpr explicit fraction(const std::ratio<num, denom>)
         : fraction(num, denom)
      {}

      template<std::floating_point ratio_type>
      [[nodiscard]] constexpr auto get_ratio() const -> ratio_type
      {
         return static_cast<ratio_type>(m_num) / static_cast<ratio_type>(m_denom);
      }

      [[nodiscard]] constexpr auto num() const noexcept -> T { return m_num; }
      [[nodiscard]] constexpr auto denom() const noexcept -> T { return m_denom; }
   };

   // Operators
   template<std::integral ratio_value_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const fraction<ratio_value_type>& ratio, const other_type other) -> other_type;
   template<std::integral ratio_value_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const fraction<ratio_value_type>& ratio)->other_type;

   // CDAT
   fraction()->fraction<int>;
   template<std::intmax_t num, std::intmax_t denom>
   fraction(const std::ratio<num, denom>)->fraction<std::intmax_t>;
}


template<std::integral ratio_value_type, std::integral other_type>
constexpr auto frac::operator*(const fraction<ratio_value_type>& ratio, const other_type other) -> other_type
{
   if ((other * ratio.num()) % ratio.denom() != 0)
   {
      // TODO
      // std::terminate();
   }
   return other * ratio.num() / ratio.denom();
}


template<std::integral ratio_value_type, std::integral other_type>
constexpr auto frac::operator*(const other_type other, const fraction<ratio_value_type>& ratio) -> other_type
{
   return ratio * other;
}


