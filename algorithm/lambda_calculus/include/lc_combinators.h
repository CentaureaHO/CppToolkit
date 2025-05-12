#ifndef __LAMBDA_CALCULUS_LC_COMBINATORS_H__
#define __LAMBDA_CALCULUS_LC_COMBINATORS_H__

#include <functional>

namespace Cele::LC
{
    template <typename T>
    inline constexpr auto I = [](T x) -> T { return x; };

    template <typename T, typename U>
    inline constexpr auto K = [](T x) { return [x](U y) -> T { return x; }; };

    template <typename X, typename Y, typename Z>
    inline constexpr auto S = [](auto x) {
        return [x](auto y) {
            return [x, y](Z z) {
                auto xz = x(z);
                auto yz = y(z);
                return xz(yz);
            };
        };
    };

    template <typename Func>
    struct FixPoint : Func
    {
        FixPoint(Func&& f) : Func(std::move(f)) {}

        template <typename... Args>
        decltype(auto) operator()(Args&&... args) const
        {
            return Func::operator()(*this, std::forward<Args>(args)...);
        }
    };
}  // namespace Cele::LC

#endif  // __LAMBDA_CALCULUS_LC_COMBINATORS_H__