#pragma once

#include <Thread.h>

namespace Thread {

    template<typename T>
    class Job
    {
        Job(std::function<T> f): mFunction(f);
        ~Job(){}
    public:
        int Begin(){
            mFunction();
        }
        int Finish(){}



    private:
        std::shared_ptr<Thread> mThread;
        std::function<T> mFunction;
        T functionType;

    };
}
