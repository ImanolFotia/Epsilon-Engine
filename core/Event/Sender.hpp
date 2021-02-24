#pragma once

#include "Event.hpp"

namespace Epsilon
{
    namespace Event
    {

        class Sender
        {
        public:
            virtual ~Sender() {}

            template <typename EventType>
            EventType to() const
            {
                return static_cast<const EventType &>(*this);
            }
        };
    }
}