#include "lc_bool.h"
#include "common.h"

void test_bool_basic();
void test_bool_and();
void test_bool_or();
void test_bool_not();
void test_bool_xor();
void test_bool_xnor();

void test_bool()
{
    std::cout << "\n[Lambda Calculus] Testing Boolean types.\n";

    test_bool_basic();
    test_bool_and();
    test_bool_or();
    test_bool_not();
    test_bool_xor();
    test_bool_xnor();

    std::cout << "[Lambda Calculus] Boolean types tests passed.\n";
}

using namespace Cele::LC;

void test_bool_basic()
{
    Assert(as_bool(TRUE), "TRUE should be true");
    Assert(!as_bool(FALSE), "FALSE should be false");
}

void test_bool_and()
{
    Assert(as_bool(AND(TRUE)(TRUE)), "AND(TRUE)(TRUE) should be true");
    Assert(!as_bool(AND(TRUE)(FALSE)), "AND(TRUE)(FALSE) should be false");
    Assert(!as_bool(AND(FALSE)(TRUE)), "AND(FALSE)(TRUE) should be false");
    Assert(!as_bool(AND(FALSE)(FALSE)), "AND(FALSE)(FALSE) should be false");
}

void test_bool_or()
{
    Assert(as_bool(OR(TRUE)(TRUE)), "OR(TRUE)(TRUE) should be true");
    Assert(as_bool(OR(TRUE)(FALSE)), "OR(TRUE)(FALSE) should be true");
    Assert(as_bool(OR(FALSE)(TRUE)), "OR(FALSE)(TRUE) should be true");
    Assert(!as_bool(OR(FALSE)(FALSE)), "OR(FALSE)(FALSE) should be false");
}

void test_bool_not()
{
    Assert(!as_bool(NOT(TRUE)), "NOT(TRUE) should be false");
    Assert(as_bool(NOT(FALSE)), "NOT(FALSE) should be true");
}

void test_bool_xor()
{
    Assert(!as_bool(XOR(TRUE)(TRUE)), "XOR(TRUE)(TRUE) should be false");
    Assert(as_bool(XOR(TRUE)(FALSE)), "XOR(TRUE)(FALSE) should be true");
    Assert(as_bool(XOR(FALSE)(TRUE)), "XOR(FALSE)(TRUE) should be true");
    Assert(!as_bool(XOR(FALSE)(FALSE)), "XOR(FALSE)(FALSE) should be false");
}

void test_bool_xnor()
{
    Assert(as_bool(XNOR(TRUE)(TRUE)), "XNOR(TRUE)(TRUE) should be true");
    Assert(!as_bool(XNOR(TRUE)(FALSE)), "XNOR(TRUE)(FALSE) should be false");
    Assert(!as_bool(XNOR(FALSE)(TRUE)), "XNOR(FALSE)(TRUE) should be false");
    Assert(as_bool(XNOR(FALSE)(FALSE)), "XNOR(FALSE)(FALSE) should be true");
}
