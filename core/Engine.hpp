#pragma once

#include <Driver/API/Context.hpp>

namespace Epsilon
{
    class Engine
    {
    public:
        static Engine &Get()
        {
            return instance;
        }

        Engine(Engine&&) = delete;
        Engine(const Engine&) = delete;

        API::CONTEXT_TYPE ContextType() {
            return mContextType;
        }

        void ContextType(API::CONTEXT_TYPE type) {
            mContextType = type;
        }

    private:
        Engine() = default;

        API::CONTEXT_TYPE mContextType;
        static Engine instance;
    };
} // namespace Epsilon