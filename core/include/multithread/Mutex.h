#pragma once

#include <mutex>

namespace Thread
{
    namespace Mutex
    {
        class Mutex
        {
        public:

        private:
            std::mutex m_Mutex;
        };
    }
}
