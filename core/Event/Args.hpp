#pragma once

#include "Event.hpp"
#include "Handler.hpp"

namespace Epsilon
{
    namespace Event
    {
        class EventArgs : public Event
        {
        public:
            virtual ~EventArgs() {}

        protected:
            HandlerType indentifier;

            friend class Handler<EventArgs>;
        };
    }
}