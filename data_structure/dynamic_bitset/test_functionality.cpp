#include <dynamic_bitset.h>

#include <iostream>
#include <random>
#include <string>
#include <cassert>
#include <functional>
#include <bitset>

#ifdef DIFF_TEST
#    include <boost/dynamic_bitset.hpp>
#endif

void test_basic_operations()
{
    std::cout << "Running basic operations tests..." << std::endl;

    Cele::dynamic_bitset bs1(10);
    assert(bs1.size() == 10);
    assert(bs1.none());
    assert(!bs1.any());
    assert(bs1.count() == 0);

    bs1.set(3);
    bs1.set(7);
    assert(bs1.test(3));
    assert(bs1.test(7));
    assert(!bs1.test(0));
    assert(bs1.count() == 2);
    assert(bs1.any());
    assert(!bs1.all());

    bs1.reset(3);
    assert(!bs1.test(3));
    assert(bs1.count() == 1);

    bs1.flip();
    assert(!bs1.test(7));
    assert(bs1.test(0));
    assert(bs1.test(1));
    assert(bs1.count() == 9);

    std::cout << "Basic operations: PASSED" << std::endl;
}

void test_constructors()
{
    std::cout << "Running constructor tests..." << std::endl;

    Cele::dynamic_bitset bs1;
    assert(bs1.size() == 0);
    assert(bs1.none());

    Cele::dynamic_bitset bs2(10, 5);  // 0000000101
    assert(bs2.size() == 10);
    assert(bs2.test(0) == true);
    assert(bs2.test(2) == true);
    assert(bs2.count() == 2);

    Cele::dynamic_bitset bs3("1010", 0, std::string::npos, '0', '1');
    assert(bs3.size() == 4);
    assert(bs3.test(0) == false);
    assert(bs3.test(1) == true);
    assert(bs3.test(2) == false);
    assert(bs3.test(3) == true);
    assert(bs3.count() == 2);

    Cele::dynamic_bitset bs4(bs3);
    assert(bs4.size() == 4);
    assert(bs4.test(1) == true);
    assert(bs4.test(3) == true);
    assert(bs4.count() == 2);

    Cele::dynamic_bitset bs5(std::move(bs4));
    assert(bs5.size() == 4);
    assert(bs5.test(1) == true);
    assert(bs5.test(3) == true);
    assert(bs5.count() == 2);
    assert(bs4.size() == 0);

    Cele::dynamic_bitset bs6;
    bs6 = bs5;
    assert(bs6.size() == 4);
    assert(bs6.test(1) == true);
    assert(bs6.test(3) == true);
    assert(bs6.count() == 2);

    Cele::dynamic_bitset bs7;
    bs7 = std::move(bs6);
    assert(bs7.size() == 4);
    assert(bs7.test(1) == true);
    assert(bs7.test(3) == true);
    assert(bs7.count() == 2);
    assert(bs6.size() == 0);

    std::cout << "Constructor tests: PASSED" << std::endl;
}

void test_bitwise_operations()
{
    std::cout << "Running bitwise operations tests..." << std::endl;

    Cele::dynamic_bitset bs1(10);
    bs1.set(1);
    bs1.set(3);
    bs1.set(5);
    bs1.set(7);

    Cele::dynamic_bitset bs2(10);
    bs2.set(1);
    bs2.set(2);
    bs2.set(5);
    bs2.set(8);

    Cele::dynamic_bitset bs_and = bs1 & bs2;
    assert(bs_and.size() == 10);
    assert(bs_and.test(1) == true);
    assert(bs_and.test(5) == true);
    assert(bs_and.count() == 2);

    Cele::dynamic_bitset bs_or = bs1 | bs2;
    assert(bs_or.size() == 10);
    assert(bs_or.test(1) == true);
    assert(bs_or.test(2) == true);
    assert(bs_or.test(3) == true);
    assert(bs_or.test(5) == true);
    assert(bs_or.test(7) == true);
    assert(bs_or.test(8) == true);
    assert(bs_or.count() == 6);

    Cele::dynamic_bitset bs_xor = bs1 ^ bs2;
    assert(bs_xor.size() == 10);
    assert(bs_xor.test(1) == false);
    assert(bs_xor.test(2) == true);
    assert(bs_xor.test(3) == true);
    assert(bs_xor.test(5) == false);
    assert(bs_xor.test(7) == true);
    assert(bs_xor.test(8) == true);
    assert(bs_xor.count() == 4);

    Cele::dynamic_bitset bs_not = ~bs1;
    assert(bs_not.size() == 10);
    assert(bs_not.test(0) == true);
    assert(bs_not.test(1) == false);
    assert(bs_not.test(2) == true);
    assert(bs_not.test(3) == false);
    assert(bs_not.count() == 6);

    std::cout << "Bitwise operations: PASSED" << std::endl;
}

void test_batch_operations()
{
    std::cout << "Running batch operations tests..." << std::endl;

    Cele::dynamic_bitset bs(100);
    assert(bs.none());

    bs.set();
    assert(bs.all());
    assert(bs.count() == 100);

    bs.reset();
    assert(bs.none());
    assert(bs.count() == 0);

    bs.set_range(10, 20);
    assert(bs.count() == 20);
    for (size_t i = 0; i < 100; ++i)
    {
        if (i >= 10 && i < 30)
            assert(bs.test(i) == true);
        else
            assert(bs.test(i) == false);
    }

    bs.set_range(15, 10, false);
    assert(bs.count() == 10);
    for (size_t i = 0; i < 100; ++i)
    {
        if ((i >= 10 && i < 15) || (i >= 25 && i < 30))
            assert(bs.test(i) == true);
        else
            assert(bs.test(i) == false);
    }

    std::cout << "Batch operations: PASSED" << std::endl;
}

void test_resize()
{
    std::cout << "Running resize tests..." << std::endl;

    Cele::dynamic_bitset bs(10);
    bs.set(1);
    bs.set(5);
    bs.set(9);
    assert(bs.count() == 3);

    bs.resize(20);
    assert(bs.size() == 20);
    assert(bs.test(1) == true);
    assert(bs.test(5) == true);
    assert(bs.test(9) == true);
    assert(bs.test(15) == false);
    assert(bs.count() == 3);

    bs.resize(30, true);
    assert(bs.size() == 30);
    assert(bs.test(1) == true);
    assert(bs.test(5) == true);
    assert(bs.test(9) == true);
    assert(bs.test(15) == true);
    assert(bs.test(25) == true);
    assert(bs.count() == 23);

    bs.resize(5);
    assert(bs.size() == 5);
    assert(bs.test(1) == true);
    assert(bs.count() == 1);

    std::cout << "Resize tests: PASSED" << std::endl;
}

void test_string_conversion()
{
    std::cout << "Running string conversion tests..." << std::endl;

    Cele::dynamic_bitset bs(10);
    bs.set(1);
    bs.set(4);
    bs.set(9);

    std::string str = bs.to_string();
    assert(str == "1000100010");

    std::string str2 = bs.to_string('F', 'T');
    assert(str2 == "TFFTFTFFTF");

    std::cout << "String conversion: PASSED" << std::endl;
}

#ifdef DIFF_TEST
void test_against_boost()
{
    std::cout << "Running tests against boost::dynamic_bitset..." << std::endl;

    const size_t size       = 10000;
    const size_t operations = 1000;

    Cele::dynamic_bitset    cele_bs(size);
    boost::dynamic_bitset<> boost_bs(size);

    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_int_distribution<size_t> dis(0, size - 1);

    for (size_t i = 0; i < operations; ++i)
    {
        size_t pos = dis(gen);
        cele_bs.set(pos);
        boost_bs[pos] = 1;
    }

    for (size_t i = 0; i < size; ++i) assert(cele_bs.test(i) == boost_bs[i]);

    assert(cele_bs.count() == boost_bs.count());

    Cele::dynamic_bitset    cele_bs2(size);
    boost::dynamic_bitset<> boost_bs2(size);

    for (size_t i = 0; i < operations; ++i)
    {
        size_t pos = dis(gen);
        cele_bs2.set(pos);
        boost_bs2[pos] = 1;
    }

    Cele::dynamic_bitset    cele_and  = cele_bs & cele_bs2;
    boost::dynamic_bitset<> boost_and = boost_bs & boost_bs2;

    Cele::dynamic_bitset    cele_or  = cele_bs | cele_bs2;
    boost::dynamic_bitset<> boost_or = boost_bs | boost_bs2;

    Cele::dynamic_bitset    cele_xor  = cele_bs ^ cele_bs2;
    boost::dynamic_bitset<> boost_xor = boost_bs ^ boost_bs2;

    Cele::dynamic_bitset    cele_not  = ~cele_bs;
    boost::dynamic_bitset<> boost_not = ~boost_bs;

    for (size_t i = 0; i < size; ++i)
    {
        assert(cele_and.test(i) == boost_and[i]);
        assert(cele_or.test(i) == boost_or[i]);
        assert(cele_xor.test(i) == boost_xor[i]);
        assert(cele_not.test(i) == boost_not[i]);
    }

    std::cout << "Tests against boost::dynamic_bitset: PASSED" << std::endl;
}
#endif

int main()
{
    std::cout << "=================================================" << std::endl;
    std::cout << "      dynamic_bitset Functionality Test Suite    " << std::endl;
    std::cout << "=================================================" << std::endl;

    try
    {
        test_basic_operations();
        test_constructors();
        test_bitwise_operations();
        test_batch_operations();
        test_resize();
        test_string_conversion();

#ifdef DIFF_TEST
        test_against_boost();
#endif

        std::cout << "=================================================" << std::endl;
        std::cout << "All tests PASSED successfully!" << std::endl;
        std::cout << "=================================================" << std::endl;
    } catch (const std::exception& e)
    {
        std::cerr << "TEST FAILED: " << e.what() << std::endl;
        return 1;
    } catch (...)
    {
        std::cerr << "TEST FAILED with unknown exception" << std::endl;
        return 1;
    }

    return 0;
}
