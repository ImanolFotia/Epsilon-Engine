#pragma once

#include <iostream>

#if USE_GLFW
#include "GLFW/glfw3.h"
#endif
#include <beacon/beacon.hpp>
#include <core/framework/window.hpp>

#if defined(_WIN32)
//  Microsoft
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __declspec(dllexport)
#else
#define EPSILON_DLL __declspec(dllimport)
#endif
#elif defined(__linux__)
//  GCC
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __attribute__((visibility("default")))
#else
#define EPSILON_DLL
#endif
#else
//  do nothing and hope for the best?
#define EPSILON_DLL
#pragma warning Unknown dynamic link import / export semantics.
#endif

namespace framework
{
	namespace Input
	{
		class DropArgs : public beacon::args
		{
		public:
			std::string front()
			{
				if (!paths.empty())
					return paths[0];

				return "";
			}

			const std::string &at(unsigned index)
			{
				return paths.at(index);
			}

			auto size()
			{
				return paths.size();
			}

			auto push_back(const std::string &str)
			{
				paths.push_back(str);
			}

			const std::vector<std::string> &data()
			{
				return paths;
			}

		private:
			std::vector<std::string> paths;
		};

		class EPSILON_DLL DragDrop
		{
		public:
			static inline beacon::single_handler<DropArgs> DropEventHandler;

#if USE_GLFW
			static void DropCallbackGLFW(Window::windowType *window, int count, const char **paths)
			{
				DropArgs dropArgs;
				for (unsigned i = 0; i < count; i++)
				{
					dropArgs.push_back(paths[i]);
				}

				DropEventHandler.raise(nullptr, &dropArgs);
			}
#endif
		};
	}
}
