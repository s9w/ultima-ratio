#pragma once

#include <stdexcept>
#include <ratio>
#include <numeric>
#include <concepts>


namespace rat
{
   struct ur_exception final : std::runtime_error { using runtime_error::runtime_error; };

   template<std::integral T>
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
      [[nodiscard]] constexpr auto get_fp() const -> fp_type;

      [[nodiscard]] constexpr auto num() const noexcept -> T { return m_num; }
      [[nodiscard]] constexpr auto denom() const noexcept -> T { return m_denom; }
   };

   // Multiplication with integer
   template<std::integral ratio_value_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const ratio<ratio_value_type>& ratio, const other_type other) -> other_type;
   template<std::integral ratio_value_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const ratio<ratio_value_type>& ratio) -> other_type;

   // Multiplication with floating points
   template<std::integral ratio_value_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const ratio<ratio_value_type>& ratio, const other_type other) -> other_type;
   template<std::integral ratio_value_type, std::floating_point other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const ratio<ratio_value_type>& ratio) -> other_type;

   // Multiplication between ratios
   template<std::integral ratio_value_type>
   [[nodiscard]] constexpr auto operator*(const ratio<ratio_value_type>& a, const ratio<ratio_value_type>& b) -> ratio<ratio_value_type>;

   // Comparison
   template<std::integral ratio_value_type>
   [[nodiscard]] constexpr auto operator==(const ratio<ratio_value_type>& a, const ratio<ratio_value_type>& b) -> bool;

   // CDAT
   ratio() -> ratio<int>;
   template<std::intmax_t num, std::intmax_t denom>
   ratio(const std::ratio<num, denom>) -> ratio<std::intmax_t>;
}


template<std::integral ratio_value_type, std::integral other_type>
constexpr auto rat::operator*(const ratio<ratio_value_type>& ratio, const other_type other) -> other_type
{
   if ((other * ratio.num()) % ratio.denom() != 0)
   {
      throw ur_exception{ "Multiplication with integer leaves a remainder" };
   }
   return other * ratio.num() / ratio.denom();
}


template<std::integral ratio_value_type, std::integral other_type>
constexpr auto rat::operator*(const other_type other, const ratio<ratio_value_type>& ratio) -> other_type
{
   return ratio * other;
}


template<std::integral ratio_value_type>
constexpr auto rat::operator*(
   const ratio<ratio_value_type>& a,
   const ratio<ratio_value_type>& b
) -> ratio<ratio_value_type>
{
   return ratio(a.num() * b.num(), a.denom() * b.denom());
}


template<std::integral ratio_value_type>
constexpr auto rat::operator==(const ratio<ratio_value_type>& a, const ratio<ratio_value_type>& b) -> bool
{
   return a.num() == b.num() && b.denom() == b.denom();
}



template<std::integral ratio_value_type, std::floating_point other_type>
constexpr auto rat::operator*(const ratio<ratio_value_type>& ratio, const other_type other) -> other_type
{
   return ratio.template get_fp<other_type>() * other;
}


template<std::integral ratio_value_type, std::floating_point other_type>
constexpr auto rat::operator*(const other_type other, const ratio<ratio_value_type>& ratio) -> other_type
{
   return ratio * other;
}


template<std::integral T>
template<std::floating_point fp_type>
constexpr auto rat::ratio<T>::get_fp() const -> fp_type
{
   return static_cast<fp_type>(m_num) / static_cast<fp_type>(m_denom);
}
