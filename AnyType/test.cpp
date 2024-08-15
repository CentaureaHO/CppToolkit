#include <bits/stdc++.h>

#include "AnyType.h"

using namespace std;
using namespace Celesteria;
using namespace chrono;

template <typename Func>
void measure_time(const string& label, Func&& func)
{
    auto start = high_resolution_clock::now();
    func();
    auto end      = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    cout << label << ": " << duration << " microseconds" << endl;
}

int main()
{
#ifdef USE_RAW_POINTER
    cout << "Using raw pointer" << endl;
#else
    cout << "Using shared pointer" << endl;
#endif

    AnyType* any = nullptr;

    measure_time("Create 1e6 objects", [&]() { any = new AnyType[1000000]; });

    measure_time("Cast 1e6 integers", [&]() {
        for (int i = 0; i < 1000000; ++i)
        {
            any[i]    = i;
            int value = any[i].cast<int>();
        }
    });

    measure_time("Cast 1e6 doubles", [&]() {
        for (int i = 0; i < 1000000; ++i)
        {
            any[i]       = 3.14;
            double value = any[i].cast<double>();
        }
    });

    measure_time("Cast 1e6 strings", [&]() {
        for (int i = 0; i < 1000000; ++i)
        {
            any[i]       = string("Hello, World!");
            string value = any[i].cast<string>();
        }
    });

    measure_time("Delete 1e6 objects", [&]() { delete[] any; });
}