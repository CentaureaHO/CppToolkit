#include "lc_pair.h"
#include "lc_bool.h"
#include "lc_natural.h"
#include "common.h"

void test_pair_basic();
void test_pair_nested();
void test_pair_difftypes();
void test_pair_nested_difftypes();

void test_pair()
{
    std::cout << "\n[Lambda Calculus] Testing Pair types.\n";

    test_pair_basic();
    test_pair_nested();
    test_pair_difftypes();
    test_pair_nested_difftypes();

    std::cout << "[Lambda Calculus] Pair types tests passed.\n";
}

using namespace Cele::LC;

void test_pair_basic()
{
    auto p1 = PAIR(ONE)(TWO);
    Assert(as_uint(FIRST(p1)) == 1, "FIRST(PAIR(ONE)(TWO)) should be 1");
    Assert(as_uint(SECOND(p1)) == 2, "SECOND(PAIR(ONE)(TWO)) should be 2");

    auto p2 = PAIR(TRUE)(FALSE);
    Assert(as_bool(FIRST(p2)) == true, "FIRST(PAIR(TRUE)(FALSE)) should be TRUE");
    Assert(as_bool(SECOND(p2)) == false, "SECOND(PAIR(TRUE)(FALSE)) should be FALSE");

    auto p_int_str = PAIR(10)(std::string("hello"));
    Assert(FIRST(p_int_str) == 10, "FIRST(PAIR(10)(\"hello\")) should be 10");
    Assert(SECOND(p_int_str) == "hello", "SECOND(PAIR(10)(\"hello\")) should be \"hello\"");
}

void test_pair_nested()
{
    auto p_nested1 = PAIR(PAIR(ONE)(TWO))(THREE);
    Assert(as_uint(FIRST(FIRST(p_nested1))) == 1, "FIRST(FIRST(PAIR(PAIR(ONE)(TWO))(THREE))) should be 1");
    Assert(as_uint(SECOND(FIRST(p_nested1))) == 2, "SECOND(FIRST(PAIR(PAIR(ONE)(TWO))(THREE))) should be 2");
    Assert(as_uint(SECOND(p_nested1)) == 3, "SECOND(PAIR(PAIR(ONE)(TWO))(THREE)) should be 3");

    auto p_nested2 = PAIR(ONE)(PAIR(TWO)(THREE));
    Assert(as_uint(FIRST(p_nested2)) == 1, "FIRST(PAIR(ONE)(PAIR(TWO)(THREE))) should be 1");
    Assert(as_uint(FIRST(SECOND(p_nested2))) == 2, "FIRST(SECOND(PAIR(ONE)(PAIR(TWO)(THREE)))) should be 2");
    Assert(as_uint(SECOND(SECOND(p_nested2))) == 3, "SECOND(SECOND(PAIR(ONE)(PAIR(TWO)(THREE)))) should be 3");

    auto p_nested3 = PAIR(PAIR(ONE)(TWO))(PAIR(THREE)(FOUR));
    Assert(as_uint(FIRST(FIRST(p_nested3))) == 1, "FIRST(FIRST(PAIR(PAIR(ONE)(TWO))(PAIR(THREE)(FOUR)))) should be 1");
    Assert(
        as_uint(SECOND(FIRST(p_nested3))) == 2, "SECOND(FIRST(PAIR(PAIR(ONE)(TWO))(PAIR(THREE)(FOUR)))) should be 2");
    Assert(
        as_uint(FIRST(SECOND(p_nested3))) == 3, "FIRST(SECOND(PAIR(PAIR(ONE)(TWO))(PAIR(THREE)(FOUR)))) should be 3");
    Assert(
        as_uint(SECOND(SECOND(p_nested3))) == 4, "SECOND(SECOND(PAIR(PAIR(ONE)(TWO))(PAIR(THREE)(FOUR)))) should be 4");
}

void test_pair_difftypes()
{
    auto p_nat_bool = PAIR(ONE)(TRUE);
    Assert(as_uint(FIRST(p_nat_bool)) == 1, "FIRST(PAIR(ONE)(TRUE)) should be 1");
    Assert(as_bool(SECOND(p_nat_bool)) == true, "SECOND(PAIR(ONE)(TRUE)) should be TRUE");

    auto p_bool_nat = PAIR(FALSE)(THREE);
    Assert(as_bool(FIRST(p_bool_nat)) == false, "FIRST(PAIR(FALSE)(THREE)) should be FALSE");
    Assert(as_uint(SECOND(p_bool_nat)) == 3, "SECOND(PAIR(FALSE)(THREE)) should be 3");
}

void test_pair_nested_difftypes()
{
    auto p_nested_mixed1 = PAIR(PAIR(ONE)(TRUE))(std::string("world"));
    Assert(as_uint(FIRST(FIRST(p_nested_mixed1))) == 1, "FIRST(FIRST(p_nested_mixed1)) should be 1");
    Assert(as_bool(SECOND(FIRST(p_nested_mixed1))) == true, "SECOND(FIRST(p_nested_mixed1)) should be TRUE");
    Assert(SECOND(p_nested_mixed1) == "world", "SECOND(p_nested_mixed1) should be \"world\"");

    auto p_nested_mixed2 = PAIR(FALSE)(PAIR(std::string("lambda"))(FIVE));
    Assert(as_bool(FIRST(p_nested_mixed2)) == false, "FIRST(p_nested_mixed2) should be FALSE");
    Assert(FIRST(SECOND(p_nested_mixed2)) == "lambda", "FIRST(SECOND(p_nested_mixed2)) should be \"lambda\"");
    Assert(as_uint(SECOND(SECOND(p_nested_mixed2))) == 5, "SECOND(SECOND(p_nested_mixed2)) should be 5");

    auto p_nested_mixed3 = PAIR(PAIR(100)(std::string("test")))(PAIR(TRUE)(ZERO));
    Assert(FIRST(FIRST(p_nested_mixed3)) == 100, "FIRST(FIRST(p_nested_mixed3)) should be 100");
    Assert(SECOND(FIRST(p_nested_mixed3)) == "test", "SECOND(FIRST(p_nested_mixed3)) should be \"test\"");
    Assert(as_bool(FIRST(SECOND(p_nested_mixed3))) == true, "FIRST(SECOND(p_nested_mixed3)) should be TRUE");
    Assert(as_uint(SECOND(SECOND(p_nested_mixed3))) == 0, "SECOND(SECOND(p_nested_mixed3)) should be 0");
}
