#pragma once

#include <pch.hpp>

namespace Epsilon
{
    namespace API
    {
        class BufferObject
        {
        protected:
            unsigned int mHandle = 0;
            size_t mSize = 0;
            size_t mOffset;
            unsigned int mUsage;
        public:
            BufferObject() = default;

            virtual void Init(const void *data) = 0;
            virtual void Update(size_t size, unsigned int offset, const void *data) = 0;
            virtual void Bind() = 0;
            virtual void Unbind() = 0;
        };
    } // namespace API
} // namespace Epsilon