#include "pthread_pool.h"
using namespace Cele::Async;

void* PThreadPool::ThreadEntry(void* args)
{
    auto* Pool = static_cast<PThreadPool*>(args);
    Pool->Run();
    return nullptr;
}

PThreadPool::PThreadPool(unsigned int ThreadNum) : Stop(false), ActiveTasks(0)
{
    pthread_mutex_init(&QueueMutex, nullptr);
    pthread_cond_init(&CondVar, nullptr);
    pthread_cond_init(&FinishedVar, nullptr);

    Workers.resize(ThreadNum);
    for (auto& Worker : Workers) pthread_create(&Worker, nullptr, ThreadEntry, this);
}

PThreadPool::~PThreadPool()
{
    pthread_mutex_lock(&QueueMutex);
    Stop = true;
    pthread_mutex_unlock(&QueueMutex);

    pthread_cond_broadcast(&CondVar);
    for (auto& Worker : Workers) pthread_join(Worker, nullptr);

    pthread_mutex_destroy(&QueueMutex);
    pthread_cond_destroy(&CondVar);
    pthread_cond_destroy(&FinishedVar);
}

void PThreadPool::Run()
{
    while (true)
    {
        ITask* task = nullptr;
        pthread_mutex_lock(&QueueMutex);

        while (!Stop && Tasks.empty()) pthread_cond_wait(&CondVar, &QueueMutex);

        if (Stop && Tasks.empty())
        {
            pthread_mutex_unlock(&QueueMutex);
            break;
        }

        task = Tasks.front();
        Tasks.pop();
        ++ActiveTasks;
        pthread_mutex_unlock(&QueueMutex);

        task->execute();
        delete task;

        pthread_mutex_lock(&QueueMutex);
        --ActiveTasks;
        if (ActiveTasks == 0 && Tasks.empty()) pthread_cond_broadcast(&FinishedVar);
        pthread_mutex_unlock(&QueueMutex);
    }
}

void PThreadPool::Sync()
{
    pthread_mutex_lock(&QueueMutex);
    while (!Tasks.empty() || ActiveTasks > 0) pthread_cond_wait(&FinishedVar, &QueueMutex);
    pthread_mutex_unlock(&QueueMutex);
}
