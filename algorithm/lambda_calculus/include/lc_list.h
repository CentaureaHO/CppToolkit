#ifndef __LAMBDA_CALCULUS_LC_LIST_H__
#define __LAMBDA_CALCULUS_LC_LIST_H__

#include "lc_bool.h"
#include "lc_combinators.h"
#include "lc_natural.h"
#include <functional>
#include <stdexcept>

namespace Cele::LC
{
    template <typename T>
    struct List_t;

    using NilBranch = std::function<void*(void*)>;
    template <typename T>
    using ConsBranch = std::function<void*(const T& /*head*/, const List_t<T>& /*tail*/)>;
    template <typename T>
    using ListVal = std::function<std::function<void*(const ConsBranch<T>&)>(const NilBranch&)>;

    template <typename T>
    struct List_t
    {
        ListVal<T> value;

        List_t() = default;
        List_t(ListVal<T> v) : value(std::move(v)) {}

        auto operator()(const NilBranch& on_nil) const
        {
            if (!value)
            {
                throw std::bad_function_call();
            }
            return value(on_nil);
        }
        auto operator()(const NilBranch& on_nil)
        {
            if (!value)
            {
                throw std::bad_function_call();
            }
            return value(on_nil);
        }
    };

    template <typename T>
    using List = List_t<T>;

    template <typename T>
    inline constexpr auto NIL = []() -> List<T> {
        return List<T>([](const NilBranch& on_nil) {
            return [on_nil](const ConsBranch<T>& /*on_cons*/) -> void* { return on_nil(nullptr); };
        });
    };

    template <typename T>
    inline constexpr auto CONS = [](const T& h_val) {
        return [h_val](const List<T>& t_val) -> List<T> {
            return List<T>([h_val, t_val](const NilBranch& /*on_nil*/) {
                return [h_val, t_val](const ConsBranch<T>& on_cons) -> void* {
                    return on_cons(h_val, t_val);
                };
            });
        };
    };

    template <typename T>
    inline constexpr auto IS_NIL = [](const List<T>& lst) -> Bool {
        return [lst](void* t_param) {
            return [lst, t_param](void* f_param) -> void* {
                auto on_nil = [t_param](void* /*dummy*/) -> void* { return t_param; };
                auto on_cons = [f_param](const T& /*h*/, const List_t<T>& /*t*/) -> void* {
                    return f_param;
                };
                return lst(on_nil)(on_cons);
            };
        };
    };

    template <typename T>
    inline constexpr auto HEAD = [](const List<T>& lst) -> T {
        using HeadThunk = std::function<T()>;

        auto on_nil_branch = [](void* /*dummy*/) -> void* {
            HeadThunk thunk = []() -> T { throw std::runtime_error("HEAD called on an empty list (NIL)"); };
            return new HeadThunk(thunk);
        };

        auto on_cons_branch = [](const T& h, const List_t<T>& /*t*/) -> void* {
            HeadThunk thunk = [h_val = h]() -> T { return h_val; };
            return new HeadThunk(thunk);
        };

        void*      v_thunk_ptr = lst(on_nil_branch)(on_cons_branch);
        HeadThunk* thk_ptr     = static_cast<HeadThunk*>(v_thunk_ptr);
        T          result      = (*thk_ptr)();
        delete thk_ptr;
        return result;
    };

    template <typename T>
    inline constexpr auto TAIL = [](const List<T>& lst) -> List<T> {
        using TailThunk = std::function<List<T>()>;

        auto on_nil_branch = [](void* /*dummy*/) -> void* {
            TailThunk thunk = []() -> List<T> { throw std::runtime_error("TAIL called on an empty list (NIL)"); };
            return new TailThunk(thunk);
        };

        auto on_cons_branch = [](const T& /*h*/, const List_t<T>& t) -> void* {
            TailThunk thunk = [t_val = t]() -> List<T> { return t_val; };
            return new TailThunk(thunk);
        };

        void*      v_thunk_ptr = lst(on_nil_branch)(on_cons_branch);
        TailThunk* thk_ptr     = static_cast<TailThunk*>(v_thunk_ptr);
        List<T>    result      = (*thk_ptr)();
        delete thk_ptr;
        return result;
    };

    template <typename T>
    inline constexpr auto FILTER = [](const std::function<Bool(const T&)>& pred) {
        auto gen = [pred_cap = pred](auto recur, const List<T>& curr_list) -> List<T> {
            return List<T>([recur, pred_cap, list_copy = curr_list](const NilBranch& nil_outer) {
                return [recur, pred_cap, list_copy, nil_outer](const ConsBranch<T>& cons_outer) -> void* {
                    auto orig_nil = [nil_outer](void* /*dummy*/) -> void* {
                        return nil_outer(nullptr);
                    };

                    auto orig_cons = [recur, pred_cap, nil_outer, cons_outer](
                                         const T& h, const List<T>& t) -> void* {
                        Bool cond_met = pred_cap(h);

                        std::function<void*()> thk_true = [h_val = h, t_val = t, recur, cons_outer]() -> void* {
                            List<T> filt_rec_tail = recur(t_val);
                            return cons_outer(h_val, filt_rec_tail);
                        };

                        std::function<void*()> thk_false =
                            [t_val = t, recur, nil_outer, cons_outer]() -> void* {
                            List<T> filt_rec_tail = recur(t_val);
                            return filt_rec_tail(nil_outer)(cons_outer);
                        };

                        void* sel_thunk_ptr = cond_met((void*)&thk_true)((void*)&thk_false);
                        return (*static_cast<std::function<void*()>*>(sel_thunk_ptr))();
                    };
                    return list_copy(orig_nil)(orig_cons);
                };
            });
        };

        FixPoint<decltype(gen)> fixed_func(std::move(gen));

        return [fixed_func_copy = fixed_func](const List<T>& in_list) -> List<T> {
            return fixed_func_copy(in_list);
        };
    };

    inline constexpr auto DROP = [](const Natural& n_drop) {
        return [n_drop_c = n_drop]<typename T>(const List<T>& in_list) -> List<T> {
            auto gen = [](auto recur, const Natural& curr_n, const List<T>& curr_list) -> List<T> {
                return List<T>([recur, n_copy = curr_n, list_copy = curr_list](const NilBranch& nil_outer) {
                    return [recur, n_copy, list_copy, nil_outer](
                               const ConsBranch<T>& cons_outer) -> void* {
                        Bool is_n_zero = IS_ZERO(n_copy);

                        std::function<void*()> thk_n_zero =
                            [&list_copy, &nil_outer, &cons_outer]() -> void* {
                            return list_copy(nil_outer)(cons_outer);
                        };

                        std::function<void*()> thk_n_not_zero =
                            [&recur, &n_copy, &list_copy, &nil_outer, &cons_outer]() -> void* {
                            auto orig_nil =
                                [&nil_outer](void* /*dummy*/) -> void* { return nil_outer(nullptr); };

                            auto orig_cons =
                                [&recur, &n_copy, &nil_outer, &cons_outer](
                                    const T& /*h*/, const List<T>& t) -> void* {
                                List<T> rec_res = recur(PRED(n_copy), t);
                                return rec_res(nil_outer)(cons_outer);
                            };
                            return list_copy(orig_nil)(orig_cons);
                        };

                        void* sel_thunk_ptr = is_n_zero((void*)&thk_n_zero)((void*)&thk_n_not_zero);
                        return (*static_cast<std::function<void*()>*>(sel_thunk_ptr))();
                    };
                });
            };

            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(n_drop_c, in_list);
        };
    };

    inline constexpr auto TAKE = [](const Natural& n_take) {
        return [n_take_c = n_take]<typename T>(const List<T>& in_list) -> List<T> {
            auto gen = [](auto recur, const Natural& curr_n, const List<T>& curr_list) -> List<T> {
                return List<T>([recur, n_copy = curr_n, list_copy = curr_list](const NilBranch& nil_outer) {
                    return [recur, n_copy, list_copy, nil_outer](
                               const ConsBranch<T>& cons_outer) -> void* {
                        Bool is_n_zero = IS_ZERO(n_copy);

                        std::function<void*()> thk_n_zero = [&nil_outer]() -> void* {
                            return NIL<T>()(nil_outer)(nullptr);
                        };

                        std::function<void*()> thk_n_not_zero =
                            [&recur, &n_copy, &list_copy, &nil_outer, &cons_outer]() -> void* {
                            auto orig_nil = [&nil_outer](void* /*dummy*/) -> void* {
                                return NIL<T>()(nil_outer)(nullptr);
                            };

                            auto orig_cons =
                                [&recur, &n_copy, &nil_outer, &cons_outer](
                                    const T& h, const List<T>& t) -> void* {
                                List<T> rec_res = recur(PRED(n_copy), t);
                                return CONS<T>(h)(rec_res)(nil_outer)(cons_outer);
                            };
                            return list_copy(orig_nil)(orig_cons);
                        };

                        void* sel_thunk_ptr = is_n_zero((void*)&thk_n_zero)((void*)&thk_n_not_zero);
                        return (*static_cast<std::function<void*()>*>(sel_thunk_ptr))();
                    };
                });
            };

            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(n_take_c, in_list);
        };
    };

    inline constexpr auto RANGE = [](const Natural& start) {
        return [start_c = start](const Natural& end_val) -> List<Natural> {
            auto gen = [](auto recur, const Natural& curr_val, const Natural& lim_val) -> List<Natural> {
                return List<Natural>([recur, val_copy = curr_val, lim_copy = lim_val](
                                         const NilBranch& nil_outer) {
                    return [recur, val_copy, lim_copy, nil_outer](
                               const ConsBranch<Natural>& cons_outer) -> void* {
                        Bool curr_lt_lim = IS_UL(val_copy)(lim_copy);

                        std::function<void*()> thk_less =
                            [&recur, &val_copy, &lim_copy, &nil_outer, &cons_outer]() -> void* {
                            List<Natural> rec_res = recur(SUCC(val_copy), lim_copy);
                            return CONS<Natural>(val_copy)(rec_res)(nil_outer)(cons_outer);
                        };

                        std::function<void*()> thk_not_less = [&nil_outer]() -> void* {
                            return NIL<Natural>()(nil_outer)(nullptr);
                        };

                        void* sel_thunk_ptr =
                            curr_lt_lim((void*)&thk_less)((void*)&thk_not_less);
                        return (*static_cast<std::function<void*()>*>(sel_thunk_ptr))();
                    };
                });
            };

            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(start_c, end_val);
        };
    };

    template <typename T>
    inline constexpr auto LENGTH = [](const List<T>& lst) -> Natural {
        auto gen = [](auto recur, const List<T>& curr_list) -> Natural {
            using LengthThunk = std::function<Natural()>;

            auto on_nil_lst = [](void* /*dummy*/) -> void* {
                LengthThunk thunk = []() -> Natural { return ZERO; };
                return new LengthThunk(thunk);
            };

            auto on_cons_lst = [recur_cap = recur](const T& /*h*/, const List<T>& t) -> void* {
                List<T>     t_val = t;
                LengthThunk thunk = [recur_cap, t_val]() -> Natural {
                    Natural tail_len = recur_cap(t_val);
                    return SUCC(tail_len);
                };
                return new LengthThunk(thunk);
            };

            void*        v_thunk_ptr = curr_list(on_nil_lst)(on_cons_lst);
            LengthThunk* thk_ptr     = static_cast<LengthThunk*>(v_thunk_ptr);
            Natural      result      = (*thk_ptr)();
            delete thk_ptr;
            return result;
        };

        FixPoint<decltype(gen)> fixed_func(std::move(gen));
        return fixed_func(lst);
    };

    template <typename T>
    inline constexpr auto APPEND = [](const List<T>& l1) {
        return [l1_copy = l1](const List<T>& l2) -> List<T> {
            auto gen = [](auto recur, const List<T>& curr_l1, const List<T>& l2_app) -> List<T> {
                return List<T>([recur, l1_c = curr_l1, l2_c = l2_app](const NilBranch& nil_outer) {
                    return [recur, l1_c, l2_c, nil_outer](const ConsBranch<T>& cons_outer) -> void* {
                        auto on_l1_nil = [&l2_c, &nil_outer, &cons_outer](void* /*dummy*/) -> void* {
                            return l2_c(nil_outer)(cons_outer);
                        };

                        auto on_l1_cons =
                            [&recur, &l2_c, &nil_outer, &cons_outer](const T& h, const List<T>& t) -> void* {
                            List<T> app_tail = recur(t, l2_c);
                            return CONS<T>(h)(app_tail)(nil_outer)(cons_outer);
                        };
                        return l1_c(on_l1_nil)(on_l1_cons);
                    };
                });
            };
            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(l1_copy, l2);
        };
    };

    template <typename T>
    inline constexpr auto REVERSE = [](const List<T>& lst) -> List<T> {
        auto gen_acc =
            [](auto recur, const List<T>& curr_list, const List<T>& acc_list) -> List<T> {
            using ThunkRet = List<T>;
            using Thunk    = std::function<ThunkRet()>;

            auto nil_lambda = [acc_c = acc_list](void* /*dummy*/) -> void* {
                Thunk thunk = [acc_c]() { return acc_c; };
                return new Thunk(thunk);
            };
            NilBranch nil_branch(nil_lambda);

            auto cons_lambda =
                [recur_c = recur, acc_list_c = acc_list](const T& h, const List<T>& t) -> void* {
                T       h_val = h;
                List<T> t_val = t;

                Thunk thunk = [recur_c, h_val, t_val, acc_list_c]() {
                    List<T> new_acc = CONS<T>(h_val)(acc_list_c);
                    return recur_c(t_val, new_acc);
                };
                return new Thunk(thunk);
            };
            ConsBranch<T> cons_branch(cons_lambda);

            void*     v_thunk_ptr = curr_list(nil_branch)(cons_branch);
            Thunk*    thk_ptr     = static_cast<Thunk*>(v_thunk_ptr);
            ThunkRet result      = (*thk_ptr)();
            delete thk_ptr;
            return result;
        };

        FixPoint<decltype(gen_acc)> fixed_func(std::move(gen_acc));
        return fixed_func(lst, NIL<T>());
    };

    template <typename InputT, typename OutputU>
    inline constexpr auto MAP = [](const std::function<OutputU(const InputT&)>& fn) {
        return [fn_cap = fn](const List<InputT>& in_list) -> List<OutputU> {
            auto gen = [](auto                                recur,
                            const std::function<OutputU(const InputT&)>& f_c,
                            const List<InputT>&                          curr_list_in) -> List<OutputU> {
                return List<OutputU>([recur, f_cap = f_c, list_in_c = curr_list_in](
                                         const NilBranch& nil_outer) {
                    return [recur, f_cap, list_in_c, nil_outer](
                               const ConsBranch<OutputU>& cons_outer) -> void* {
                        auto orig_in_nil = [&nil_outer](void* /*dummy*/) -> void* {
                            return NIL<OutputU>()(nil_outer)(nullptr);
                        };

                        auto orig_in_cons =
                            [&recur, &f_cap, &nil_outer, &cons_outer](
                                const InputT& h_in, const List<InputT>& t_in) -> void* {
                            OutputU       map_h_out = f_cap(h_in);
                            List<OutputU> map_t_out = recur(f_cap, t_in);
                            return CONS<OutputU>(map_h_out)(map_t_out)(nil_outer)(cons_outer);
                        };
                        return list_in_c(orig_in_nil)(orig_in_cons);
                    };
                });
            };
            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(fn_cap, in_list);
        };
    };

    template <typename T>
    inline constexpr auto INDEX = [](const List<T>& lst) {
        return [lst_c = lst](const Natural& idx) -> T {
            auto gen = [](auto recur, const List<T>& curr_list, const Natural& curr_idx) -> T {
                using ElemThunk = std::function<T()>;

                NilBranch on_list_nil = [](void* /*dummy*/) -> void* {
                    ElemThunk thunk = []() -> T {
                        throw std::runtime_error("INDEX: Index out of bounds (list is NIL or index too large).");
                    };
                    return new ElemThunk(thunk);
                };

                ConsBranch<T> on_list_cons =
                    [recur_cap = recur, idx_cap = curr_idx](const T& h, const List<T>& t) -> void* {
                    Bool is_idx_zero = IS_ZERO(idx_cap);

                    std::function<void*()> thk_idx_zero = [h_val = h]() -> void* {
                        ElemThunk elem_thk_head = [h_val]() -> T { return h_val; };
                        return new ElemThunk(elem_thk_head);
                    };

                    std::function<void*()> thk_idx_not_zero =
                        [recur_cap, t_val = t, idx_cap]() -> void* {
                        ElemThunk rec_elem_thk = [recur_cap, t_val, idx_cap]() -> T {
                            return recur_cap(t_val, PRED(idx_cap));
                        };
                        return new ElemThunk(rec_elem_thk);
                    };

                    void* sel_outer_thk_ptr =
                        is_idx_zero((void*)&thk_idx_zero)((void*)&thk_idx_not_zero);
                    std::function<void*()>& sel_outer_thk_ref =
                        *static_cast<std::function<void*()>*>(sel_outer_thk_ptr);
                    void* elem_thk_ptr = sel_outer_thk_ref();
                    return elem_thk_ptr;
                };

                void*      v_thunk_ptr = curr_list(on_list_nil)(on_list_cons);
                ElemThunk* thk_ptr     = static_cast<ElemThunk*>(v_thunk_ptr);
                T          result      = (*thk_ptr)();
                delete thk_ptr;
                return result;
            };

            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(lst_c, idx);
        };
    };

    template <typename T>
    inline constexpr auto ANY = [](const std::function<Bool(const T&)>& pred) {
        return [pred_cap = pred](const List<T>& lst) -> Bool {
            auto gen = [](auto                                recur,
                            const std::function<Bool(const T&)>& p,
                            const List<T>&                       curr_list) -> Bool {
                using BoolResThunk = std::function<Bool()>;

                NilBranch on_list_nil = [](void* /*dummy*/) -> void* {
                    BoolResThunk thunk = []() -> Bool { return FALSE; };
                    return new BoolResThunk(thunk);
                };

                ConsBranch<T> on_list_cons =
                    [recur_cap = recur, p_cap = p](const T& h, const List<T>& t) -> void* {
                    Bool head_ok = p_cap(h);

                    std::function<void*()> thk_head_true = []() -> void* {
                        BoolResThunk inner_thunk = []() -> Bool { return TRUE; };
                        return new BoolResThunk(inner_thunk);
                    };

                    std::function<void*()> thk_head_false =
                        [recur_cap, p_cap, t_val = t]() -> void* {
                        BoolResThunk inner_thunk = [recur_cap, p_cap, t_val]() -> Bool {
                            return recur_cap(p_cap, t_val);
                        };
                        return new BoolResThunk(inner_thunk);
                    };

                    void* sel_outer_thk_ptr =
                        head_ok((void*)&thk_head_true)((void*)&thk_head_false);
                    std::function<void*()>& sel_outer_thk_ref =
                        *static_cast<std::function<void*()>*>(sel_outer_thk_ptr);
                    return sel_outer_thk_ref();
                };

                void*         v_thunk_ptr = curr_list(on_list_nil)(on_list_cons);
                BoolResThunk* thk_ptr     = static_cast<BoolResThunk*>(v_thunk_ptr);
                Bool          result      = (*thk_ptr)();
                delete thk_ptr;
                return result;
            };

            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(pred_cap, lst);
        };
    };

    template <typename T>
    inline constexpr auto ALL = [](const std::function<Bool(const T&)>& pred) {
        return [pred_cap = pred](const List<T>& lst) -> Bool {
            auto gen = [](auto                                recur,
                            const std::function<Bool(const T&)>& p,
                            const List<T>&                       curr_list) -> Bool {
                using BoolResThunk = std::function<Bool()>;

                NilBranch on_list_nil = [](void* /*dummy*/) -> void* {
                    BoolResThunk thunk = []() -> Bool { return TRUE; };
                    return new BoolResThunk(thunk);
                };

                ConsBranch<T> on_list_cons =
                    [recur_cap = recur, p_cap = p](const T& h, const List<T>& t) -> void* {
                    Bool head_ok = p_cap(h);

                    std::function<void*()> thk_head_true =
                        [recur_cap, p_cap, t_val = t]() -> void* {
                        BoolResThunk inner_thunk = [recur_cap, p_cap, t_val]() -> Bool {
                            return recur_cap(p_cap, t_val);
                        };
                        return new BoolResThunk(inner_thunk);
                    };

                    std::function<void*()> thk_head_false = []() -> void* {
                        BoolResThunk inner_thunk = []() -> Bool { return FALSE; };
                        return new BoolResThunk(inner_thunk);
                    };

                    void* sel_outer_thk_ptr =
                        head_ok((void*)&thk_head_true)((void*)&thk_head_false);
                    std::function<void*()>& sel_outer_thk_ref =
                        *static_cast<std::function<void*()>*>(sel_outer_thk_ptr);
                    return sel_outer_thk_ref();
                };

                void*         v_thunk_ptr = curr_list(on_list_nil)(on_list_cons);
                BoolResThunk* thk_ptr     = static_cast<BoolResThunk*>(v_thunk_ptr);
                Bool          result      = (*thk_ptr)();
                delete thk_ptr;
                return result;
            };

            FixPoint<decltype(gen)> fixed_func(std::move(gen));
            return fixed_func(pred_cap, lst);
        };
    };
}  // namespace Cele::LC

#endif  // __LAMBDA_CALCULUS_LC_LIST_H__
