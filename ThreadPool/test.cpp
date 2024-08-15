#include <bits/stdc++.h>

#include "ThreadPool.h"

#ifndef THREAD_POOL_SIZE
#    define THREAD_POOL_SIZE 4
#endif

#ifndef N
#    define N 1000000000
#endif

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
    cout << "Number of threads: " << THREAD_POOL_SIZE << endl;
    cout << "Number of ones to sum: " << N << endl;

    ThreadPool pool(THREAD_POOL_SIZE);

    long long sum = 0;
    mutex     sum_mutex;

    auto task = [&sum, &sum_mutex](long long start, long long end) {
        long long local_sum = 0;
        for (long long i = start; i < end; ++i) { local_sum += 1; }
        lock_guard<mutex> lock(sum_mutex);
        sum += local_sum;
    };

    measure_time("Summing", [&]() {
        long long chunk_size = N / THREAD_POOL_SIZE;
        for (int i = 0; i < THREAD_POOL_SIZE; ++i)
        {
            long long start = i * chunk_size;
            long long end   = (i == THREAD_POOL_SIZE - 1) ? N : start + chunk_size;
            pool.EnQueue(task, start, end);
        }

        pool.Sync();
    });

    cout << "Sum: " << sum << endl;
}