#pragma once
#include <string>

#include "Socket.hpp"
#include "Common.hpp"
namespace Epsilon
{
    namespace Networking
    {
        class ConnectionEntity
        {
            enum ENTITY_TYPE
            {
                CLIENT,
                SERVER,
                CLIENT_SERVER
            };

        public:
            virtual int Init() = 0;
            virtual int Bind(int) = 0;
            virtual int Send(std::string) = 0;
            virtual int Receive() = 0;
            virtual int Listen() = 0;
            virtual int Accept() = 0;
            virtual int Connect(std::string, int) = 0;
            virtual int Disconnect() = 0;
            virtual ENTITY_TYPE getType() = 0;
        };
    } // namespace Networking
} // namespace Epsilon