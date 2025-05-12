#ifndef __LAMBDA_CALCULUS_LC_PAIR_H__
#define __LAMBDA_CALCULUS_LC_PAIR_H__

#include <functional>

namespace Cele::LC
{
    template <typename F, typename S>
    struct Pair_t
    {
        F first_val;
        S second_val;

        Pair_t(F f, S s) : first_val(std::move(f)), second_val(std::move(s)) {}

        template <typename Ret>
        Ret operator()(const std::function<Ret(const F&, const S&)>& selector) const
        {
            return selector(first_val, second_val);
        }
    };

    inline constexpr auto PAIR = [](auto first) {
        return [first = std::move(first)](auto second) {
            return [first = std::move(first), second = std::move(second)](
                       auto selector) { return selector(first, second); };
        };
    };

    inline constexpr auto FIRST = [](const auto& pair) {
        return pair([](const auto& first, const auto&) { return first; });
    };

    inline constexpr auto SECOND = [](const auto& pair) {
        return pair([](const auto&, const auto& second) { return second; });
    };
}  // namespace Cele::LC

#endif  // __LAMBDA_CALCULUS_LC_PAIR_H__
