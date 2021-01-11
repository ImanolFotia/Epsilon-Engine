#pragma once

namespace Epsilon
{
    namespace API
    {
        class ShaderStorage
        {
        protected:
            unsigned int mHandle;
            size_t mSize = 0;
            size_t mOffset = 0;
            unsigned int mBindingPoint = 0;
            unsigned int mUsage = 0;

        public:
            //ShaderStorage(size_t size, unsigned int binding_point, size_t offset, unsigned int usage) {}
            ShaderStorage() = default;
            ShaderStorage(const ShaderStorage &&) = delete;
            ShaderStorage(ShaderStorage&&) = delete;

            virtual void Init(const void *data) = 0;
            virtual void Update(size_t size, unsigned int offset, const void *data) = 0;
            
            virtual void Get(void *data) = 0;
            virtual void GetSubData(size_t size, unsigned int offset, void *data) = 0;

            virtual void Bind() = 0;
            virtual void Unbind() = 0;
        };
    } // namespace API
} // namespace Epsilon