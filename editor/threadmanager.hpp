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

template <unsigned n>
class ThreadManager
{
public:
    ThreadManager() : _stopped(false), _max_threads(std::min(n, std::thread::hardware_concurrency())), _active_threads(0)
    {
        std::cout << "Thread manager initialized with " << _max_threads << " threads." << std::endl;

        if(_max_threads == 0){
            throw std::runtime_error("Could not initialize Thread manager with 0(zero) threads.");
            return;
        }

        for(unsigned i = 0;i < _max_threads; i++)
        {
            _threads.emplace_back(
            [this]{
                for(;;)
                {
                    if(this->_active_threads >= _max_threads){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); continue; }
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->_mgr_mutex);
                        this->_mgr_condition.wait(lock, [this]{return this->_stopped || !this->_active_task_queue.empty() || !this->_waiting_task_queue.empty();});
                        if (this->_stopped && this->_active_task_queue.empty() && this->_waiting_task_queue.empty())
                                return;
                        if(this->_active_task_queue.size() > 0){
                            task = std::move(this->_active_task_queue.front());
                            this->_active_task_queue.pop();
                        }
                        else if(this->_waiting_task_queue.size() > 0 && this->_active_task_queue.size() < _max_threads){
                            this->_active_task_queue.emplace(std::move(this->_waiting_task_queue.front()));
                            this->_waiting_task_queue.pop();
                            continue;
                        }
                    }
                    try{
                        task();
                    } catch(std::exception e)
                    {
                        std::cout << e.what() << std::endl;
                    }
                }
            });
        }
    }

    ThreadManager(ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&)= delete;

    virtual ~ThreadManager(){
        std::cout << "Deleting thread pool" << std::endl;
        {
            std::unique_lock<std::mutex> lock(_mgr_mutex);
            _stopped = true;
        }
        _mgr_condition.notify_all();
        for (std::thread &worker : _threads)
            worker.join();
    }

    /*
     * Singleton methods, this ensures that the whole program uses the same instance of this class
     */
     using  ThreadManager_ptr = std::shared_ptr<ThreadManager>;
     static ThreadManager_ptr getInstance()
     {
         if(_instance == nullptr)
            _instance = (ThreadManager_ptr) new ThreadManager<n>();

         return _instance;
     }

     unsigned ActiveTasksCount() {
         return _active_task_queue.size();
     }

    /*
     * Tasks are pushed into a queue, if there's an available thread it will immediatelly pick up and execute the task
     * if not, it will remain in the queue until a thread becomes available
     */
    template <class T, class... Args>
    auto PushTask(T &&t, Args&&... args) -> std::future<typename std::result_of<T(Args...)>::type>
    {
        if(!_stopped)
        {
            using ret_type = typename std::result_of<T(Args...)>::type;
            auto task = std::make_shared<std::packaged_task<ret_type()>>(
                std::bind(std::forward<T>(t), std::forward<Args>(args)...));
            auto res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(_mgr_mutex);

                if (_stopped)
                    throw std::runtime_error("Cannot push new Task, thread manager is stopped");
                if(_active_task_queue.size() >= _max_threads)
                {
                    _waiting_task_queue.emplace([task]() { (*task)(); });
                }
                else
                {
                    _active_task_queue.emplace([task]() { (*task)(); });
                }
            }

            _mgr_condition.notify_one();
            return res;
        }
    }

    void Stop() {_stopped = true;}

    std::atomic<int> loadedLastFrame;
private:
    int _max_threads = 0;

    bool _stopped;
    std::mutex _mgr_mutex;
    std::condition_variable _mgr_condition;
    std::queue<std::function<void()> > _active_task_queue;
    std::queue<std::function<void()> > _waiting_task_queue;
    std::atomic<unsigned> _active_threads;
    std::vector<std::thread> _threads;
    static ThreadManager_ptr _instance;
};
