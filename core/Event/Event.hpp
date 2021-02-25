#pragma once

#include <pch.hpp>

namespace Epsilon
{
    namespace Event
    {
        class EventArgs;
        class Sender;
        class Event;

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

        template <typename F, typename T>
        static EventFunctionwSendernArgs bind(F const & fn, T* o) {
            return std::bind(fn, o, std::placeholders::_1, std::placeholders::_2);
        }

    }
}