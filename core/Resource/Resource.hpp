#pragma once

#include <pch.hpp>

#include "ResourceInternal.hpp"

namespace Epsilon
{
    namespace Resources
    {

        enum ResourceType
        {
            AUDIO = 0,
            IMAGE,
            VIDEO,
            MESH,
            EMPTY
        };

        class ResourceInternal
        {
        private:
            static uint32_t gResourceIndex;
            template <ResourceType T> friend class Resource;
        };

        template <ResourceType T>
        class Resource
        {
        public:
            Resource() : mType(T)
            {
                mHandle = ResourceInternal::gResourceIndex;
                ResourceInternal::gResourceIndex++;
            }

            uint32_t Handle() { return mHandle; }

            std::shared_ptr<unsigned char> Data() { return mData; }

            ResourceType Type() { return mType; }

        protected:
            std::shared_ptr<unsigned char> mData;
            uint32_t mHandle;

            ResourceType mType;
        };


    } // namespace Resources
} // namespace Epsilon