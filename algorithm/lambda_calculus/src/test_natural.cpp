#include "lc_natural.h"
#include "common.h"
#include <cmath>

void test_natural_basic();
void test_natural_arithmetic();
void test_natural_comparison();

void test_natural()
{
    std::cout << "\n[Lambda Calculus] Testing Natural types.\n";

    test_natural_basic();
    test_natural_comparison();
    test_natural_arithmetic();

    std::cout << "[Lambda Calculus] Natural types tests passed.\n";
}

using namespace Cele::LC;

void test_natural_basic()
{
    Assert(as_uint(ZERO) == 0, "0 should be 0");
    Assert(as_uint(ONE) == 1, "1 should be 1");
    Assert(as_uint(TWO) == 2, "2 should be 2");
    Assert(as_uint(THREE) == 3, "3 should be 3");
    Assert(as_uint(FOUR) == 4, "4 should be 4");
    Assert(as_uint(FIVE) == 5, "5 should be 5");
    Assert(as_uint(SIX) == 6, "6 should be 6");
    Assert(as_uint(SEVEN) == 7, "7 should be 7");
    Assert(as_uint(EIGHT) == 8, "8 should be 8");
    Assert(as_uint(NINE) == 9, "9 should be 9");
}

void test_natural_comparison()
{
    auto IS_UG  = [&](const Natural& x) { return [&, x](const Natural& y) { return IS_UL(y)(x); }; };
    auto IS_UGE = [&](const Natural& x) { return [&, x](const Natural& y) { return IS_ULE(y)(x); }; };
    auto IS_UNE = [&](const Natural& x) { return [&, x](const Natural& y) { return NOT(IS_UEQ(x)(y)); }; };

    uint32_t val_two = 2, val_three = 3, val_five = 5, val_six = 6, val_seven = 7, val_eight = 8;

    Assert(as_bool(IS_UL(TWO)(THREE)) == (val_two < val_three),
        "IS_UL(2)(3) should be " + std::to_string(val_two < val_three));
    Assert(as_bool(IS_UL(THREE)(TWO)) == (val_three < val_two),
        "IS_UL(3)(2) should be " + std::to_string(val_three < val_two));

    Assert(as_bool(IS_UG(THREE)(TWO)) == (val_three > val_two),
        "IS_UG(3)(2) should be " + std::to_string(val_three > val_two));
    Assert(as_bool(IS_UG(TWO)(THREE)) == (val_two > val_three),
        "IS_UG(2)(3) should be " + std::to_string(val_two > val_three));

    Assert(as_bool(IS_ULE(TWO)(TWO)) == (val_two <= val_two),
        "IS_ULE(2)(2) should be " + std::to_string(val_two <= val_two));
    Assert(as_bool(IS_ULE(TWO)(THREE)) == (val_two <= val_three),
        "IS_ULE(2)(3) should be " + std::to_string(val_two <= val_three));
    Assert(as_bool(IS_ULE(THREE)(TWO)) == (val_three <= val_two),
        "IS_ULE(3)(2) should be " + std::to_string(val_three <= val_two));

    Assert(as_bool(IS_UGE(TWO)(TWO)) == (val_two >= val_two),
        "IS_UGE(2)(2) should be " + std::to_string(val_two >= val_two));
    Assert(as_bool(IS_UGE(THREE)(TWO)) == (val_three >= val_two),
        "IS_UGE(3)(2) should be " + std::to_string(val_three >= val_two));
    Assert(as_bool(IS_UGE(TWO)(THREE)) == (val_two >= val_three),
        "IS_UGE(2)(3) should be " + std::to_string(val_two >= val_three));

    Assert(as_bool(IS_UEQ(TWO)(TWO)) == (val_two == val_two),
        "IS_UEQ(2)(2) should be " + std::to_string(val_two == val_two));
    Assert(as_bool(IS_UEQ(TWO)(THREE)) == (val_two == val_three),
        "IS_UEQ(2)(3) should be " + std::to_string(val_two == val_three));

    Assert(as_bool(IS_UNE(TWO)(THREE)) == (val_two != val_three),
        "IS_UNE(2)(3) should be " + std::to_string(val_two != val_three));
    Assert(as_bool(IS_UNE(TWO)(TWO)) == (val_two != val_two),
        "IS_UNE(2)(2) should be " + std::to_string(val_two != val_two));

    Assert(
        as_bool(IS_EVEN(EIGHT)) == (val_eight % 2 == 0), "IS_EVEN(8) should be " + std::to_string(val_eight % 2 == 0));
    Assert(
        as_bool(IS_EVEN(SEVEN)) == (val_seven % 2 == 0), "IS_EVEN(7) should be " + std::to_string(val_seven % 2 == 0));
    Assert(as_bool(IS_ODD(FIVE)) == (val_five % 2 != 0), "IS_ODD(5) should be " + std::to_string(val_five % 2 != 0));
    Assert(as_bool(IS_ODD(SIX)) == (val_six % 2 != 0), "IS_ODD(6) should be " + std::to_string(val_six % 2 != 0));
}

void test_natural_arithmetic()
{
    uint32_t val_one = 1, val_two = 2, val_three = 3, val_five = 5, val_seven = 7, val_eight = 8, val_nine = 9;

    Assert(as_uint(ADD(ONE)(TWO)) == (val_one + val_two), "ADD(1)(2) should be " + std::to_string(val_one + val_two));
    Assert(as_uint(ADD(TWO)(THREE)) == (val_two + val_three),
        "ADD(2)(3) should be " + std::to_string(val_two + val_three));
    Assert(as_uint(ADD(SEVEN)(ADD(EIGHT)(NINE))) == (val_seven + (val_eight + val_nine)),
        "ADD(7)(ADD(8)(9)) should be " + std::to_string(val_seven + val_eight + val_nine));

    Assert(as_uint(SUB(FIVE)(THREE)) == (val_five - val_three),
        "SUB(5)(3) should be " + std::to_string(val_five - val_three));
    uint32_t sub_two_five_expected = (val_two >= val_five) ? (val_two - val_five) : 0;  // LC SUB floors at 0
    Assert(as_uint(SUB(TWO)(FIVE)) == sub_two_five_expected,
        "SUB(2)(5) should be " + std::to_string(sub_two_five_expected) + " (floor at 0)");

    Assert(as_uint(MUL(TWO)(THREE)) == (val_two * val_three),
        "MUL(2)(3) should be " + std::to_string(val_two * val_three));
    Assert(as_uint(MUL(NINE)(NINE)) == (val_nine * val_nine),
        "MUL(9)(9) should be " + std::to_string(val_nine * val_nine));

    Assert(as_uint(DIV(EIGHT)(TWO)) == (val_eight / val_two),
        "DIV(8)(2) should be " + std::to_string(val_eight / val_two));
    Assert(as_uint(DIV(THREE)(TWO)) == (val_three / val_two),
        "DIV(3)(2) should be " + std::to_string(val_three / val_two));

    Assert(as_uint(MOD(EIGHT)(THREE)) == (val_eight % val_three),
        "MOD(8)(3) should be " + std::to_string(val_eight % val_three));
    Assert(as_uint(MOD(THREE)(TWO)) == (val_three % val_two),
        "MOD(3)(2) should be " + std::to_string(val_three % val_two));

    std::cout << "Followed 7 ^ 3 might cause stack overflow on windows.\n";
    uint32_t pow_seven_three_expected = static_cast<uint32_t>(std::pow(val_seven, val_three));
    Assert(as_uint(POW(SEVEN)(THREE)) == pow_seven_three_expected,
        "POW(7)(3) should be " + std::to_string(pow_seven_three_expected));
    uint32_t pow_three_two_expected = static_cast<uint32_t>(std::pow(val_three, val_two));
    Assert(as_uint(POW(THREE)(TWO)) == pow_three_two_expected,
        "POW(3)(2) should be " + std::to_string(pow_three_two_expected));
    uint32_t pow_one_two_expected           = static_cast<uint32_t>(std::pow(val_one, val_two));
    uint32_t pow_three_pow_one_two_expected = static_cast<uint32_t>(std::pow(val_three, pow_one_two_expected));
    Assert(as_uint(POW(THREE)(POW(ONE)(TWO))) == pow_three_pow_one_two_expected,
        "POW(3)(POW(1)(2)) should be " + std::to_string(pow_three_pow_one_two_expected));
}
