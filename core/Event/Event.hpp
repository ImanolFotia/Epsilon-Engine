#pragma once

#include <pch.hpp>

namespace Epsilon
{
    namespace Event
    {
        class EventArgs;
        class Sender;

        // using Sender = void *;
        using EventFunction = std::function<void(const Event &)>;
        using EventFunctionwSender = std::function<void(Sender, const Event &)>;
        using EventFunctionwSendernArgs = std::function<void(Sender *, EventArgs *)>;
        using HandlerType = uint32_t *;
        using HandlerMapping = std::multimap<const std::type_info *, HandlerType>;
        using CallbackMap = std::unordered_map<HandlerType, const EventFunction>;

        class Event
        {
        public:
            virtual ~Event() {}

            template <typename EventType>
            EventType to() const
            {
                return static_cast<const EventType &>(*this);
            }
        };

    }
}