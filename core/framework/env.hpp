#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

namespace framework
{
    enum GRAPHICAL_API
    {
        VULKAN = 0,
        OPENGL,
        DIRECT3D12,
        DIRECT3D11,
        METAL
    };
    
    struct env
    {
        static std::string getArgument(const std::string& arg) {
            //if(args.contains(arg)) return args.at(arg);
            return "";
        }

        static bool hasArgument(const std::string& arg) {
            //return args.contains(arg);

            return false;
        }
        /*
#ifdef _WIN32
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef min
#undef max
        static void setArgs(int argc_, LPSTR commandLine) {
            int argc = 0;
            LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#else*/
        static void setArgs(int argc, char** argv) { 
//#endif
        
            for (int i = 1; i < argc; i++)
            {
                std::string arg = std::string((char*)argv[i]);
                if (arg == "--gapi")
                {
                    if (i + 1 >= argc)
                        throw std::runtime_error("argument --gapi needs an argument");

                    std::string next = std::string((char*)argv[++i]);
                    if (!checkArgs(next, "vulkan", "opengl", "dx12", "dx11", "metal") ||
                        next.length() <= 0)
                    {
                        throw std::runtime_error(std::string("argument ") + next + " not recognized");
                    }
                    args["gapi"] = next;
                }
                else if (arg == "--vsync")
                {
                    args["vsync"] = "true";
                }
                else if (arg == "--scene")
                {

                    if (i + 1 >= argc)
                        throw std::runtime_error("argument --scene needs an argument");
                    std::string next = std::string((char*)argv[++i]);
                    if (next.length() <= 0)
                    {
                        throw std::runtime_error(std::string("argument ") + next + " not recognized");
                    }
                    args["scene"] = next;
                }
                else if (arg == "-h" || arg == "--help")
                {
                    std::cout << "Help me! D:" << std::endl;
                }
                else
                {
                    throw std::runtime_error(std::string("argument ") + arg + " not recognized");
                }
            }
        }

    private:
        template <class... Args>
        static bool checkArgs(const std::string& arg, Args&&... test)
        {
            //std::ranges::any_of(test..., [arg](const std::string& t) {t == arg;});
            for (auto &t : {test...})
            {
                if (t == arg)
                    return true;
            }

            return false;
        }

        inline static std::unordered_map<std::string, std::string> args;
    };
}