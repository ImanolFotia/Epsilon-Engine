#pragma once

#include "Event.hpp"

namespace Epsilon
{
    namespace Event
    {
        template <typename Args>
        class Handler : public Event
        {
            EventFunctionwSendernArgs evtCallback;

        public:
            Handler() = default;
            Handler(const EventFunctionwSendernArgs &callback) { evtCallback = callback; }

            Handler(EventFunctionwSendernArgs &&callback) { evtCallback = callback; }
            virtual ~Handler() {}

            void operator=(const EventFunctionwSendernArgs &callback)
            {
                evtCallback = callback;
            }

            Handler<Args> &operator+=(const EventFunctionwSendernArgs &callback)
            {
                evtCallback = callback;
                return *this;
            }

            template <typename T>
            Handler<T> &operator+=(const Handler<T> &callback)
            {
                this->evtCallback = callback.evtCallback;
                return *this;
            }

            void Raise(Sender *sender, Args *args) { evtCallback(sender, args); }
        };
    }
}