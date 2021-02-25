#pragma once

#include "Event.hpp"

namespace Epsilon
{
    namespace Event
    {
        template <typename Args>
        class Handler : public Event
        {
            EventFunctionwSendernArgs evtCallback = nullptr;

            std::unordered_map<HandlerType, const EventFunctionwSendernArgs> callbackMap;
            HandlerMapping Mapping;

            static inline uint32_t generateEventId()
            {
                static uint32_t eventIdCount = 0;
                return eventIdCount++;
            }

        public:
            Handler() = default;
            Handler(const EventFunctionwSendernArgs &callback) { evtCallback = callback; }

            Handler(EventFunctionwSendernArgs &&callback) { evtCallback = callback; }
            virtual ~Handler() {}

            template <typename EventType>
            HandlerType addListener(const EventFunctionwSendernArgs &callback)
            {
                auto handler = new auto(generateEventId());
                callbackMap.emplace(handler, callback);
                Mapping.emplace(&typeid(EventType), handler);
                return handler;
            }

            void operator=(const EventFunctionwSendernArgs &callback)
            {
                evtCallback = callback;
            }

            Handler<Args> &operator+=(const EventFunctionwSendernArgs &callback)
            {
                evtCallback = callback;
                return *this;
            }

            // void raise(Sender *sender, Args *args) { evtCallback(sender, args); }

            void raise(Sender *sender, Args *args)
            {
                auto range = Mapping.equal_range(&typeid(Args));
                for (auto it = range.first; it != range.second; ++it)
                {
                    callbackMap.at(it->second)(sender, args);
                }

                if(evtCallback != nullptr)
                    evtCallback(sender, args);
            }
        };
    }
}