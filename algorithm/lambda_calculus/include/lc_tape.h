#ifndef __LAMBDA_CALCULUS_LC_TAPE_H__
#define __LAMBDA_CALCULUS_LC_TAPE_H__

#include "lc_pair.h"
#include "lc_list.h"
#include "lc_natural.h"
#include "lc_bool.h"
#include <functional>

namespace Cele::LC
{
    using Tape =
        decltype(PAIR(std::declval<List<Natural>>())(PAIR(std::declval<Natural>())(std::declval<List<Natural>>())));

    inline const auto TAPE_NEW = []() -> Tape { return PAIR(NIL<Natural>())(PAIR(ZERO)(NIL<Natural>())); };

    inline const auto CURRENT_VAL = [](const Tape& tape) -> Natural { return FIRST(SECOND(tape)); };

    inline const auto WRITE_TAPE = [](const Tape& tape) {
        return [tape](const Natural& new_val) -> Tape {
            auto left_part  = FIRST(tape);
            auto right_part = SECOND(SECOND(tape));
            return PAIR(left_part)(PAIR(new_val)(right_part));
        };
    };

    inline const auto LEFT = [](const Tape& tape) -> Tape {
        List<Natural> left_list   = FIRST(tape);
        Natural       current_val = FIRST(SECOND(tape));
        List<Natural> right_list  = SECOND(SECOND(tape));

        using TapeThunk = std::function<Tape()>;

        auto on_nil_left = [&](void* /*dummy*/) -> void* {
            TapeThunk thunk = [current_val, right_list]() -> Tape {
                return PAIR(NIL<Natural>())(PAIR(ZERO)(CONS<Natural>(current_val)(right_list)));
            };
            return new TapeThunk(thunk);
        };

        auto on_cons_left = [&](const Natural& h_left, const List<Natural>& t_left) -> void* {
            TapeThunk thunk = [h_left, t_left, current_val, right_list]() -> Tape {
                return PAIR(t_left)(PAIR(h_left)(CONS<Natural>(current_val)(right_list)));
            };
            return new TapeThunk(thunk);
        };

        void*      v_thunk_ptr = left_list(on_nil_left)(on_cons_left);
        TapeThunk* thk_ptr     = static_cast<TapeThunk*>(v_thunk_ptr);
        Tape       result      = (*thk_ptr)();
        delete thk_ptr;
        return result;
    };

    inline const auto RIGHT = [](const Tape& tape) -> Tape {
        List<Natural> left_list   = FIRST(tape);
        Natural       current_val = FIRST(SECOND(tape));
        List<Natural> right_list  = SECOND(SECOND(tape));

        using TapeThunk = std::function<Tape()>;

        auto on_nil_right = [&](void* /*dummy*/) -> void* {
            TapeThunk thunk = [left_list, current_val]() -> Tape {
                return PAIR(CONS<Natural>(current_val)(left_list))(PAIR(ZERO)(NIL<Natural>()));
            };
            return new TapeThunk(thunk);
        };

        auto on_cons_right = [&](const Natural& h_right, const List<Natural>& t_right) -> void* {
            TapeThunk thunk = [left_list, current_val, h_right, t_right]() -> Tape {
                return PAIR(CONS<Natural>(current_val)(left_list))(PAIR(h_right)(t_right));
            };
            return new TapeThunk(thunk);
        };

        void*      v_thunk_ptr = right_list(on_nil_right)(on_cons_right);
        TapeThunk* thk_ptr     = static_cast<TapeThunk*>(v_thunk_ptr);
        Tape       result      = (*thk_ptr)();
        delete thk_ptr;
        return result;
    };

    inline const auto INC_TAPE = [](const Tape& tape) -> Tape {
        auto    left_part   = FIRST(tape);
        auto    current_val = FIRST(SECOND(tape));
        auto    right_part  = SECOND(SECOND(tape));
        Natural new_current_val;
        if (as_bool(IS_UEQ(current_val)(as_lcnat(255))))
        {
            new_current_val = ZERO;
        }
        else { new_current_val = SUCC(current_val); }
        return PAIR(left_part)(PAIR(new_current_val)(right_part));
    };

    inline const auto DEC_TAPE = [](const Tape& tape) -> Tape {
        auto    left_part   = FIRST(tape);
        auto    current_val = FIRST(SECOND(tape));
        auto    right_part  = SECOND(SECOND(tape));
        Natural new_current_val;
        if (as_bool(IS_ZERO(current_val)))
        {
            new_current_val = as_lcnat(255);
        }
        else { new_current_val = PRED(current_val); }
        return PAIR(left_part)(PAIR(new_current_val)(right_part));
    };

    inline const auto IS_ZERO_TAPE = [](const Tape& tape) -> Bool { return IS_ZERO(CURRENT_VAL(tape)); };

}  // namespace Cele::LC

#endif  // __LAMBDA_CALCULUS_LC_TAPE_H__
