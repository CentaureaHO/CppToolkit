#include "lc_list.h"
#include "common.h"
#include "lc_natural.h"
#include "lc_bool.h"
#include <vector>

void test_list_basic();
void test_list_length();
void test_list_append();
void test_list_reverse();
void test_list_map();
void test_list_filter();
void test_list_range();
void test_list_drop_take();
void test_list_index();
void test_list_any_all();
void test_list_mixed();

void test_list()
{
    std::cout << "\n[Lambda Calculus] Testing List types.\n";

    test_list_basic();
    test_list_length();
    test_list_append();
    test_list_reverse();
    test_list_map();
    test_list_filter();
    test_list_range();
    test_list_drop_take();
    test_list_index();
    test_list_any_all();
    test_list_mixed();

    std::cout << "[Lambda Calculus] List types tests passed.\n";
}

using namespace Cele::LC;

#define ListDef(prefix, type)                 \
    using prefix##List   = List<type>;        \
    auto prefix##Nil     = NIL<type>;         \
    auto prefix##Head    = HEAD<type>;        \
    auto prefix##Tail    = TAIL<type>;        \
    auto prefix##Cons    = CONS<type>;        \
    auto prefix##IsNil   = IS_NIL<type>;      \
    auto prefix##Length  = LENGTH<type>;      \
    auto prefix##Append  = APPEND<type>;      \
    auto prefix##Reverse = REVERSE<type>;     \
    template <typename OutputU>               \
    auto prefix##MapTo  = MAP<type, OutputU>; \
    auto prefix##Filter = FILTER<type>;       \
    auto prefix##Index  = INDEX<type>;        \
    auto prefix##Any    = ANY<type>;          \
    auto prefix##All    = ALL<type>;

ListDef(Nat, Natural) ListDef(Bool, Bool) ListDef(String, std::string)

    auto LCRange = RANGE;

std::vector<uint32_t> as_vector_uint(const NatList& lc_list)
{
    std::vector<uint32_t> vec;
    NatList               current = lc_list;
    while (!as_bool(NatIsNil(current)))
    {
        vec.push_back(as_uint(NatHead(current)));
        current = NatTail(current);
    }
    return vec;
}

std::vector<bool> as_vector_bool(const BoolList& lc_list)
{
    std::vector<bool> vec;
    BoolList          current = lc_list;
    while (!as_bool(BoolIsNil(current)))
    {
        vec.push_back(as_bool(BoolHead(current)));
        current = BoolTail(current);
    }
    return vec;
}

void test_list_basic()
{
    auto nil_list_nat = NatNil();
    Assert(as_bool(NatIsNil(nil_list_nat)), "IS_NIL(NIL) should be TRUE");

    auto list1 = NatCons(ONE)(nil_list_nat);
    Assert(!as_bool(NatIsNil(list1)), "IS_NIL(CONS(ONE)(NIL)) should be FALSE");
    Assert(as_uint(NatHead(list1)) == 1, "HEAD(CONS(ONE)(NIL)) should be 1");
    Assert(as_bool(NatIsNil(NatTail(list1))), "TAIL(CONS(ONE)(NIL)) should be NIL");

    auto list2 = NatCons(TWO)(list1);
    Assert(!as_bool(NatIsNil(list2)), "IS_NIL(list2) should be FALSE");
    Assert(as_uint(NatHead(list2)) == 2, "HEAD(list2) should be 2");
    Assert(as_uint(NatHead(NatTail(list2))) == 1, "HEAD(TAIL(list2)) should be 1");
    Assert(as_bool(NatIsNil(NatTail(NatTail(list2)))), "TAIL(TAIL(list2)) should be NIL");

    bool head_threw = false;
    try
    {
        NatHead(nil_list_nat);
    } catch (const std::runtime_error& e)
    {
        head_threw = true;
        Assert(std::string(e.what()).find("HEAD called on an empty list") != std::string::npos,
            "HEAD on NIL exception message check");
    }
    Assert(head_threw, "HEAD(NIL) should throw std::runtime_error");

    bool tail_threw = false;
    try
    {
        NatTail(nil_list_nat);
    } catch (const std::runtime_error& e)
    {
        tail_threw = true;
        Assert(std::string(e.what()).find("TAIL called on an empty list") != std::string::npos,
            "TAIL on NIL exception message check");
    }
    Assert(tail_threw, "TAIL(NIL) should throw std::runtime_error");
}

void test_list_length()
{
    auto nil_list = NatNil();
    Assert(as_uint(NatLength(nil_list)) == 0, "LENGTH(NIL) should be 0");

    auto list1 = NatCons(ONE)(NatNil());
    Assert(as_uint(NatLength(list1)) == 1, "LENGTH([1]) should be 1");

    auto list3 = NatCons(THREE)(NatCons(TWO)(NatCons(ONE)(NatNil())));
    Assert(as_uint(NatLength(list3)) == 3, "LENGTH([3,2,1]) should be 3");
}

void test_list_append()
{
    auto nil1 = NatNil();
    auto nil2 = NatNil();
    Assert(as_bool(NatIsNil(NatAppend(nil1)(nil2))), "APPEND(NIL)(NIL) should be NIL");

    auto list1 = NatCons(ONE)(NatCons(TWO)(NatNil()));
    auto list2 = NatCons(THREE)(NatCons(FOUR)(NatNil()));

    auto appended1 = NatAppend(nil1)(list1);
    Assert(as_vector_uint(appended1) == std::vector<uint32_t>({1, 2}), "APPEND(NIL, [1,2])");

    auto appended2 = NatAppend(list1)(nil1);
    Assert(as_vector_uint(appended2) == std::vector<uint32_t>({1, 2}), "APPEND([1,2], NIL)");

    auto appended3 = NatAppend(list1)(list2);
    Assert(as_vector_uint(appended3) == std::vector<uint32_t>({1, 2, 3, 4}), "APPEND([1,2], [3,4])");
}

void test_list_reverse()
{
    auto nil_list = NatNil();
    Assert(as_bool(NatIsNil(NatReverse(nil_list))), "REVERSE(NIL) should be NIL");

    auto list1 = NatCons(ONE)(NatNil());
    Assert(as_vector_uint(NatReverse(list1)) == std::vector<uint32_t>({1}), "REVERSE([1])");

    auto list3 = NatCons(ONE)(NatCons(TWO)(NatCons(THREE)(NatNil())));
    Assert(as_vector_uint(NatReverse(list3)) == std::vector<uint32_t>({3, 2, 1}), "REVERSE([1,2,3])");
}

void test_list_map()
{
    auto nil_list_nat        = NatNil();
    auto map_to_bool_is_even = NatMapTo<Bool>(IS_EVEN);

    auto result_nil = map_to_bool_is_even(nil_list_nat);
    Assert(as_bool(BoolIsNil(result_nil)), "MAP(IS_EVEN)(NIL) should be NIL");

    auto              list_nums      = NatCons(ONE)(NatCons(TWO)(NatCons(THREE)(NatNil())));
    auto              list_bools     = map_to_bool_is_even(list_nums);
    std::vector<bool> expected_bools = {false, true, false};
    Assert(as_vector_bool(list_bools) == expected_bools, "MAP(IS_EVEN)([1,2,3])");

    auto                  map_succ       = NatMapTo<Natural>(SUCC);
    auto                  list_succs     = map_succ(list_nums);
    std::vector<uint32_t> expected_succs = {2, 3, 4};
    Assert(as_vector_uint(list_succs) == expected_succs, "MAP(SUCC)([1,2,3])");
}

void test_list_filter()
{
    auto nil_list_nat   = NatNil();
    auto filter_is_even = NatFilter(IS_EVEN);

    auto result_nil = filter_is_even(nil_list_nat);
    Assert(as_bool(NatIsNil(result_nil)), "FILTER(IS_EVEN)(NIL) should be NIL");

    auto list_nums      = NatCons(ONE)(NatCons(TWO)(NatCons(THREE)(NatCons(FOUR)(NatCons(FIVE)(NatNil())))));
    auto filtered_evens = filter_is_even(list_nums);
    std::vector<uint32_t> expected_evens = {2, 4};
    Assert(as_vector_uint(filtered_evens) == expected_evens, "FILTER(IS_EVEN)([1,2,3,4,5])");

    auto                  filter_is_odd = NatFilter(IS_ODD);
    auto                  filtered_odds = filter_is_odd(list_nums);
    std::vector<uint32_t> expected_odds = {1, 3, 5};
    Assert(as_vector_uint(filtered_odds) == expected_odds, "FILTER(IS_ODD)([1,2,3,4,5])");

    auto filter_always_false = NatFilter([](const Natural&) { return FALSE; });
    Assert(as_bool(NatIsNil(filter_always_false(list_nums))), "FILTER(FALSE)(list) should be NIL");

    auto filter_always_true = NatFilter([](const Natural&) { return TRUE; });
    Assert(as_vector_uint(filter_always_true(list_nums)) == std::vector<uint32_t>({1, 2, 3, 4, 5}),
        "FILTER(TRUE)(list) should be list");
}

void test_list_range()
{
    auto range0_0 = LCRange(ZERO)(ZERO);
    Assert(as_bool(NatIsNil(range0_0)), "RANGE(0)(0) should be NIL");

    auto                  range0_3    = LCRange(ZERO)(THREE);
    std::vector<uint32_t> expected0_3 = {0, 1, 2};
    Assert(as_vector_uint(range0_3) == expected0_3, "RANGE(0)(3)");

    auto range3_3 = LCRange(THREE)(THREE);
    Assert(as_bool(NatIsNil(range3_3)), "RANGE(3)(3) should be NIL");

    auto range5_2 = LCRange(FIVE)(TWO);
    Assert(as_bool(NatIsNil(range5_2)), "RANGE(5)(2) should be NIL");
}

void test_list_drop_take()
{
    auto list_01234 = LCRange(ZERO)(FIVE);

    Assert(as_bool(NatIsNil(DROP(FIVE)(list_01234))), "DROP(5)([0,1,2,3,4]) should be NIL");
    Assert(as_bool(NatIsNil(DROP(SIX)(list_01234))), "DROP(6)([0,1,2,3,4]) should be NIL (drop more than length)");
    Assert(as_vector_uint(DROP(ZERO)(list_01234)) == std::vector<uint32_t>({0, 1, 2, 3, 4}), "DROP(0)([0,1,2,3,4])");
    Assert(as_vector_uint(DROP(TWO)(list_01234)) == std::vector<uint32_t>({2, 3, 4}), "DROP(2)([0,1,2,3,4])");
    Assert(as_bool(NatIsNil(DROP(ZERO)(NatNil()))), "DROP(0)(NIL) should be NIL");
    Assert(as_bool(NatIsNil(DROP(ONE)(NatNil()))), "DROP(1)(NIL) should be NIL");

    Assert(as_vector_uint(TAKE(FIVE)(list_01234)) == std::vector<uint32_t>({0, 1, 2, 3, 4}), "TAKE(5)([0,1,2,3,4])");
    Assert(as_vector_uint(TAKE(SIX)(list_01234)) == std::vector<uint32_t>({0, 1, 2, 3, 4}),
        "TAKE(6)([0,1,2,3,4]) (take more than length)");
    Assert(as_bool(NatIsNil(TAKE(ZERO)(list_01234))), "TAKE(0)([0,1,2,3,4]) should be NIL");
    Assert(as_vector_uint(TAKE(TWO)(list_01234)) == std::vector<uint32_t>({0, 1}), "TAKE(2)([0,1,2,3,4])");
    Assert(as_bool(NatIsNil(TAKE(ZERO)(NatNil()))), "TAKE(0)(NIL) should be NIL");
    Assert(as_bool(NatIsNil(TAKE(ONE)(NatNil()))), "TAKE(1)(NIL) should be NIL");
}

void test_list_index()
{
    auto list_012 = LCRange(ZERO)(THREE);

    Assert(as_uint(NatIndex(list_012)(ZERO)) == 0, "INDEX([0,1,2])(0) should be 0");
    Assert(as_uint(NatIndex(list_012)(ONE)) == 1, "INDEX([0,1,2])(1) should be 1");
    Assert(as_uint(NatIndex(list_012)(TWO)) == 2, "INDEX([0,1,2])(2) should be 2");

    bool index_threw_nil = false;
    try
    {
        NatIndex(NatNil())(ZERO);
    } catch (const std::runtime_error& e)
    {
        index_threw_nil = true;
        Assert(std::string(e.what()).find("INDEX: Index out of bounds") != std::string::npos,
            "INDEX on NIL exception message check");
    }
    Assert(index_threw_nil, "INDEX(NIL)(0) should throw");

    bool index_threw_oob = false;
    try
    {
        NatIndex(list_012)(THREE);
    } catch (const std::runtime_error& e)
    {
        index_threw_oob = true;
        Assert(std::string(e.what()).find("INDEX: Index out of bounds") != std::string::npos,
            "INDEX out of bounds exception message check");
    }
    Assert(index_threw_oob, "INDEX(list)(length) should throw");
}

void test_list_any_all()
{
    auto list_123 = NatCons(ONE)(NatCons(TWO)(NatCons(THREE)(NatNil())));
    auto list_246 = NatCons(TWO)(NatCons(FOUR)(NatCons(SIX)(NatNil())));
    auto nil_l    = NatNil();

    auto is_odd_pred  = IS_ODD;
    auto is_even_pred = IS_EVEN;

    Assert(as_bool(NatAny(is_odd_pred)(list_123)), "ANY(IS_ODD)([1,2,3]) should be TRUE");
    Assert(!as_bool(NatAny(is_odd_pred)(list_246)), "ANY(IS_ODD)([2,4,6]) should be FALSE");
    Assert(!as_bool(NatAny(is_odd_pred)(nil_l)), "ANY(IS_ODD)(NIL) should be FALSE");
    Assert(as_bool(NatAny(is_even_pred)(list_123)), "ANY(IS_EVEN)([1,2,3]) should be TRUE");

    Assert(!as_bool(NatAll(is_odd_pred)(list_123)), "ALL(IS_ODD)([1,2,3]) should be FALSE");
    Assert(as_bool(NatAll(is_even_pred)(list_246)), "ALL(IS_EVEN)([2,4,6]) should be TRUE");
    Assert(as_bool(NatAll(is_odd_pred)(nil_l)), "ALL(IS_ODD)(NIL) should be TRUE (vacuously true)");
    Assert(!as_bool(NatAll(is_even_pred)(list_123)), "ALL(IS_EVEN)([1,2,3]) should be FALSE");
}

void test_list_mixed()
{
    auto list_bools = BoolCons(TRUE)(BoolCons(FALSE)(BoolCons(TRUE)(BoolNil())));
    Assert(as_uint(BoolLength(list_bools)) == 3, "LENGTH of Bool list");
    Assert(as_bool(BoolHead(list_bools)) == true, "HEAD of Bool list");

    auto filter_true    = BoolFilter([](const Bool& b) { return b; });
    auto filtered_trues = filter_true(list_bools);
    Assert(as_uint(BoolLength(filtered_trues)) == 2, "FILTER TRUEs from Bool list length");
    Assert(as_bool(BoolHead(filtered_trues)) == true, "HEAD of filtered TRUEs");
    Assert(as_bool(BoolHead(BoolTail(filtered_trues))) == true, "HEAD(TAIL) of filtered TRUEs");

    auto              list_nums_for_map    = NatCons(ZERO)(NatCons(ONE)(NatCons(ZERO)(NatNil())));
    auto              map_is_zero          = NatMapTo<Bool>(IS_ZERO);
    auto              mapped_to_bools      = map_is_zero(list_nums_for_map);
    std::vector<bool> expected_map_is_zero = {true, false, true};
    Assert(as_vector_bool(mapped_to_bools) == expected_map_is_zero, "MAP(IS_ZERO) on [0,1,0]");

    auto list_str = StringCons(std::string("a"))(StringCons(std::string("b"))(StringNil()));
    Assert(StringHead(list_str) == "a", "HEAD of std::string list");
    Assert(StringHead(StringTail(list_str)) == "b", "HEAD(TAIL) of std::string list");
    Assert(as_uint(StringLength(list_str)) == 2, "LENGTH of std::string list");

    auto map_str_len   = StringMapTo<Natural>([](const std::string& s) { return as_lcnat(s.length()); });
    auto list_str_lens = map_str_len(list_str);
    Assert(as_uint(NatHead(list_str_lens)) == 1, "MAP string length, HEAD");
    Assert(as_uint(NatHead(NatTail(list_str_lens))) == 1, "MAP string length, HEAD(TAIL)");
}
