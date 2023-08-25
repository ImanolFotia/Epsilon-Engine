//
// Created by solaire on 2/4/23.
//

#ifndef EPSILON_DOTNET_HOST_HPP
#define EPSILON_DOTNET_HOST_HPP

// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// Standard headers
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <any>
#include <functional>

// Provided by the AppHost NuGet package and installed as an SDK pack
#include "nethost.hpp"

// Header files copied from https://github.com/dotnet/core-setup
#include "coreclr_delegates.hpp"
#include "hostfxr.hpp"

#ifdef _WIN32
#include <Windows.h>
#define DIR_SEPARATOR L'\\'
#else

#include <dlfcn.h>
#include <limits.h>
#define DIR_SEPARATOR L'/'

#endif

#define STR(s) L##s
#define CH(c) L##c

namespace dotnet
{

    namespace
    {
        hostfxr_initialize_for_runtime_config_fn init_fptr;
        hostfxr_get_runtime_delegate_fn get_delegate_fptr;
        hostfxr_close_fn close_fptr;
    }

    template <typename ReturnType, typename... Arguments>
    class DotnetDelegate
    {
    public:
        using FunctionSignature = ReturnType(CORECLR_DELEGATE_CALLTYPE *)(Arguments... args);

        FunctionSignature m_pFunctionPointer;
        std::wstring m_pName;

    public:
        DotnetDelegate(const std::wstring &name, FunctionSignature fnct_ptr) : m_pName(name), m_pFunctionPointer(fnct_ptr)
        {
        }
    };

    class DotnetAssembly
    {

        using string_t = std::basic_string<char_t>;
        string_t m_pName;
        string_t m_pPath;

        string_t dotnetlib_path;
        load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;

        std::unordered_map<std::wstring, const component_entry_point_fn> functionPointers;
        std::unordered_map<std::wstring, void *> CustomFunctionPointers;

        string_t resolveRootPath()
        {
            char_t host_path[MAX_PATH];
#if _WIN32
            auto size = ::GetFullPathNameW(m_pPath.c_str(), sizeof(host_path) / sizeof(char_t), host_path, nullptr);
            assert(size != 0);
#else
            auto resolved = realpath(argv[0], host_path);
            assert(resolved != nullptr);
#endif

            // wprintf(host_path);
            string_t root_path = host_path;
            auto pos = root_path.find_last_of(DIR_SEPARATOR);
            assert(pos != string_t::npos);
            root_path = root_path.substr(0, pos + 1);

            return root_path;
        }

        string_t resolveRuntimeConfig()
        {
            return m_pPath + m_pName + L".runtimeconfig.json";
        }

    public:
        DotnetAssembly(const string_t &assemblyPath, const string_t &assemblyName) : m_pPath(assemblyPath), m_pName(assemblyName)
        {

            m_pPath = resolveRootPath();
            const string_t config_path = resolveRuntimeConfig();

            // wprintf(config_path.c_str());
            load_assembly_and_get_function_pointer = getDotnetLoadAssembly(config_path.c_str());
            assert(load_assembly_and_get_function_pointer != nullptr && "Failure: get_dotnet_load_assembly()");
            //
            // STEP 3: Load managed assembly and get function pointer to a managed method
            //
            dotnetlib_path = m_pPath + DIR_SEPARATOR + m_pName + STR(".dll");

            // wprintf(dotnetlib_path.c_str());
        }

        void
        LoadDefaultFunction(std::wstring name)
        {
            // std::cout << (char *)dotnetlib_path.c_str() << "\n";
            const string_t dotnet_type = m_pName + STR(".Lib, ") + m_pName;
            wprintf(dotnet_type.c_str());
            const string_t dotnet_type_method = name.c_str();

            // Function pointer to managed delegate
            component_entry_point_fn hello = nullptr;
            int rc = load_assembly_and_get_function_pointer(
                dotnetlib_path.c_str(),
                dotnet_type.c_str(),
                dotnet_type_method.c_str(),
                nullptr /*delegate_type_name*/,
                nullptr,
                (void **)&hello);
            // </SnippetLoadAndGet>
            assert(rc == 0 && hello != nullptr && "Failure: load_assembly_and_get_function_pointer()");
            try
            {
                functionPointers.insert({name, std::move(hello)});
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
            }
            // functionPointers[name] = std::move(hello);
        }

        template <typename ReturnType, class... Args>
        void LoadDelegate(string_t method_name, string_t delegate_type_name)
        {

            const string_t dotnet_type = m_pName + STR(".Lib, ") + m_pName;
            // const char_t *dotnet_type_method = name.c_str();

#ifdef NET5_0
            // Function pointer to managed delegate with non-default signature
            typedef ReturnType(CORECLR_DELEGATE_CALLTYPE * custom_entry_point_fn)(Args... args);
            custom_entry_point_fn custom = nullptr;
            int rc = load_assembly_and_get_function_pointer(
                dotnetlib_path.c_str(),
                dotnet_type,
                STR("CustomEntryPointUnmanaged"), /*method_name*/

                UNMANAGEDCALLERSONLY_METHOD,
                nullptr,
                (void **)&custom);
            assert(rc == 0 && custom != nullptr && "Failure: load_assembly_and_get_function_pointer()");
#else
            // Function pointer to managed delegate with non-default signature
            typedef ReturnType(CORECLR_DELEGATE_CALLTYPE * custom_entry_point_fn)(Args... args);
            custom_entry_point_fn custom = nullptr;
            int rc = load_assembly_and_get_function_pointer(
                dotnetlib_path.c_str(),
                dotnet_type.c_str(),
                method_name.c_str(),                                                         // L"CustomEntryPoint" /*method_name*/
                (m_pName + STR(".Lib+") + delegate_type_name + STR(", ") + m_pName).c_str(), // L"DotNetLib.Lib+CustomEntryPointDelegate, DotNetLib" /*delegate_type_name*/
                nullptr,
                (void **)&custom);
            assert(rc == 0 && custom != nullptr && "Failure: load_assembly_and_get_function_pointer()");
#endif
            CustomFunctionPointers.insert({method_name, (void *)custom});
        }
        template <typename Args>
        void Invoke(std::wstring delegate, Args args)
        {
            functionPointers.at(delegate)(&args, sizeof(Args));
        }

        template <typename ReturnType, class... Args>
        ReturnType InvokeCustom(std::wstring name, Args &&...args)
        {
            typedef ReturnType(CORECLR_DELEGATE_CALLTYPE * custom_entry_point_fn)(Args... args);

            custom_entry_point_fn func_ptr = reinterpret_cast<custom_entry_point_fn>(CustomFunctionPointers.at(name));
            if (func_ptr != nullptr)
            {
                if constexpr (std::is_void<ReturnType>::value)
                {
                    func_ptr(args...);
                    return;
                }
                else
                {
                    return func_ptr(args...);
                }
            }
        }

        load_assembly_and_get_function_pointer_fn
        getDotnetLoadAssembly(const char_t *config_path)
        {
            // Load .NET Core
            void *load_assembly_and_get_function_pointer = nullptr;
            hostfxr_handle cxt = nullptr;
            int rc = init_fptr(config_path, nullptr, &cxt);
            if (rc != 0 || cxt == nullptr)
            {
                std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
                close_fptr(cxt);
                return nullptr;
            }
            // Get the load assembly function pointer
            rc = get_delegate_fptr(
                cxt,
                hdt_load_assembly_and_get_function_pointer,
                &load_assembly_and_get_function_pointer);
            if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
                std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

            close_fptr(cxt);
            return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
        }
    };

    class DotnetHost
    {
#ifndef MAX_PATH
        const int MAX_PATH = 256;
#endif
        using string_t = std::basic_string<char_t>;

    public:
        void Load(std::wstring argv)
        {
            if (!loadHostFxr())
            {
                assert(false && "Failure: load_hostfxr()");
                exit(255);
            }

            std::cout << "hostfxr loaded...\n";

            DotnetAssembly assembly(argv.c_str(), L"DotNetLib");
            assembly.LoadDefaultFunction(L"Hello");
            struct Args
            {
                const char_t *message;
                int number;
            };
            Args args{
                L"Hello from new implementation",
                42};
            assembly.Invoke(L"Hello", args);

            args = {
                L"Hello from new implementation custom delegate",
                720};
            assembly.LoadDelegate<int, Args>(L"CustomEntryPoint", L"CustomEntryPointDelegate");
            assembly.InvokeCustom<int>(L"CustomEntryPoint", args);
        }

        DotnetHost() = default;

    private:
        void *loadLibrary(const char_t *path)
        {
#ifdef _WIN32
            HMODULE h = ::LoadLibraryW(path);
            assert(h != nullptr);
            return (void *)h;
#else
            void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
            assert(h != nullptr);
            return h;
#endif
        }

#ifdef _WIN32
        FARPROC
#else
        void *
#endif
        getExport(void *h, const char *name)
        {
#ifdef _WIN32
            FARPROC f = ::GetProcAddress((HMODULE)h, name);
            assert(f != nullptr);
            return f;
#else
            void *f = dlsym(h, name);
            assert(f != nullptr);
            return f;
#endif
        }

        bool loadHostFxr()
        {
            // Pre-allocate a large buffer for the path to hostfxr
            char_t buffer[MAX_PATH];
            size_t buffer_size = sizeof(buffer) / sizeof(char_t);
            int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
            if (rc != 0)
                return false;

            // Load hostfxr and get desired exports
            void *lib = loadLibrary(buffer);
            init_fptr = (hostfxr_initialize_for_runtime_config_fn)getExport(lib,
                                                                            "hostfxr_initialize_for_runtime_config");
            get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)getExport(lib, "hostfxr_get_runtime_delegate");
            close_fptr = (hostfxr_close_fn)getExport(lib, "hostfxr_close");

            return (init_fptr && get_delegate_fptr && close_fptr);
        }
    };
}
#endif // EPSILON_DOTNET_HOST_HPP
