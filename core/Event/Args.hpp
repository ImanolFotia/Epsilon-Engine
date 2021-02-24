#pragma once

#include "Event.hpp"

namespace Epsilon
{
    namespace Event
    {
        class EventArgs : public Event
        {
        public:
            virtual ~EventArgs() {}
        };
    }
}