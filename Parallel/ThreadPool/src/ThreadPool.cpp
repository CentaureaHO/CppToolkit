#include "ThreadPool.h"

namespace Celesteria
{

    /**
     * @brief 线程入口函数
     *
     * @param args 线程池对象
     * @return void* 返回值
     */
    void* ThreadEntry(void* args)
    {
        auto* Pool = static_cast<ThreadPool*>(args);
        Pool->Run();
        return nullptr;
    }

    /**
     * @brief 线程池构造函数
     *
     * @param ThreadNum 线程池中的线程数量
     */
    ThreadPool::ThreadPool(unsigned int ThreadNum) : Stop(0), ActiveTasks(0)
    {
        Workers.resize(ThreadNum);
        for (auto& Worker : Workers) { pthread_create(&Worker, nullptr, ThreadEntry, this); }
    }

    /**
     * @brief 线程池析构函数
     */
    ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> Lock(QueueMutex);
            Stop = 1;
        }
        CondVar.notify_all();
        for (auto& Worker : Workers) { pthread_join(Worker, nullptr); }
    }

    /**
     * @brief 运行线程池中的线程
     *
     * 线程从任务队列中获取任务并执行。
     */
    void ThreadPool::Run()
    {
        while (true)
        {
            std::function<void()> Task;
            {
                std::unique_lock<std::mutex> Lock(QueueMutex);
                CondVar.wait(Lock, [this] { return Stop || !Tasks.empty(); });
                if (Stop && Tasks.empty()) break;
                Task = std::move(Tasks.front());
                Tasks.pop();
                ++ActiveTasks;
            }
            Task();
            {
                std::unique_lock<std::mutex> Lock(QueueMutex);
                --ActiveTasks;
                if (ActiveTasks == 0 && Tasks.empty()) FinishedVar.notify_all();
            }
        }
    }

    /**
     * @brief 同步等待所有任务完成
     */
    void ThreadPool::Sync()
    {
        std::unique_lock<std::mutex> Lock(QueueMutex);
        FinishedVar.wait(Lock, [this] { return Tasks.empty() && ActiveTasks == 0; });
    }

    /**
     * @brief 停止线程池
     *
     * 停止所有线程并等待所有任务完成。
     */
    void ThreadPool::StopPool()
    {
        {
            std::unique_lock<std::mutex> Lock(QueueMutex);
            Stop = true;
        }
        CondVar.notify_all();
        Sync();
    }

}  // namespace Celesteria