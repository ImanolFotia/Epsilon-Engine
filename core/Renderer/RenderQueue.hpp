#pragma once

#include <pch.hpp>

#include "RenderCommand.hpp"

namespace Epsilon
{
    class RenderQueue
    {
        enum Order
        {
            ASCEND = 0;
            DESCEND
        };

    public : RenderQueue() = default;
        void Push(RenderCommand_ptr comm)
        {
            mQueue.push_back(comm);
        }

        void Remove(unsigned int index) const
        {
            mQueue.erase(index);
        }

        RenderCommand_ptr At(unsigned int index)
        {
            return mQueue.at(index);
        }

        RenderCommand_ptr Last()
        {
            return mQueue.back();
        }

        RenderCommand_ptr First()
        {
            return mQueue.front();
        }

        RenderCommand_ptr Next()
        {
            RenderCommand_ptr outCommand;

            mQueue.back().swap(outCommand);
            mQueue.erase(mQueue.end());

            return outCommand;
        }

        auto begin()
        {
            return mQueue.begin();
        }

        auto end()
        {
            return mQueue.end();
        }

        template <typename T>
        void OrderBy(Order order)
        {
            //...
        }

    private:
        std::vector<RenderCommand_ptr> mQueue;
    };
} // namespace Epsilon