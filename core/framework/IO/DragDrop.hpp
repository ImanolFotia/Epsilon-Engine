#pragma once

#include <iostream>

#if USE_GLFW
#include "GLFW/glfw3.h"
#endif
#include <beacon/beacon.hpp>

namespace framework
{
	namespace Input
	{
		class DropArgs : public beacon::args
		{
		public:
			const std::string &front()
			{
				if (!paths.empty())
					return paths[0];
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

		class DragDrop
		{
		public:
			static beacon::single_handler<DropArgs> DropEventHandler;

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
