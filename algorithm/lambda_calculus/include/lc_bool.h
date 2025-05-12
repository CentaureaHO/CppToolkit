#ifndef __LAMBDA_CALCULUS_LC_BOOL_H__
#define __LAMBDA_CALCULUS_LC_BOOL_H__

#include <functional>

namespace Cele::LC
{
    using Func = std::function<void*(void*)>;
    using Bool = std::function<Func(void*)>;

    inline const Bool TRUE  = [](void* a) { return [a](void* b) -> void* { return a; }; };
    inline const Bool FALSE = [](void* a) { return [a](void* b) -> void* { return b; }; };

    inline constexpr auto IDENTITY = [](auto a) { return a; };
    inline constexpr auto IF       = IDENTITY;

    inline const auto IF_THEN_ELSE = [](const Bool& condition) {
        return [condition](const std::function<Bool()>& then_thunk) {
            return [condition, &then_thunk](const std::function<Bool()>& else_thunk) -> Bool {
                void* selected_thunk_ptr = condition((void*)&then_thunk)((void*)&else_thunk);
                return (*static_cast<const std::function<Bool()>*>(selected_thunk_ptr))();
            };
        };
    };

    inline constexpr auto AND = [](const Bool& a) {
        return [a](const Bool& b) -> Bool {
            return [a, b](void* x) -> Func {
                auto b_x     = b(x);
                auto false_x = FALSE(x);
                return [a, b_x, false_x](void* y) -> void* { return a(b_x(y))(false_x(y)); };
            };
        };
    };

    inline constexpr auto OR = [](const Bool& a) {
        return [a](const Bool& b) -> Bool {
            return [a, b](void* x) -> Func {
                auto true_x = TRUE(x);
                auto b_x    = b(x);
                return [a, true_x, b_x](void* y) -> void* { return a(true_x(y))(b_x(y)); };
            };
        };
    };

    inline constexpr auto NOT = [](const Bool& a) -> Bool {
        return [a](void* x) -> Func { return [a, x](void* y) -> void* { return a(y)(x); }; };
    };

    inline constexpr auto XOR = [](const Bool& a) {
        return [a](const Bool& b) -> Bool {
            return [a, b](void* x) -> Func {
                auto not_b   = NOT(b);
                auto not_b_x = not_b(x);
                auto b_x     = b(x);
                return [a, not_b_x, b_x](void* y) -> void* { return a(not_b_x(y))(b_x(y)); };
            };
        };
    };

    inline constexpr auto XNOR = [](const Bool& a) {
        return [a](const Bool& b) -> Bool {
            auto xor_ab = XOR(a)(b);
            return NOT(xor_ab);
        };
    };

    inline bool as_bool(const Bool& b)
    {
        void* one  = reinterpret_cast<void*>(1);
        void* zero = reinterpret_cast<void*>(0);
        return b(one)(zero) != zero;
    }
}  // namespace Cele::LC

#endif  // __LAMBDA_CALCULUS_LC_BOOL_H__
