#ifndef __PTHREAD_POOL_H__
#define __PTHREAD_POOL_H__

#include "async_result.h"
#include <pthread.h>
#include <queue>
#include <tuple>
#include <vector>

namespace Cele::Async
{
    class ITask
    {
      public:
        virtual ~ITask()       = default;
        virtual void execute() = 0;
    };

    template <typename Func, typename... Args>
    class Task : public ITask
    {
        using RetType = decltype(std::apply(std::declval<Func>(), std::declval<std::tuple<Args...>>()));

        Func                  func;
        std::tuple<Args...>   args;
        SharedState<RetType>* state;

      public:
        Task(Func&& f, Args&&... a, SharedState<RetType>* s)
            : func(std::forward<Func>(f)), args(std::forward<Args>(a)...), state(s)
        {}

        void execute() override
        {
            if constexpr (std::is_void_v<RetType>)
            {
                std::apply(func, args);
                state->set_value();
            }
            else
                state->set_value(std::apply(func, args));

            state->dec_ref();
        }
    };

    class PThreadPool
    {
      private:
        std::vector<pthread_t> Workers;
        std::queue<ITask*>     Tasks;
        pthread_mutex_t        QueueMutex;
        pthread_cond_t         CondVar;
        pthread_cond_t         FinishedVar;
        bool                   Stop;
        unsigned int           ActiveTasks;

        static void* ThreadEntry(void* args);

      public:
        PThreadPool(unsigned int ThreadNum);
        ~PThreadPool();
        void Run();
        void Sync();

        template <typename F, typename... Args>
        auto EnQueue(F&& f, Args&&... args) -> Result<decltype(f(args...))>
        {
            using ReturnType = decltype(f(args...));
            Result<ReturnType> future;
            auto*              state = future.get_state();

            auto* task = new Task<F, Args...>(std::forward<F>(f), std::forward<Args>(args)..., state);

            pthread_mutex_lock(&QueueMutex);
            Tasks.push(task);
            pthread_mutex_unlock(&QueueMutex);
            pthread_cond_signal(&CondVar);

            return future;
        }
    };
}  // namespace Cele::Async

#endif  // __PTHREAD_POOL_H__
