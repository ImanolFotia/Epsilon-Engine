#pragma once

#include <thread>
#include <queue>
#include <functional>
#include <future>
#include <map>
#include <algorithm> // for std::min
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <cstdio>
#include <chrono>
#include <future>

template <unsigned n>
class ThreadManager
{
public:
    ThreadManager() : m_pStopped(false), m_pMax_threads(std::min(n, std::thread::hardware_concurrency())), m_pActive_threads(0)
    {
        std::cout << "Thread manager initialized with " << m_pMax_threads << " threads." << std::endl;

        if (m_pMax_threads == 0) {
            throw std::runtime_error("Could not initialize Thread manager with 0(zero) threads.");
            return;
        }

        for (unsigned i = 0; i < m_pMax_threads; i++)
        {
            m_pAsync_threads.emplace_back(
                std::async(std::launch::async, [this] {
                    for (;;)
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->m_pMgr_mutex);
                            this->m_pMgr_condition.wait(lock, [this] {return this->m_pStopped || !this->m_pActive_task_queue.empty() || !this->m_pWaiting_task_queue.empty(); });
                            if (this->m_pStopped && this->m_pActive_task_queue.empty() && this->m_pWaiting_task_queue.empty())
                                return;
                            if (this->m_pActive_task_queue.size() > 0) {
                                task = std::move(this->m_pActive_task_queue.front());
                                this->m_pActive_task_queue.pop();
                            }
                            else if (this->m_pWaiting_task_queue.size() > 0) {
                                this->m_pActive_task_queue.emplace(std::move(this->m_pWaiting_task_queue.front()));
                                this->m_pWaiting_task_queue.pop();
                                continue;
                            }
                        }
                        try {
                            task();
                        }
                        catch (std::exception& e)
                        {
                            std::cout << e.what() << std::endl;
                        }
                    }
                    }));
        }
    }

    ThreadManager(ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

    virtual ~ThreadManager() {
        std::cout << "Deleting thread pool" << std::endl;
        {
            std::unique_lock<std::mutex> lock(m_pMgr_mutex);
            m_pStopped = true;
        }
        m_pMgr_condition.notify_all();

        for (auto& f : m_pAsync_threads)
            f.get();
    }

    /*
     * Singleton methods, this ensures that the whole program uses the same instance of this class
     */
    using  ThreadManager_ptr = std::shared_ptr<ThreadManager>;

    /*
     * Tasks are pushed into a queue, if there's an available thread it will immediatelly pick up and execute the task
     * if not, it will remain in the queue until a thread becomes available
     */
    template <class T, class... Args>
    auto PushTask(T&& t, Args&&... args) /*-> std::future<typename std::result_of<T(Args...)>::type>*/ //Required when using C++11
    {
        if (!m_pStopped)
        {
            using ret_type = typename std::invoke_result<T, Args...>::type;
            auto task = std::make_shared<std::packaged_task<ret_type()>>(
                std::bind(std::forward<T>(t), std::forward<Args>(args)...));
            decltype(auto) res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_pMgr_mutex);

                if (m_pStopped)
                    throw std::runtime_error("Cannot push new Task, thread manager is stopped");
                if (m_pActive_threads >= m_pMax_threads)
                {
                    m_pWaiting_task_queue.emplace([task]() { (*task)(); });
                }
                else
                {
                    m_pActive_task_queue.emplace([task]() { (*task)(); });
                    m_pActive_threads++;
                }
            }

            m_pMgr_condition.notify_one();
            return res;
        }
        else
        {
            throw std::runtime_error("Cannot push new Task, thread manager is stopped");
        }
    }

    constexpr auto Stop() { m_pStopped = true; }

    void Wait()
    {
        for (auto& thread : m_pAsync_threads)
        {
            thread->wait();
        }
    }

private:
    bool  m_pStopped;
    unsigned  m_pMax_threads = 0;

    std::mutex m_pMgr_mutex;
    std::condition_variable m_pMgr_condition;
    std::queue<std::function<void()> > m_pActive_task_queue;
    std::queue<std::function<void()> > m_pWaiting_task_queue;
    std::atomic<unsigned> m_pActive_threads;
    std::vector<std::shared_future<void>> m_pAsync_threads;
};
