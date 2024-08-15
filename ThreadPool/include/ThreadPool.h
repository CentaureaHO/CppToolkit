#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

namespace Celesteria
{

    /**
     * @brief 类型定义，用于获取函数的返回类型
     */
    template <class F, class... Args>
    using RetType = typename std::invoke_result<F, Args...>::type;

    /**
     * @brief 线程池类
     *
     * 提供一个固定数量的线程来执行任务，支持任务的异步提交和同步等待。
     */
    class ThreadPool
    {
      private:
        std::vector<pthread_t>            Workers;      ///< 工作线程
        std::queue<std::function<void()>> Tasks;        ///< 任务队列
        std::mutex                        QueueMutex;   ///< 任务队列的互斥锁
        std::condition_variable           CondVar;      ///< 任务队列的条件变量
        std::condition_variable           FinishedVar;  ///< 所有任务完成的条件变量
        bool                              Stop;         ///< 停止线程池的标志
        unsigned int                      ActiveTasks;  ///< 活跃任务计数

      public:
        /**
         * @brief 构造函数
         *
         * @param ThreadNum 线程池中的线程数量
         */
        ThreadPool(unsigned int ThreadNum);

        /**
         * @brief 析构函数
         *
         * 释放所有线程并清理资源。
         */
        ~ThreadPool();

        /**
         * @brief 异步提交任务到线程池
         *
         * @tparam F 函数类型
         * @tparam Args 参数类型
         * @param ThFunc 函数对象
         * @param args 函数参数
         * @return std::future<RetType<F, Args...>> 任务的返回值
         */
        template <class F, class... Args>
        std::future<RetType<F, Args...>> EnQueue(F&& ThFunc, Args&&... args);

        /**
         * @brief 运行线程池中的线程
         *
         * 线程从任务队列中获取任务并执行。
         */
        void Run();

        /**
         * @brief 同步等待所有任务完成
         */
        void Sync();

        /**
         * @brief 停止线程池
         *
         * 停止所有线程并等待所有任务完成。
         */
        void StopPool();
    };

    /**
     * @brief 异步提交任务到线程池
     *
     * @tparam F 函数类型
     * @tparam Args 参数类型
     * @param ThFunc 函数对象
     * @param args 函数参数
     * @return std::future<RetType<F, Args...>> 任务的返回值
     */
    template <class F, class... Args>
    std::future<RetType<F, Args...>> ThreadPool::EnQueue(F&& ThFunc, Args&&... args)
    {
        auto Task = std::make_shared<std::packaged_task<RetType<F, Args...>()>>(
            std::bind(std::forward<F>(ThFunc), std::forward<Args>(args)...));

        std::future<RetType<F, Args...>> Res = Task->get_future();
        {
            std::unique_lock<std::mutex> Lock(QueueMutex);
            Tasks.emplace([Task] { (*Task)(); });
        }
        CondVar.notify_one();
        return Res;
    }

}  // namespace Celesteria