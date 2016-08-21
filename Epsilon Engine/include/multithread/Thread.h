#pragma once

#include <thread>
#include <memory>

namespace Thread {

    class Thread {
    public:
        Thread() = default;
        ~Thread() = default;

    public:
        bool JoinThread() {
            this->m_Thread->join()
        }

        bool DetachThread() {}

        bool isJoinable() {
            this->m_Thread->joinable()
        }

        std::thread::id getThreadID() {
            return this->m_ThreadID;
        }

    private:

        std::shared_ptr<std::thread> m_Thread;
        std::thread::id m_ThreadID;

    };
}


