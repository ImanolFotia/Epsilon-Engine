#pragma once

#include <pch.hpp>

namespace Epsilon {

class Clock {

    Clock& Get() {
        return instance;
    }

    const long double& Time() {
        return mCurrentTime;
    }

    const long double& Last() {
        return mLastTime;
    }

    const long double& Delta() {
        return mDeltaTime;
    }

    void Tick() {

    }

    private:

    long double mCurrentTime;
    long double mLastTime;
    long double mDeltaTime;

    Clock(){
        mCurrentTime = 0.0;
        mCurrentTime = 0.0;
        mCurrentTime = 0.0;
    }

    static Clock instance;
};

}