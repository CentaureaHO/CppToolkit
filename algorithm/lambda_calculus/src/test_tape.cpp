#include "lc_tape.h"
#include "common.h"
#include "lc_natural.h"
#include "lc_bool.h"
#include <vector>

std::vector<uint32_t> as_vector_uint_tape(const Cele::LC::List<Cele::LC::Natural>& lc_list)
{
    std::vector<uint32_t>             vec;
    Cele::LC::List<Cele::LC::Natural> current = lc_list;
    while (!Cele::LC::as_bool(Cele::LC::IS_NIL<Cele::LC::Natural>(current)))
    {
        vec.push_back(Cele::LC::as_uint(Cele::LC::HEAD<Cele::LC::Natural>(current)));
        current = Cele::LC::TAIL<Cele::LC::Natural>(current);
    }
    return vec;
}

struct TapeState
{
    std::vector<uint32_t> left;
    uint32_t              current;
    std::vector<uint32_t> right;

    bool operator==(const TapeState& other) const
    {
        return left == other.left && current == other.current && right == other.right;
    }
};

TapeState get_tape_state(const Cele::LC::Tape& tape)
{
    auto tmp = as_vector_uint_tape(Cele::LC::FIRST(tape));
    std::reverse(tmp.begin(), tmp.end());
    return {tmp,
        Cele::LC::as_uint(Cele::LC::FIRST(Cele::LC::SECOND(tape))),
        as_vector_uint_tape(Cele::LC::SECOND(Cele::LC::SECOND(tape)))};
}

void test_tape_basic();
void test_tape_movement();
void test_tape_operations();

using namespace Cele::LC;

template <int N>
struct tmp
{
    static auto apply(const auto& tape)
    {
        std::cout << "Current middle value: " << as_uint(CURRENT_VAL(tape)) << "\n";
        return tmp<N - 1>::apply(INC_TAPE(tape));
    }
};

template <>
struct tmp<0>
{
    static auto apply(const auto& tape)
    {
        return tape;
    }
};

void test_tape()
{
    std::cout << "\n[Lambda Calculus] Testing Tape type.\n";

    test_tape_basic();
    test_tape_movement();
    test_tape_operations();

    std::cout << "[Lambda Calculus] Tape type tests passed.\n";

    auto tape100 = tmp<100>::apply(TAPE_NEW());
}

void test_tape_basic()
{
    auto tape0 = TAPE_NEW();
    Assert(get_tape_state(tape0) == TapeState{{}, 0, {}}, "TAPE_NEW initial state");
    Assert(as_uint(CURRENT_VAL(tape0)) == 0, "CURRENT_VAL of new tape");

    auto tape1 = WRITE_TAPE(tape0)(FIVE);
    Assert(get_tape_state(tape1) == TapeState{{}, 5, {}}, "WRITE_TAPE to new tape");
    Assert(as_uint(CURRENT_VAL(tape1)) == 5, "CURRENT_VAL after WRITE_TAPE");

    Assert(get_tape_state(tape0) == TapeState{{}, 0, {}}, "TAPE_NEW original state unchanged");
}

void test_tape_movement()
{
    auto tape_v0 = TAPE_NEW();  // ([], 0, [])

    auto tape_v1 = RIGHT(tape_v0);
    Assert(get_tape_state(tape_v1) == TapeState{{0}, 0, {}}, "RIGHT from initial");

    auto tape_v2 = WRITE_TAPE(tape_v1)(ONE);
    Assert(get_tape_state(tape_v2) == TapeState{{0}, 1, {}}, "WRITE after RIGHT");

    auto tape_v3 = RIGHT(tape_v2);
    Assert(get_tape_state(tape_v3) == TapeState{{0, 1}, 0, {}}, "RIGHT again");
    auto tape_v4 = WRITE_TAPE(tape_v3)(TWO);
    Assert(get_tape_state(tape_v4) == TapeState{{0, 1}, 2, {}}, "WRITE after second RIGHT");

    auto tape_v5 = LEFT(tape_v4);
    Assert(get_tape_state(tape_v5) == TapeState{{0}, 1, {2}}, "LEFT");
    Assert(as_uint(CURRENT_VAL(tape_v5)) == 1, "CURRENT_VAL after LEFT");

    auto tape_v6 = LEFT(tape_v5);
    Assert(get_tape_state(tape_v6) == TapeState{{}, 0, {1, 2}}, "LEFT again");
    Assert(as_uint(CURRENT_VAL(tape_v6)) == 0, "CURRENT_VAL after second LEFT");

    auto tape_v7 = LEFT(tape_v6);
    Assert(get_tape_state(tape_v7) == TapeState{{}, 0, {0, 1, 2}}, "LEFT from edge");
    Assert(as_uint(CURRENT_VAL(tape_v7)) == 0, "CURRENT_VAL after LEFT from edge");

    auto tape_setup_v0  = TAPE_NEW();                        // ([], 0, [])
    auto tape_setup_v1  = WRITE_TAPE(tape_setup_v0)(THREE);  // ([], 3, [])
    auto tape_setup_v2  = LEFT(tape_setup_v1);               // ([], 0, [3])
    auto tape_setup_v3  = WRITE_TAPE(tape_setup_v2)(TWO);    // ([], 2, [3])
    auto tape_setup_v4  = LEFT(tape_setup_v3);               // ([], 0, [2, 3])
    auto tape_setup_v5  = WRITE_TAPE(tape_setup_v4)(ONE);    // ([], 1, [2, 3])
    auto tape_setup_v6  = RIGHT(tape_setup_v5);              // ([1], 2, [3])
    auto tape_setup_v7  = RIGHT(tape_setup_v6);              // ([1, 2], 3, [])
    auto tape_setup_v8  = RIGHT(tape_setup_v7);              // ([1, 2, 3], 0, [])
    auto tape_setup_v9  = WRITE_TAPE(tape_setup_v8)(FOUR);   // ([1, 2, 3], 4, [])
    auto tape_setup_v10 = RIGHT(tape_setup_v9);              // ([1, 2, 3, 4], 0, [])
    auto tape_setup_v11 = WRITE_TAPE(tape_setup_v10)(FIVE);  // ([1, 2, 3, 4], 5, [])
    auto tape_setup_v12 = LEFT(tape_setup_v11);              // ([1, 2, 3], 4, [5])
    auto tape_setup_v13 = LEFT(tape_setup_v12);              // ([1, 2], 3, [4, 5])

    Assert(get_tape_state(tape_setup_v13) == TapeState{{1, 2}, 3, {4, 5}}, "Complex setup verification");
}

void test_tape_operations()
{
    auto tape_ops_v0 = TAPE_NEW();  // ([], 0, [])
    Assert(as_bool(IS_ZERO_TAPE(tape_ops_v0)), "IS_ZERO_TAPE on new tape");

    auto tape_ops_v1 = INC_TAPE(tape_ops_v0);  // ([], 1, [])
    Assert(get_tape_state(tape_ops_v1) == TapeState{{}, 1, {}}, "INC_TAPE on 0");
    Assert(as_uint(CURRENT_VAL(tape_ops_v1)) == 1, "CURRENT_VAL after INC_TAPE");
    Assert(!as_bool(IS_ZERO_TAPE(tape_ops_v1)), "IS_ZERO_TAPE after INC_TAPE");

    auto tape_ops_v2 = INC_TAPE(tape_ops_v1);  // ([], 2, [])
    Assert(get_tape_state(tape_ops_v2) == TapeState{{}, 2, {}}, "INC_TAPE on 1");
    Assert(as_uint(CURRENT_VAL(tape_ops_v2)) == 2, "CURRENT_VAL after second INC_TAPE");

    auto tape_ops_v3 = DEC_TAPE(tape_ops_v2);  // ([], 1, [])
    Assert(get_tape_state(tape_ops_v3) == TapeState{{}, 1, {}}, "DEC_TAPE on 2");
    Assert(as_uint(CURRENT_VAL(tape_ops_v3)) == 1, "CURRENT_VAL after DEC_TAPE");

    auto tape_ops_v4 = DEC_TAPE(tape_ops_v3);  // ([], 0, [])
    Assert(get_tape_state(tape_ops_v4) == TapeState{{}, 0, {}}, "DEC_TAPE on 1");
    Assert(as_uint(CURRENT_VAL(tape_ops_v4)) == 0, "CURRENT_VAL after second DEC_TAPE");
    Assert(as_bool(IS_ZERO_TAPE(tape_ops_v4)), "IS_ZERO_TAPE after DEC_TAPE to 0");

    auto tape_ops_v5 = DEC_TAPE(tape_ops_v4);  // ([], 0, []) // DEC(ZERO) is ZERO for Natural
    Assert(get_tape_state(tape_ops_v5) == TapeState{{}, 0, {}}, "DEC_TAPE on 0 (Natural behavior)");
    Assert(as_uint(CURRENT_VAL(tape_ops_v5)) == 0, "CURRENT_VAL after DEC_TAPE on 0");
    Assert(as_bool(IS_ZERO_TAPE(tape_ops_v5)), "IS_ZERO_TAPE after DEC_TAPE on 0");
}
