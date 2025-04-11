#include <iostream>
#include <chrono>
#include <vector>
#include <random>
#include <bitset>
#include <iomanip>
#include <functional>
#include <sstream>

#include "include/dynamic_bitset.h"

#ifdef DIFF_TEST
#    include <boost/dynamic_bitset.hpp>
#endif

class Timer
{
  private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::string                                    name;
    bool                                           print;

  public:
    Timer(const std::string& timer_name, bool p = true) : name(timer_name), print(p)
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        if (print) { std::cout << "[" << name << "] Time: " << duration << " microseconds" << std::endl; }
    }

    long long elapsed_us()
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    }
};

template <typename Func>
double benchmark(const std::string& name, Func func, int iterations = 10)
{
    std::vector<long long> times;

    func();

    for (int i = 0; i < iterations; ++i)
    {
        Timer timer(name, false);
        func();
        times.push_back(timer.elapsed_us());
    }
    double sum = 0;
    for (auto t : times) sum += t;
    double avg = sum / times.size();

    double variance = 0;
    for (auto t : times) variance += (t - avg) * (t - avg);
    variance /= times.size();
    double stddev = std::sqrt(variance);

    std::cout << "[" << name << "] Average time: " << avg << " microseconds, StdDev: " << stddev << " microseconds"
              << std::endl;
    return avg;
}

void bench_random_set(size_t size, size_t operations)
{
    std::cout << "\n===== Random Bit Setting Performance Test (Size: " << size << ", Operations: " << operations
              << ") =====" << std::endl;

    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_int_distribution<size_t> dis(0, size - 1);

    std::vector<size_t> positions(operations);
    for (size_t i = 0; i < operations; ++i) positions[i] = dis(gen);

    benchmark("Cele::dynamic_bitset", [&]() {
        Cele::dynamic_bitset bs(size);
        for (size_t i = 0; i < operations; ++i) bs.set(positions[i]);
    });

#ifdef DIFF_TEST
    benchmark("boost::dynamic_bitset", [&]() {
        boost::dynamic_bitset<> bs(size);
        for (size_t i = 0; i < operations; ++i) bs[positions[i]] = 1;
    });
#endif
}

void bench_set_range(size_t size)
{
    std::cout << "\n===== Batch Bit Setting Performance Test (Size: " << size << ") =====" << std::endl;

    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_int_distribution<size_t> start_dis(0, size / 2);
    std::uniform_int_distribution<size_t> len_dis(size / 10, size / 5);

    size_t start = start_dis(gen);
    size_t count = std::min(len_dis(gen), size - start);

    std::cout << "Test range: [" << start << ", " << (start + count - 1) << "]" << std::endl;

    benchmark("Cele::dynamic_bitset bit-by-bit", [&]() {
        Cele::dynamic_bitset bs(size);
        for (size_t i = start; i < start + count; ++i) bs.set(i);
    });

    benchmark("Cele::dynamic_bitset batch setting", [&]() {
        Cele::dynamic_bitset bs(size);
        bs.set_range(start, count);
    });

#ifdef DIFF_TEST
    benchmark("boost::dynamic_bitset bit-by-bit", [&]() {
        boost::dynamic_bitset<> bs(size);
        for (size_t i = start; i < start + count; ++i) bs[i] = 1;
    });
#endif
}

void bench_bitwise_ops(size_t size)
{
    std::cout << "\n===== Bitwise Operations Performance Test (Size: " << size << ") =====" << std::endl;

    Cele::dynamic_bitset a(size);
    Cele::dynamic_bitset b(size);

    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_int_distribution<size_t> dis(0, size - 1);

    for (size_t i = 0; i < size / 10; ++i)
    {
        a.set(dis(gen));
        b.set(dis(gen));
    }

#ifdef DIFF_TEST
    boost::dynamic_bitset<> boost_a(size);
    boost::dynamic_bitset<> boost_b(size);

    for (size_t i = 0; i < size; ++i)
    {
        if (a.test(i)) boost_a[i] = 1;
        if (b.test(i)) boost_b[i] = 1;
    }
#endif

    benchmark("Cele::dynamic_bitset AND", [&]() { Cele::dynamic_bitset result = a & b; });

    benchmark("Cele::dynamic_bitset OR", [&]() { Cele::dynamic_bitset result = a | b; });

    benchmark("Cele::dynamic_bitset XOR", [&]() { Cele::dynamic_bitset result = a ^ b; });

    benchmark("Cele::dynamic_bitset NOT", [&]() { Cele::dynamic_bitset result = ~a; });

#ifdef DIFF_TEST
    benchmark("boost::dynamic_bitset AND", [&]() { boost::dynamic_bitset<> result = boost_a & boost_b; });

    benchmark("boost::dynamic_bitset OR", [&]() { boost::dynamic_bitset<> result = boost_a | boost_b; });

    benchmark("boost::dynamic_bitset XOR", [&]() { boost::dynamic_bitset<> result = boost_a ^ boost_b; });

    benchmark("boost::dynamic_bitset NOT", [&]() { boost::dynamic_bitset<> result = ~boost_a; });
#endif
}

void bench_count(size_t size)
{
    std::cout << "\n===== Count Performance Test (Size: " << size << ") =====" << std::endl;

    Cele::dynamic_bitset                  bs(size);
    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_int_distribution<size_t> dis(0, size - 1);
    std::uniform_int_distribution<int>    flip_dis(0, 1);

    for (size_t i = 0; i < size; ++i)
        if (flip_dis(gen)) bs.set(i);

#ifdef DIFF_TEST
    boost::dynamic_bitset<> boost_bs(size);
    for (size_t i = 0; i < size; ++i) boost_bs[i] = bs.test(i);
#endif

    benchmark("Cele::dynamic_bitset count", [&]() {
        volatile size_t count = bs.count();
        (void)count;
    });

#ifdef DIFF_TEST
    benchmark("boost::dynamic_bitset count", [&]() {
        volatile size_t count = boost_bs.count();
        (void)count;
    });
#endif
}

void print_performance_report()
{
    std::cout << "\n=========================================================" << std::endl;
    std::cout << "            dynamic_bitset Performance Report" << std::endl;
    std::cout << "=========================================================" << std::endl;

    std::cout << "Compilation environment:" << std::endl;

#ifdef __GNUC__
    std::cout << "  GCC/G++ version: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
#endif

#ifdef __clang__
    std::cout << "  Clang version: " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__
              << std::endl;
#endif

#ifdef DIFF_TEST
    std::cout << "  Boost comparison tests enabled" << std::endl;
#else
    std::cout << "  Boost comparison tests disabled (use -DENABLE_DIFF_TEST=ON to enable)" << std::endl;
#endif

#if defined(__POPCNT__) && defined(__GNUC__)
    std::cout << "  Hardware POPCNT instruction enabled" << std::endl;
#else
    std::cout << "  Hardware POPCNT instruction disabled" << std::endl;
#endif

#ifdef NDEBUG
    std::cout << "  Optimization mode enabled (NDEBUG)" << std::endl;
#else
    std::cout << "  Debug mode" << std::endl;
#endif

    std::cout << "=========================================================" << std::endl;
}

int main()
{
    print_performance_report();

    /*
    // Small tests
    bench_random_set(10000, 5000);
    bench_set_range(10000);
    bench_bitwise_ops(10000);
    bench_count(10000);

    // Medium tests
    bench_random_set(100000, 50000);
    bench_set_range(100000);
    bench_bitwise_ops(100000);
    bench_count(100000);
    */

    // Large tests
    bench_random_set(1000000, 500000);
    bench_set_range(1000000);
    bench_bitwise_ops(1000000);
    bench_count(1000000);

    // Extra large tests
    bench_random_set(10000000, 5000000);
    bench_set_range(10000000);
    bench_bitwise_ops(10000000);
    bench_count(10000000);

    return 0;
}
