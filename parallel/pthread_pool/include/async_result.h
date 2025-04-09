#ifndef __ASYNC_RESULT_H__
#define __ASYNC_RESULT_H__

#include <atomic>
#include <pthread.h>

namespace Cele::Async
{
    class StateBase
    {
      protected:
        std::atomic<int> ref_count{1};

      public:
        virtual ~StateBase() = default;

        void inc_ref() { ++ref_count; }
        void dec_ref()
        {
            if (--ref_count == 0) delete this;
        }
    };

    template <typename T>
    class SharedState : public StateBase
    {
        T               value;
        bool            ready = false;
        pthread_mutex_t mutex;
        pthread_cond_t  cond;

      public:
        SharedState()
        {
            pthread_mutex_init(&mutex, nullptr);
            pthread_cond_init(&cond, nullptr);
        }

        ~SharedState()
        {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&cond);
        }

        void set_value(T&& val)
        {
            pthread_mutex_lock(&mutex);
            value = std::move(val);
            ready = true;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond);
        }

        T get()
        {
            pthread_mutex_lock(&mutex);
            while (!ready) { pthread_cond_wait(&cond, &mutex); }
            T result = value;
            pthread_mutex_unlock(&mutex);
            return result;
        }
    };

    template <>
    class SharedState<void> : public StateBase
    {
        bool            ready = false;
        pthread_mutex_t mutex;
        pthread_cond_t  cond;

      public:
        SharedState()
        {
            pthread_mutex_init(&mutex, nullptr);
            pthread_cond_init(&cond, nullptr);
        }

        ~SharedState()
        {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&cond);
        }

        void set_value()
        {
            pthread_mutex_lock(&mutex);
            ready = true;
            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond);
        }

        void get()
        {
            pthread_mutex_lock(&mutex);
            while (!ready) { pthread_cond_wait(&cond, &mutex); }
            pthread_mutex_unlock(&mutex);
        }
    };

    template <typename T>
    class Result
    {
        SharedState<T>* state;

      public:
        Result() : state(new SharedState<T>()) {}
        Result(Result&& other) noexcept : state(other.state) { other.state = nullptr; }
        Result& operator=(Result&& other) noexcept
        {
            if (this != &other)
            {
                if (state) state->dec_ref();
                state       = other.state;
                other.state = nullptr;
            }
            return *this;
        }
        Result(const Result&)            = delete;
        Result& operator=(const Result&) = delete;
        ~Result()
        {
            if (state) state->dec_ref();
        }

      public:
        T               get() { return state->get(); }
        SharedState<T>* get_state()
        {
            state->inc_ref();
            return state;
        }
    };

    template <>
    class Result<void>
    {
        SharedState<void>* state;

      public:
        Result() : state(new SharedState<void>()) {}
        Result(Result&& other) noexcept : state(other.state) { other.state = nullptr; }
        Result& operator=(Result&& other) noexcept
        {
            if (this != &other)
            {
                if (state) state->dec_ref();
                state       = other.state;
                other.state = nullptr;
            }
            return *this;
        }
        Result(const Result&)            = delete;
        Result& operator=(const Result&) = delete;
        ~Result()
        {
            if (state) state->dec_ref();
        }

      public:
        void               get() { state->get(); }
        SharedState<void>* get_state()
        {
            state->inc_ref();
            return state;
        }
    };
}  // namespace Cele::Async

#endif  // __ASYNC_RESULT_H__
