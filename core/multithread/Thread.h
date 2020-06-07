#pragma once

#include <thread>
#include <memory>
#include <functional>

namespace Threading {

    class Thread {
    public:
        Thread() = default;
        ~Thread() = default;

    public:
        bool JoinThread() {
            if(this->isJoinable())
                this->m_Thread->join()
        }

        bool DetachThread() {
            mThread->detach();
        }

        bool isJoinable() {
            this->m_Thread->joinable()
        }

        std::thread::id getThreadID() {
            return this->m_ThreadID;
        }

        static uint32_t getConcurrency(){ return std::thread::hardware_concurrency();}

    private:

        std::shared_ptr<std::thread> m_Thread;
        std::thread::id m_ThreadID;

    };
}


