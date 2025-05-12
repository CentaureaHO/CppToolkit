#ifndef __LAMBDA_CALCULUS_LC_SIGNED_H__
#define __LAMBDA_CALCULUS_LC_SIGNED_H__

#include "lc_bool.h"
#include "lc_pair.h"
#include "lc_natural.h"
#include <functional>
#include <stdint.h>

namespace Cele::LC
{
    using Sign = decltype(PAIR(std::declval<Bool>())(std::declval<Natural>()));

    inline const auto SIGN = [](const Natural& n) -> Sign { return PAIR(TRUE)(n); };

    inline const auto UNSIGN = [](const Sign& s) -> Natural { return SECOND(s); };

    inline const auto NEG = [](const Sign& s) -> Sign { return PAIR(NOT(FIRST(s)))(SECOND(s)); };

    inline const auto SADD = [](const Sign& s1) {
        return [s1](const Sign& s2) -> Sign {
            auto sign1 = FIRST(s1);
            auto mag1  = SECOND(s1);
            auto sign2 = FIRST(s2);
            auto mag2  = SECOND(s2);

            Bool                  signs_eq       = XNOR(sign1)(sign2);
            std::function<Sign()> thunk_signs_eq = [sign1, mag1, mag2]() {
                return PAIR(sign1)(ADD(mag1)(mag2));
            };

            std::function<Sign()> thunk_signs_diff = [sign1, sign2, mag1, mag2]() {
                Bool                  mag1_lt_mag2   = IS_UL(mag1)(mag2);
                std::function<Sign()> thunk_m1_lt_m2 = [sign2, mag1, mag2]() {
                    return PAIR(sign2)(SUB(mag2)(mag1));
                };

                std::function<Sign()> thunk_m1_ge_m2 = [sign1, sign2, mag1, mag2]() {
                    Bool                  mag2_lt_mag1   = IS_UL(mag2)(mag1);
                    std::function<Sign()> thunk_m1_gt_m2 = [sign1, mag1, mag2]() {
                        return PAIR(sign1)(SUB(mag1)(mag2));
                    };

                    std::function<Sign()> thunk_mags_eq = [sign1]() { return PAIR(sign1)(ZERO); };

                    void* inner_thunk_ptr = mag2_lt_mag1((void*)&thunk_m1_gt_m2)((void*)&thunk_mags_eq);
                    return (*static_cast<std::function<Sign()>*>(inner_thunk_ptr))();
                };

                void* mid_thunk_ptr = mag1_lt_mag2((void*)&thunk_m1_lt_m2)((void*)&thunk_m1_ge_m2);
                return (*static_cast<std::function<Sign()>*>(mid_thunk_ptr))();
            };

            void* outer_thunk_ptr = signs_eq((void*)&thunk_signs_eq)((void*)&thunk_signs_diff);
            return (*static_cast<std::function<Sign()>*>(outer_thunk_ptr))();
        };
    };

    inline const auto SMUL = [](const Sign& s1) {
        return [s1](const Sign& s2) -> Sign {
            auto sign1 = FIRST(s1);
            auto sign2 = FIRST(s2);
            auto mag1  = SECOND(s1);
            auto mag2  = SECOND(s2);

            Bool    res_sign = XNOR(sign1)(sign2);
            Natural res_mag  = MUL(mag1)(mag2);

            return PAIR(res_sign)(res_mag);
        };
    };

    inline const auto SSUB = [](const Sign& s1) { return [s1](const Sign& s2) -> Sign { return SADD(s1)(NEG(s2)); }; };

    inline const auto SDIV = [](const Sign& s1) {
        return [s1](const Sign& s2) -> Sign {
            auto sign1 = FIRST(s1);
            auto sign2 = FIRST(s2);
            auto mag1  = SECOND(s1);
            auto mag2  = SECOND(s2);

            Bool    res_sign = XNOR(sign1)(sign2);
            Natural res_mag  = DIV(mag1)(mag2);

            return PAIR(res_sign)(res_mag);
        };
    };

    inline const auto ISPOS = [](const Sign& s) -> Bool { return FIRST(s); };

    inline const auto ISNEG = [](const Sign& s) -> Bool { return NOT(FIRST(s)); };

    inline const auto IS_SZERO = [](const Sign& s) -> Bool { return IS_ZERO(SECOND(s)); };

    inline const auto SMOD = [](const Sign& s1) {
        return [s1](const Sign& s2) -> Sign {
            auto sign1 = FIRST(s1);
            auto sign2 = FIRST(s2);
            auto mag1  = SECOND(s1);
            auto mag2  = SECOND(s2);

            Bool    res_sign = sign1;
            Natural res_mag  = MOD(mag1)(mag2);

            return PAIR(res_sign)(res_mag);
        };
    };

    inline const auto IS_SLE = [](const Sign& s1) {
        return [s1](const Sign& s2) -> Bool {
            auto sign1 = FIRST(s1);
            auto sign2 = FIRST(s2);
            auto mag1  = SECOND(s1);
            auto mag2  = SECOND(s2);

            Bool signs_eq = XNOR(sign1)(sign2);

            std::function<Bool()> thunk_signs_eq = [sign1, mag1, mag2, sign2]() -> Bool {
                std::function<Bool()> both_pos_case = [mag1, mag2]() -> Bool { return IS_ULE(mag1)(mag2); };
                std::function<Bool()> both_neg_case = [mag1, mag2]() -> Bool { return IS_ULE(mag2)(mag1); };
                return IF_THEN_ELSE(sign1)(both_pos_case)(both_neg_case);
            };

            std::function<Bool()> thunk_signs_diff = [sign1]() -> Bool { return NOT(sign1); };

            return IF_THEN_ELSE(signs_eq)(thunk_signs_eq)(thunk_signs_diff);
        };
    };

    inline const auto IS_SL = [](const Sign& s1) {
        return [s1](const Sign& s2) -> Bool {
            auto sign1 = FIRST(s1);
            auto sign2 = FIRST(s2);
            auto mag1  = SECOND(s1);
            auto mag2  = SECOND(s2);

            Bool signs_eq = XNOR(sign1)(sign2);

            std::function<Bool()> thunk_signs_eq = [sign1, mag1, mag2, sign2]() -> Bool {
                std::function<Bool()> both_pos_case = [mag1, mag2]() -> Bool { return IS_UL(mag1)(mag2); };
                std::function<Bool()> both_neg_case = [mag1, mag2]() -> Bool { return IS_UL(mag2)(mag1); };
                return IF_THEN_ELSE(sign1)(both_pos_case)(both_neg_case);
            };

            std::function<Bool()> thunk_signs_diff = [sign1]() -> Bool { return NOT(sign1); };

            return IF_THEN_ELSE(signs_eq)(thunk_signs_eq)(thunk_signs_diff);
        };
    };

    inline const auto IS_SEQ = [](const Sign& s1) {
        return [s1](const Sign& s2) -> Bool {
            auto sign1 = FIRST(s1);
            auto sign2 = FIRST(s2);
            auto mag1  = SECOND(s1);
            auto mag2  = SECOND(s2);

            auto signs_eq = XNOR(sign1)(sign2);
            auto mags_eq  = IS_UEQ(mag1)(mag2);

            return AND(signs_eq)(mags_eq);
        };
    };

    inline int32_t as_int(const Sign& s)
    {
        bool     positive  = as_bool(FIRST(s));
        uint32_t magnitude = as_uint(SECOND(s));

        return positive ? magnitude : -static_cast<int32_t>(magnitude);
    }

    inline Sign as_sign(int32_t val)
    {
        bool     positive  = (val >= 0);
        uint32_t magnitude = positive ? val : -val;

        return PAIR(positive ? TRUE : FALSE)(as_lcnat(magnitude));
    }

}  // namespace Cele::LC

#endif  // __LAMBDA_CALCULUS_LC_SIGNED_H__
