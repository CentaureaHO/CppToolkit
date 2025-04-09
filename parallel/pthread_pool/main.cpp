#include <pthread_pool.h>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

int main()
{
    Cele::Async::PThreadPool              tp(16);
    std::vector<Cele::Async::Result<int>> futures;

    for (int i = 0; i < 100; i++)
    {
        futures.push_back(tp.EnQueue([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));

            std::stringstream ss;
            ss << "Task " << i << " done\n";
            std::cout << ss.str();

            return i;
        }));
    }

    tp.Sync();

    for (auto& f : futures) std::cout << f.get() << " ";
}
