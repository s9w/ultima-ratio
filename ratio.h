#pragma once

#include <concepts>
#include <numeric>
#include <ratio>


namespace rat
{
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
      constexpr explicit ratio(const std::ratio<num, denom>)
         : ratio(num, denom)
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
   [[nodiscard]] constexpr auto operator*(const ratio<ratio_value_type>& ratio, const other_type other) -> other_type;
   template<std::integral ratio_value_type, std::integral other_type>
   [[nodiscard]] constexpr auto operator*(const other_type other, const ratio<ratio_value_type>& ratio) -> other_type;
   template<std::integral ratio_value_type>
   [[nodiscard]] constexpr auto operator*(const ratio<ratio_value_type>& a, const ratio<ratio_value_type>& b) -> ratio<ratio_value_type>;

   // Comparison
   template<std::integral ratio_value_type>
   [[nodiscard]] constexpr auto operator==(const ratio<ratio_value_type>& a, const ratio<ratio_value_type>& b) -> bool;

   // CDAT
   ratio()->ratio<int>;
   template<std::intmax_t num, std::intmax_t denom>
   ratio(const std::ratio<num, denom>)->ratio<std::intmax_t>;
}


template<std::integral ratio_value_type, std::integral other_type>
constexpr auto rat::operator*(const ratio<ratio_value_type>& ratio, const other_type other) -> other_type
{
   if ((other * ratio.num()) % ratio.denom() != 0)
   {
      // TODO
      // std::terminate();
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
   return a.get_ratio<double>() == b.get_ratio<double>(); // TODO: this is awful
}
