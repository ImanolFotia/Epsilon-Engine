#pragma once
#include <thread>
#include <future>
#include <atomic>
#include <memory>
#include <iostream>
#include <exception>
#include <queue>
#include <thread>
#include <mutex>
#include <random>
#include <vector>
#include <functional>
#include <stdexcept>
#include <condition_variable>
#include <type_traits>

class ThreadPool
{
  public:
    virtual ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(pool_mutex);
            stopped = true;
        }
        condition.notify_all();
        for (std::thread &worker : _threads)
            worker.join();
    }

    using ThreadPool_ptr = std::shared_ptr<ThreadPool>;

    static ThreadPool_ptr getInstance_shared_ptr()
    {
        if (_instance == nullptr)
            _instance = (ThreadPool_ptr) new ThreadPool();

        return _instance;
    }
    static ThreadPool* getInstance()
    {
        if (_instance == nullptr)
            _instance = (ThreadPool_ptr) new ThreadPool();

        return _instance.get();
    }

    template <class T, class... Args>
    auto EmplaceTask(T &&t, Args &&... args)
        -> std::future<typename std::result_of<T(Args...)>::type>
    {
        if (_tasks_queue.size() >= _capacity)
        {
            //put in another queue
            std::cout << "Queue is complete" << std::endl;
        }

        using ret_type = typename std::result_of<T(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<ret_type()>>(
            std::bind(std::forward<T>(t), std::forward<Args>(args)...));

        auto res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(pool_mutex);

            if (stopped)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            _tasks_queue.emplace([task]() { (*task)(); });
            _current_active_threads++;
        }
        condition.notify_one();

        return res;
    }

    void Update()
    {
        /*
        for (auto &t : _threads)
        {
        }
        */
    }

  private:
    ThreadPool() : stopped(false), _current_active_threads(0)
    {
        _capacity = std::thread::hardware_concurrency();
        std::cout << "Initializing thread pool with: " << _capacity << " threads" << std::endl;

        for (size_t i = 0; i < _capacity; ++i)
            _threads.emplace_back(
                [this] {
                    for (;;)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->pool_mutex);
                            this->condition.wait(lock,
                                                 [this] { return this->stopped || !this->_tasks_queue.empty(); });
                            if (this->stopped && this->_tasks_queue.empty())
                                return;
                            task = std::move(this->_tasks_queue.front());
                            this->_tasks_queue.pop();
                        }

                        task();
                    }
                });
    }

    ThreadPool(const ThreadPool&)= delete;
    ThreadPool& operator=(const ThreadPool&)= delete;

    std::atomic<bool> stopped;
    std::mutex pool_mutex;
    uint16_t _capacity;
    static ThreadPool_ptr _instance;
    std::atomic<int> _current_active_threads;
    std::condition_variable condition;
    std::vector<std::thread> _threads;
    std::queue<std::function<void()>> _tasks_queue;
};
