#ifndef __LAMBDA_CALCULUS_LC_NATURAL_H__
#define __LAMBDA_CALCULUS_LC_NATURAL_H__

#include "lc_bool.h"
#include <functional>
#include <stdexcept>

namespace Cele::LC
{
    struct Natural_t;
    using ZeroBranch = std::function<void*(void*)>;
    using SuccBranch = std::function<void*(const Natural_t&)>;
    using NaturalVal = std::function<std::function<void*(const SuccBranch&)>(const ZeroBranch&)>;

    struct Natural_t
    {
        NaturalVal value;

        Natural_t() = default;
        Natural_t(NaturalVal v) : value(std::move(v)) {}

        auto operator()(const ZeroBranch& on_zero) const
        {
            if (!value)
            {
                throw std::bad_function_call();
            }
            return value(on_zero);
        }
        auto operator()(const ZeroBranch& on_zero)
        {
            if (!value)
            {
                throw std::bad_function_call();
            }
            return value(on_zero);
        }
    };

    using Natural       = Natural_t;
    using BinaryNatOp   = std::function<std::function<Natural(const Natural&)>(const Natural&)>;
    using BinaryNatPred = std::function<std::function<Bool(const Natural&)>(const Natural&)>;

    inline const Natural ZERO = Natural(
        [](const ZeroBranch& on_zero) { return [on_zero](const SuccBranch&) -> void* { return on_zero(nullptr); }; });

    inline const auto SUCC = [](const Natural& n_pred_arg) -> Natural {
        // Capture n_pred_arg by value to avoid dangling reference if n_pred_arg is temporary.
        return Natural([n_pred_captured_val = n_pred_arg](const ZeroBranch&) {
            // The inner lambda also captures n_pred_captured_val by value (copy).
            return [n_pred_captured_val](const SuccBranch& on_succ) -> void* {
                return on_succ(n_pred_captured_val);
            };
        });
    };

    inline const auto PRED = [](const Natural& n_orig) -> Natural {
        // Capture n_orig by value.
        return Natural([n_val_captured = n_orig](const ZeroBranch& z_outer) {
            return [n_val_captured, z_outer](const SuccBranch& s_outer) -> void* {
                auto n_on_zero = [z_outer, s_outer /*, &n_val_captured -- not needed here explicitly*/ ](void*) -> void* {
                    return ZERO(z_outer)(s_outer);
                };
                auto n_on_succ = [z_outer, s_outer /*, &n_val_captured -- not needed here explicitly*/ ](const Natural& pred_of_n_val_captured) -> void* {
                    return pred_of_n_val_captured(z_outer)(s_outer);
                };
                return n_val_captured(n_on_zero)(n_on_succ);
            };
        });
    };
    inline const auto INC = SUCC;
    inline const auto DEC = PRED;

    inline const Natural ONE = SUCC(ZERO);

    inline const auto IS_ZERO = [](const Natural& n) -> Bool {
        return [n](void* t_param) {
            return [n, t_param](void* f_param) -> void* {
                auto n_on_zero = [t_param](void*) -> void* { return t_param; };
                auto n_on_succ = [f_param](const Natural&) -> void* { return f_param; };
                return n(n_on_zero)(n_on_succ);
            };
        };
    };

    inline const BinaryNatPred IS_ULE = [](const Natural& m) -> std::function<Bool(const Natural&)> {
        return [m, &IS_ULE_rec = IS_ULE](const Natural& n) -> Bool {
            return [m, n, &IS_ULE_rec](void* t_param) {
                return [m, n, &IS_ULE_rec, t_param](void* f_param) -> void* {
                    auto m_on_zero = [&](void*) -> void* { return t_param; };
                    auto m_on_succ = [&](const Natural& pred_m) -> void* {
                        auto n_on_zero = [&](void*) -> void* { return f_param; };
                        auto n_on_succ = [&](const Natural& pred_n) -> void* {
                            return IS_ULE_rec(pred_m)(pred_n)(t_param)(f_param);
                        };
                        return n(n_on_zero)(n_on_succ);
                    };
                    return m(m_on_zero)(m_on_succ);
                };
            };
        };
    };

    inline const BinaryNatPred IS_UL = [](const Natural& m) -> std::function<Bool(const Natural&)> {
        return [m](const Natural& n) -> Bool { return IS_ULE(SUCC(m))(n); };
    };

    inline const BinaryNatPred IS_UEQ = [](const Natural& m) -> std::function<Bool(const Natural&)> {
        return [m](const Natural& n) -> Bool { return AND(IS_ULE(m)(n))(IS_ULE(n)(m)); };
    };

    inline const BinaryNatOp ADD = [](const Natural& m) -> std::function<Natural(const Natural&)> {
        return [m, &ADD_rec = ADD](const Natural& n) -> Natural {
            return Natural([m, n, &ADD_rec](const ZeroBranch& z_case) {
                return [m, n, &ADD_rec, z_case](const SuccBranch& s_case) -> void* {
                    auto m_on_zero = [n, z_case, s_case](void*) -> void* {
                        return n(z_case)(s_case);
                    };
                    auto m_on_succ = [n, z_case, s_case, &ADD_rec](const Natural& pred_m) -> void* {
                        Natural sum_pred_m_n = ADD_rec(pred_m)(n);
                        Natural succ_sum     = SUCC(sum_pred_m_n);
                        return succ_sum(z_case)(s_case);
                    };
                    return m(m_on_zero)(m_on_succ);
                };
            });
        };
    };

    inline const BinaryNatOp SUB = [](const Natural& m) -> std::function<Natural(const Natural&)> {
        return [m, &SUB_rec = SUB](const Natural& n) -> Natural {
            return Natural([m, n, &SUB_rec](const ZeroBranch& z_res) {
                return [m, n, &SUB_rec, z_res](const SuccBranch& s_res) -> void* {
                    auto m_on_zero = [&](void*) -> void* { return ZERO(z_res)(s_res); };
                    auto m_on_succ = [&](const Natural& pred_m) -> void* {
                        auto n_on_zero = [&](void*) -> void* { return m(z_res)(s_res); };
                        auto n_on_succ = [&](const Natural& pred_n) -> void* {
                            return SUB_rec(pred_m)(pred_n)(z_res)(s_res);
                        };
                        return n(n_on_zero)(n_on_succ);
                    };
                    return m(m_on_zero)(m_on_succ);
                };
            });
        };
    };

    inline const BinaryNatOp MUL = [](const Natural& m) -> std::function<Natural(const Natural&)> {
        return [m, &MUL_rec = MUL, &ADD_func = ADD](const Natural& n) -> Natural {
            return Natural([m, n, &MUL_rec, &ADD_func](const ZeroBranch& z_case) {
                return [m, n, &MUL_rec, &ADD_func, z_case](const SuccBranch& s_case) -> void* {
                    auto m_on_zero = [z_case, s_case](void*) -> void* { return ZERO(z_case)(s_case); };
                    auto m_on_succ =
                        [n, z_case, s_case, &MUL_rec, &ADD_func](const Natural& pred_m) -> void* {
                        Natural prod_pred_m_n = MUL_rec(pred_m)(n);
                        Natural sum_res       = ADD_func(n)(prod_pred_m_n);
                        return sum_res(z_case)(s_case);
                    };
                    return m(m_on_zero)(m_on_succ);
                };
            });
        };
    };

    inline const BinaryNatOp DIV = [](const Natural& m) -> std::function<Natural(const Natural&)> {
        return [m, &DIV_rec = DIV](const Natural& n) -> Natural {
            return Natural([m, n, &DIV_rec](const ZeroBranch& z_res) {
                return [m, n, &DIV_rec, z_res](const SuccBranch& s_res) -> void* {
                    using Thunk = std::function<void*()>;

                    Bool  n_is_zero      = IS_ZERO(n);
                    Thunk thunk_n_zero   = [&]() -> void* { return ZERO(z_res)(s_res); };
                    Thunk thunk_n_not_zero = [&]() -> void* {
                        Bool  m_lt_n       = IS_UL(m)(n);
                        Thunk thunk_m_lt_n = [&]() -> void* { return ZERO(z_res)(s_res); };
                        Thunk thunk_m_ge_n = [&]() -> void* {
                            Natural m_sub_n      = SUB(m)(n);
                            Natural rec_div_res  = DIV_rec(m_sub_n)(n);
                            Natural res_plus_one = SUCC(rec_div_res);
                            return res_plus_one(z_res)(s_res);
                        };

                        void* inner_thunk_ptr = m_lt_n((void*)&thunk_m_lt_n)((void*)&thunk_m_ge_n);
                        return (*static_cast<Thunk*>(inner_thunk_ptr))();
                    };

                    void* outer_thunk_ptr = n_is_zero((void*)&thunk_n_zero)((void*)&thunk_n_not_zero);
                    return (*static_cast<Thunk*>(outer_thunk_ptr))();
                };
            });
        };
    };

    inline const BinaryNatOp MOD = [](const Natural& m) -> std::function<Natural(const Natural&)> {
        return [m, &MOD_rec = MOD](const Natural& n) -> Natural {
            return Natural([m, n, &MOD_rec](const ZeroBranch& z_res) {
                return [m, n, &MOD_rec, z_res](const SuccBranch& s_res) -> void* {
                    using Thunk      = std::function<void*()>;
                    Bool n_is_zero = IS_ZERO(n);

                    Thunk thunk_n_zero = [&]() -> void* { return m(z_res)(s_res); };

                    Thunk thunk_n_not_zero = [&]() -> void* {
                        Bool  m_lt_n       = IS_UL(m)(n);
                        Thunk thunk_m_lt_n = [&]() -> void* { return m(z_res)(s_res); };
                        Thunk thunk_m_ge_n = [&]() -> void* {
                            Natural m_sub_n     = SUB(m)(n);
                            Natural rec_mod_res = MOD_rec(m_sub_n)(n);
                            return rec_mod_res(z_res)(s_res);
                        };

                        void* inner_thunk_ptr = m_lt_n((void*)&thunk_m_lt_n)((void*)&thunk_m_ge_n);
                        return (*static_cast<Thunk*>(inner_thunk_ptr))();
                    };

                    void* outer_thunk_ptr = n_is_zero((void*)&thunk_n_zero)((void*)&thunk_n_not_zero);
                    return (*static_cast<Thunk*>(outer_thunk_ptr))();
                };
            });
        };
    };

    inline const BinaryNatOp POW = [](const Natural& base) -> std::function<Natural(const Natural&)> {
        return [base, &POW_rec = POW, &MUL_func = MUL](const Natural& exp) -> Natural {
            return Natural([base, exp, &POW_rec, &MUL_func](const ZeroBranch& z_case) {
                return [base, exp, &POW_rec, &MUL_func, z_case](const SuccBranch& s_case) -> void* {
                    auto exp_on_zero = [z_case, s_case](void*) -> void* { return ONE(z_case)(s_case); };
                    auto exp_on_succ =
                        [base, z_case, s_case, &POW_rec, &MUL_func](const Natural& pred_exp) -> void* {
                        Natural pow_base_pred_exp = POW_rec(base)(pred_exp);
                        Natural res_prod          = MUL_func(base)(pow_base_pred_exp);
                        return res_prod(z_case)(s_case);
                    };
                    return exp(exp_on_zero)(exp_on_succ);
                };
            });
        };
    };

    inline const Natural TWO   = INC(ONE);
    inline const Natural THREE = ADD(ONE)(TWO);
    inline const Natural FOUR  = MUL(TWO)(TWO);
    inline const Natural FIVE  = ADD(TWO)(THREE);
    inline const Natural SIX   = MUL(TWO)(THREE);
    inline const Natural SEVEN = INC(SIX);
    inline const Natural EIGHT = POW(TWO)(THREE);
    inline const Natural NINE  = POW(THREE)(TWO);

    inline const auto IS_EVEN = [](const Natural& n) { return IS_ZERO(MOD(n)(TWO)); };

    inline const auto IS_ODD = [](const Natural& n) { return NOT(IS_EVEN(n)); };

    inline uint32_t as_uint(const Natural& n)
    {
        static std::function<uint32_t(const Natural&)> converter = nullptr;

        if (!converter)
        {
            converter = [](const Natural& num) -> uint32_t {
                auto on_zero = [](void* /*dummy*/) -> void* {
                    return reinterpret_cast<void*>(static_cast<uintptr_t>(0));
                };

                auto on_succ = [](const Natural& pred_val) -> void* {
                    uint32_t count = converter(pred_val);
                    return reinterpret_cast<void*>(static_cast<uintptr_t>(count + 1));
                };
                void* res_ptr = num(on_zero)(on_succ);
                return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(res_ptr));
            };
        }
        return converter(n);
    }

    inline Natural as_lcnat(uint32_t val)
    {
        if (val == 0)
        {
            return ZERO;
        }
        return SUCC(as_lcnat(val - 1));
    }
}  // namespace Cele::LC

#endif  // __LAMBDA_CALCULUS_LC_NATURAL_H__
