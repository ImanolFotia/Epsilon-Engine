#pragma once

#include <exception>
#include <iostream>
#include <unistd.h>

#define PORT 80
#define SPORT 443

#define DEFAULT_MASK 255.255.255.0

#if __linux__
#include <sys/socket.h> //socket
#include <arpa/inet.h>	//inet_addr
#include <netdb.h>		//hostent
#include <sys/ioctl.h>
#elif _WIN32
#include <winsock2.h>
<<<<<<< HEAD
=======
#include <windows.h>
>>>>>>> f87825e45cd4ab71b09c8c50f30bd763892779ed
#include <ws2tcpip.h>
#include <iphlpapi.h>
#define ioctl ioctlsocket
#endif

namespace Epsilon
{
	namespace Networking
	{
		class Socket
		{
		public:
			Socket()
			{
#ifdef _WIN32
				WSADATA wsaData;
				// Initialize Winsock
				int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
				if (iResult != 0)
				{
					std::cout << "WSAStartup failed with error: " << iResult << std::endl;
				}
#if DEBUG
				std::cout << "WSA Started succesfully" << std::endl;
#endif
#endif
				create();
			}

			bool Init(std::string ip, int port)
			{
				this->mAddress = ip;
				this->mPort = port;
				this->create();
				this->bind(port);
				this->connect(mAddress, port);
				return true;
			}

			bool create()
			{
				//Create socket
				mSock = socket(AF_INET, SOCK_STREAM, 0);
				if (mSock == -1)
				{
					std::cout << "Could not create socket" << std::endl;
#ifdef _WIN32
					WSACleanup();
#endif
					return false;
				}

				return true;
			}

			bool bind(int port)
			{
				this->mPort = port;
				mAddr.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY;
				mAddr.sin_family = AF_INET;
				mAddr.sin_port = htons(this->mPort);

				//bind Socket
				int bRes = ::bind(mSock, (struct sockaddr *)&mAddr, sizeof(mAddr));
				if (bRes == -1)
				{
					std::cout << "Failed to bind socket" << std::endl;
#ifdef _WIN32
					WSACleanup();
#endif
					return false;
				}

				return true;
			}

			bool connect(std::string addr, int port)
			{
				this->mPort = port;
				//Connect to remote server
				std::cout << "Attempting to connect to: " << addr << std::endl;
				struct hostent *he;
				if ((he = gethostbyname(addr.c_str())) == NULL)
				{
					std::cout << "Socket::connect(): Failed to convert " << addr << " to an IP" << std::endl;
					return false;
				}

				mAddr.sin_addr.s_addr = INADDR_ANY;
				mAddr.sin_family = AF_INET;
				mAddr.sin_port = htons(this->mPort);
				memcpy(&mAddr.sin_addr, he->h_addr_list[0], he->h_length);
				int status = ::connect(mSock, (sockaddr *)&mAddr, sizeof(mAddr));
				if (status != 0)
				{
					std::cout << "Error connecting to " << addr << std::endl;
					return false;
				}
				return true;
			}

			bool send(std::string message)
			{
				int sRet = ::send(mSock, message.c_str(), strlen(message.c_str()), 0);
				if (sRet == -1)
				{
#ifdef _WIN32
					std::cout << "Socket::send(): Failed to send message: " << WSAGetLastError() << std::endl;
#endif
					return false;
				}
				return true;
			}

			ssize_t receive(char *outBuff)
			{
				ssize_t bytesRead;
				bytesRead = ::recv(mSock, (char *)outBuff, 1000, 0);
				return bytesRead;
			}

			bool listen()
			{
				int lRet = ::listen(mSock, 10);
				if (lRet < 0)
				{
#ifdef _WIN32
					std::cout << "Socket::listen(): Failed with error " << WSAGetLastError() << std::endl;
#endif
					return false;
				}
				return true;
			}

			bool lock()
			{
				unsigned long int block = true;
				int opt = ::ioctl(mSock, FIONBIO, &block);
				if (opt)
				{
#ifdef _WIN32
					std::cout << "Socket::setNonBlocking(): Failed with error " << WSAGetLastError() << std::endl;
#endif
					return false;
				}
				return true;
			}

			bool accept()
			{
				size_t c = sizeof(struct sockaddr_in);
				mSock = ::accept(mSock, (struct sockaddr *)&mAddr, (socklen_t *)&c);

				std::cout << "Incoming connection..." << std::endl;
				std::cout << mAddr.sin_family << std::endl;
				std::cout << mAddr.sin_port << std::endl;
				std::flush(std::cout);
				if (mSock < 0)
				{
#ifdef _WIN32
					std::cout << "Socket::accept(): Failed with error " << WSAGetLastError() << std::endl;
#endif
					return 1;
				}
			}

			bool shutdown()
			{
				int ret = ::shutdown(this->mSock, 2);
				if (ret < 0)
				{
#ifdef _WIN32
					std::cout << "Socket::accept(): Failed with error " << WSAGetLastError() << std::endl;
#endif
					return false;
				}
#ifdef _WIN32
				WSACleanup();
#endif
				return true;
			}

			int mSock = 0;
			std::string mAddress;
			int mPort = 27015;

		private:
			struct sockaddr_in mAddr;
		};
	} // namespace Networking
} // namespace Epsilon