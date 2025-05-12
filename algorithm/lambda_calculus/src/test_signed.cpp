#include "lc_signed.h"
#include "common.h"

void test_signed_basic();
void test_signed_arithmetic();
void test_signed_comparison();

void test_signed()
{
    std::cout << "\n[Lambda Calculus] Testing Signed types.\n";

    test_signed_basic();
    test_signed_comparison();
    test_signed_arithmetic();

    std::cout << "[Lambda Calculus] Signed types tests passed.\n";
}

using namespace Cele::LC;

void test_signed_basic()
{
    Sign pos_three = SIGN(THREE);
    Sign neg_three = NEG(pos_three);
    Sign pos_five  = SIGN(FIVE);
    Sign neg_five  = NEG(pos_five);
    Sign zero      = SIGN(ZERO);
    Sign neg_zero  = NEG(zero);

    Assert(as_int(pos_three) == 3, "+3 should be 3");
    Assert(as_int(neg_three) == -3, "-3 should be -3");
    Assert(as_int(zero) == 0, "+0 should be 0");
    Assert(as_int(neg_zero) == 0, "-0 should be 0");

    Assert(as_bool(ISPOS(pos_three)), "+3 should be positive");
    Assert(!as_bool(ISPOS(neg_three)), "-3 should not be positive");
    Assert(as_bool(ISNEG(neg_five)), "-5 should be negative");
    Assert(!as_bool(ISNEG(pos_five)), "+5 should not be negative");
    Assert(as_bool(IS_SZERO(zero)), "+0 should be zero");
    Assert(as_bool(IS_SZERO(neg_zero)), "-0 should be zero");
    Assert(!as_bool(IS_SZERO(pos_three)), "+3 should not be zero");
    Assert(!as_bool(IS_SZERO(neg_three)), "-3 should not be zero");

    Assert(as_int(NEG(pos_three)) == -3, "NEG(+3) should be -3");
    Assert(as_int(NEG(neg_three)) == 3, "NEG(-3) should be +3");
    Assert(as_int(NEG(zero)) == 0, "NEG(0) should be 0");
}

void test_signed_comparison()
{
    Sign pos_three_lc = SIGN(THREE);
    Sign neg_three_lc = NEG(pos_three_lc);
    Sign pos_five_lc  = SIGN(FIVE);
    Sign neg_five_lc  = NEG(pos_five_lc);
    Sign zero_lc      = SIGN(ZERO);

    int32_t pos_three = 3, neg_three = -3, pos_five = 5, neg_five = -5, zero_val = 0;

    auto IS_SG  = [](const Sign& x) { return [&, x](const Sign& y) { return IS_SL(y)(x); }; };
    auto IS_SGE = [](const Sign& x) { return [&, x](const Sign& y) { return IS_SLE(y)(x); }; };
    auto IS_SNE = [](const Sign& x) { return [&, x](const Sign& y) { return NOT(IS_SEQ(x)(y)); }; };

    Assert(as_bool(IS_SL(neg_five_lc)(neg_three_lc)) == (neg_five < neg_three),
        "IS_SL(-5)(-3) should be " + std::to_string(neg_five < neg_three));
    Assert(as_bool(IS_SL(neg_three_lc)(zero_lc)) == (neg_three < zero_val),
        "IS_SL(-3)(0) should be " + std::to_string(neg_three < zero_val));
    Assert(as_bool(IS_SL(zero_lc)(pos_three_lc)) == (zero_val < pos_three),
        "IS_SL(0)(3) should be " + std::to_string(zero_val < pos_three));
    Assert(as_bool(IS_SL(pos_three_lc)(pos_five_lc)) == (pos_three < pos_five),
        "IS_SL(3)(5) should be " + std::to_string(pos_three < pos_five));
    Assert(as_bool(IS_SL(pos_five_lc)(pos_three_lc)) == (pos_five < pos_three),
        "IS_SL(5)(3) should be " + std::to_string(pos_five < pos_three));
    Assert(as_bool(IS_SL(pos_three_lc)(zero_lc)) == (pos_three < zero_val),
        "IS_SL(3)(0) should be " + std::to_string(pos_three < zero_val));
    Assert(as_bool(IS_SL(zero_lc)(neg_three_lc)) == (zero_val < neg_three),
        "IS_SL(0)(-3) should be " + std::to_string(zero_val < neg_three));

    Assert(as_bool(IS_SLE(neg_five_lc)(neg_three_lc)) == (neg_five <= neg_three),
        "IS_SLE(-5)(-3) should be " + std::to_string(neg_five <= neg_three));
    Assert(as_bool(IS_SLE(neg_three_lc)(zero_lc)) == (neg_three <= zero_val),
        "IS_SLE(-3)(0) should be " + std::to_string(neg_three <= zero_val));
    Assert(as_bool(IS_SLE(zero_lc)(pos_three_lc)) == (zero_val <= pos_three),
        "IS_SLE(0)(3) should be " + std::to_string(zero_val <= pos_three));
    Assert(as_bool(IS_SLE(pos_three_lc)(pos_three_lc)) == (pos_three <= pos_three),
        "IS_SLE(3)(3) should be " + std::to_string(pos_three <= pos_three));
    Assert(as_bool(IS_SLE(neg_three_lc)(neg_three_lc)) == (neg_three <= neg_three),
        "IS_SLE(-3)(-3) should be " + std::to_string(neg_three <= neg_three));
    Assert(as_bool(IS_SLE(pos_five_lc)(pos_three_lc)) == (pos_five <= pos_three),
        "IS_SLE(5)(3) should be " + std::to_string(pos_five <= pos_three));

    Assert(as_bool(IS_SG(pos_five_lc)(pos_three_lc)) == (pos_five > pos_three),
        "IS_SG(5)(3) should be " + std::to_string(pos_five > pos_three));
    Assert(as_bool(IS_SG(pos_three_lc)(zero_lc)) == (pos_three > zero_val),
        "IS_SG(3)(0) should be " + std::to_string(pos_three > zero_val));
    Assert(as_bool(IS_SG(zero_lc)(neg_three_lc)) == (zero_val > neg_three),
        "IS_SG(0)(-3) should be " + std::to_string(zero_val > neg_three));
    Assert(as_bool(IS_SG(neg_three_lc)(neg_five_lc)) == (neg_three > neg_five),
        "IS_SG(-3)(-5) should be " + std::to_string(neg_three > neg_five));
    Assert(as_bool(IS_SG(pos_three_lc)(pos_five_lc)) == (pos_three > pos_five),
        "IS_SG(3)(5) should be " + std::to_string(pos_three > pos_five));

    Assert(as_bool(IS_SGE(pos_five_lc)(pos_three_lc)) == (pos_five >= pos_three),
        "IS_SGE(5)(3) should be " + std::to_string(pos_five >= pos_three));
    Assert(as_bool(IS_SGE(pos_three_lc)(pos_three_lc)) == (pos_three >= pos_three),
        "IS_SGE(3)(3) should be " + std::to_string(pos_three >= pos_three));
    Assert(as_bool(IS_SGE(neg_three_lc)(neg_three_lc)) == (neg_three >= neg_three),
        "IS_SGE(-3)(-3) should be " + std::to_string(neg_three >= neg_three));
    Assert(as_bool(IS_SGE(neg_five_lc)(neg_three_lc)) == (neg_five >= neg_three),
        "IS_SGE(-5)(-3) should be " + std::to_string(neg_five >= neg_three));

    Assert(as_bool(IS_SEQ(pos_three_lc)(pos_three_lc)) == (pos_three == pos_three),
        "IS_SEQ(3)(3) should be " + std::to_string(pos_three == pos_three));
    Assert(as_bool(IS_SEQ(neg_three_lc)(neg_three_lc)) == (neg_three == neg_three),
        "IS_SEQ(-3)(-3) should be " + std::to_string(neg_three == neg_three));
    Assert(as_bool(IS_SEQ(zero_lc)(zero_lc)) == (zero_val == zero_val),
        "IS_SEQ(0)(0) should be " + std::to_string(zero_val == zero_val));
    Assert(as_bool(IS_SEQ(pos_three_lc)(neg_three_lc)) == (pos_three == neg_three),
        "IS_SEQ(3)(-3) should be " + std::to_string(pos_three == neg_three));
    Assert(as_bool(IS_SEQ(pos_three_lc)(pos_five_lc)) == (pos_three == pos_five),
        "IS_SEQ(3)(5) should be " + std::to_string(pos_three == pos_five));

    Assert(as_bool(IS_SNE(pos_three_lc)(neg_three_lc)) == (pos_three != neg_three),
        "IS_SNE(3)(-3) should be " + std::to_string(pos_three != neg_three));
    Assert(as_bool(IS_SNE(pos_three_lc)(pos_five_lc)) == (pos_three != pos_five),
        "IS_SNE(3)(5) should be " + std::to_string(pos_three != pos_five));
    Assert(as_bool(IS_SNE(pos_three_lc)(pos_three_lc)) == (pos_three != pos_three),
        "IS_SNE(3)(3) should be " + std::to_string(pos_three != pos_three));
}

void test_signed_arithmetic()
{
    Sign pos_three_lc = SIGN(THREE);
    Sign neg_three_lc = NEG(pos_three_lc);
    Sign pos_five_lc  = SIGN(FIVE);
    Sign neg_five_lc  = NEG(pos_five_lc);
    Sign zero_lc      = SIGN(ZERO);
    Sign pos_two_lc   = SIGN(TWO);
    Sign neg_two_lc   = NEG(pos_two_lc);

    int32_t pos_three = 3, neg_three = -3, pos_five = 5, neg_five = -5, zero_val = 0, pos_two = 2, neg_two = -2;

    Assert(as_int(SADD(pos_three_lc)(pos_two_lc)) == (pos_three + pos_two),
        "SADD(3)(2) should be " + std::to_string(pos_three + pos_two));
    Assert(as_int(SADD(pos_three_lc)(neg_two_lc)) == (pos_three + neg_two),
        "SADD(3)(-2) should be " + std::to_string(pos_three + neg_two));
    Assert(as_int(SADD(neg_three_lc)(pos_five_lc)) == (neg_three + pos_five),
        "SADD(-3)(5) should be " + std::to_string(neg_three + pos_five));
    Assert(as_int(SADD(neg_three_lc)(neg_two_lc)) == (neg_three + neg_two),
        "SADD(-3)(-2) should be " + std::to_string(neg_three + neg_two));
    Assert(as_int(SADD(zero_lc)(pos_three_lc)) == (zero_val + pos_three),
        "SADD(0)(3) should be " + std::to_string(zero_val + pos_three));
    Assert(as_int(SADD(zero_lc)(neg_three_lc)) == (zero_val + neg_three),
        "SADD(0)(-3) should be " + std::to_string(zero_val + neg_three));
    Assert(as_int(SADD(pos_three_lc)(zero_lc)) == (pos_three + zero_val),
        "SADD(3)(0) should be " + std::to_string(pos_three + zero_val));

    Assert(as_int(SSUB(pos_five_lc)(pos_two_lc)) == (pos_five - pos_two),
        "SSUB(5)(2) should be " + std::to_string(pos_five - pos_two));
    Assert(as_int(SSUB(pos_three_lc)(pos_five_lc)) == (pos_three - pos_five),
        "SSUB(3)(5) should be " + std::to_string(pos_three - pos_five));
    Assert(as_int(SSUB(neg_three_lc)(neg_five_lc)) == (neg_three - neg_five),
        "SSUB(-3)(-5) should be " + std::to_string(neg_three - neg_five));
    Assert(as_int(SSUB(neg_three_lc)(pos_two_lc)) == (neg_three - pos_two),
        "SSUB(-3)(2) should be " + std::to_string(neg_three - pos_two));
    Assert(as_int(SSUB(zero_lc)(pos_three_lc)) == (zero_val - pos_three),
        "SSUB(0)(3) should be " + std::to_string(zero_val - pos_three));
    Assert(as_int(SSUB(zero_lc)(neg_three_lc)) == (zero_val - neg_three),
        "SSUB(0)(-3) should be " + std::to_string(zero_val - neg_three));

    Assert(as_int(SMUL(pos_three_lc)(pos_two_lc)) == (pos_three * pos_two),
        "SMUL(3)(2) should be " + std::to_string(pos_three * pos_two));
    Assert(as_int(SMUL(pos_three_lc)(neg_two_lc)) == (pos_three * neg_two),
        "SMUL(3)(-2) should be " + std::to_string(pos_three * neg_two));
    Assert(as_int(SMUL(neg_three_lc)(pos_two_lc)) == (neg_three * pos_two),
        "SMUL(-3)(2) should be " + std::to_string(neg_three * pos_two));
    Assert(as_int(SMUL(neg_three_lc)(neg_two_lc)) == (neg_three * neg_two),
        "SMUL(-3)(-2) should be " + std::to_string(neg_three * neg_two));
    Assert(as_int(SMUL(zero_lc)(pos_three_lc)) == (zero_val * pos_three),
        "SMUL(0)(3) should be " + std::to_string(zero_val * pos_three));
    Assert(as_int(SMUL(zero_lc)(neg_three_lc)) == (zero_val * neg_three),
        "SMUL(0)(-3) should be " + std::to_string(zero_val * neg_three));

    Assert(as_int(SDIV(pos_five_lc)(pos_two_lc)) == (pos_five / pos_two),
        "SDIV(5)(2) should be " + std::to_string(pos_five / pos_two));
    Assert(as_int(SDIV(pos_five_lc)(neg_two_lc)) == (pos_five / neg_two),
        "SDIV(5)(-2) should be " + std::to_string(pos_five / neg_two));
    Assert(as_int(SDIV(neg_five_lc)(pos_two_lc)) == (neg_five / pos_two),
        "SDIV(-5)(2) should be " + std::to_string(neg_five / pos_two));
    Assert(as_int(SDIV(neg_five_lc)(neg_two_lc)) == (neg_five / neg_two),
        "SDIV(-5)(-2) should be " + std::to_string(neg_five / neg_two));

    Assert(as_int(SMOD(pos_five_lc)(pos_two_lc)) == (pos_five % pos_two),
        "SMOD(5)(2) should be " + std::to_string(pos_five % pos_two));
    Assert(as_int(SMOD(pos_five_lc)(neg_two_lc)) == (pos_five % neg_two),
        "SMOD(5)(-2) should be " + std::to_string(pos_five % neg_two));  // C++ % sign matches dividend
    Assert(as_int(SMOD(neg_five_lc)(pos_two_lc)) == (neg_five % pos_two),
        "SMOD(-5)(2) should be " + std::to_string(neg_five % pos_two));
    Assert(as_int(SMOD(neg_five_lc)(neg_two_lc)) == (neg_five % neg_two),
        "SMOD(-5)(-2) should be " + std::to_string(neg_five % neg_two));
}
