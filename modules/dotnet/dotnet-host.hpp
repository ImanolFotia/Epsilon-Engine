//
// Created by solaire on 2/4/23.
//

#ifndef EPSILON_DOTNET_HOST_HPP
#define EPSILON_DOTNET_HOST_HPP

#include <core/framework/singleton.hpp>

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

// Provided by the AppHost NuGet package and installed as an SDK pack
#include "nethost.hpp"

// Header files copied from https://github.com/dotnet/core-setup
#include "coreclr_delegates.hpp"
#include "hostfxr.hpp"

#ifdef WINDOWS
#include <Windows.h>
#else

#include <dlfcn.h>
#include <limits.h>

#endif

namespace dotnet {

    namespace {
        hostfxr_initialize_for_runtime_config_fn init_fptr;
        hostfxr_get_runtime_delegate_fn get_delegate_fptr;
        hostfxr_close_fn close_fptr;
    }
    class DotnetDelegate {
        component_entry_point_fn m_pFunctionPointer;
    public:
        DotnetDelegate(const char* name, component_entry_point_fn fnct_ptr) {

        }
    };

    class DotnetAssembly {

        using string_t = std::basic_string<char_t>;

    public:
        DotnetAssembly(const std::string &assemblyPath, const std::string &assemblyName) {

            const string_t config_path = assemblyPath + "/" + assemblyName + ".runtimeconfig.json";
            load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;
            load_assembly_and_get_function_pointer = getDotnetLoadAssembly(config_path.c_str());
            assert(load_assembly_and_get_function_pointer != nullptr && "Failure: get_dotnet_load_assembly()");
            //
            // STEP 3: Load managed assembly and get function pointer to a managed method
            //
            const string_t dotnetlib_path = assemblyPath + assemblyName;
            const char_t *dotnet_type = "DotNetLib.Lib, DotNetLib";
            const char_t *dotnet_type_method = "Hello";


            // Function pointer to managed delegate
            component_entry_point_fn hello = nullptr;
            int rc = load_assembly_and_get_function_pointer(
                    dotnetlib_path.c_str(),
                    dotnet_type,
                    dotnet_type_method,
                    nullptr /*delegate_type_name*/,
                    nullptr,
                    (void **) &hello);
            // </SnippetLoadAndGet>
            assert(rc == 0 && hello != nullptr && "Failure: load_assembly_and_get_function_pointer()");

            //
            // STEP 4: Run managed code
            //
            struct lib_args {
                const char_t *message;
                int number;
            };
            for (int i = 0; i < 3; ++i) {
                // <SnippetCallManaged>
                lib_args args
                        {
                                "from host!",
                                i
                        };

                hello(&args, sizeof(args));
                // </SnippetCallManaged>
            }


#ifdef NET5_0
            // Function pointer to managed delegate with non-default signature
        typedef void (CORECLR_DELEGATE_CALLTYPE *custom_entry_point_fn)(lib_args args);
        custom_entry_point_fn custom = nullptr;
        rc = load_assembly_and_get_function_pointer(
            dotnetlib_path.c_str(),
            dotnet_type,
            STR("CustomEntryPointUnmanaged") /*method_name*/,
            UNMANAGEDCALLERSONLY_METHOD,
            nullptr,
            (void**)&custom);
        assert(rc == 0 && custom != nullptr && "Failure: load_assembly_and_get_function_pointer()");
#else
            // Function pointer to managed delegate with non-default signature
            typedef void (CORECLR_DELEGATE_CALLTYPE *custom_entry_point_fn)(lib_args args);
            custom_entry_point_fn custom = nullptr;
            rc = load_assembly_and_get_function_pointer(
                    dotnetlib_path.c_str(),
                    dotnet_type,
                    "CustomEntryPoint" /*method_name*/,
                    "DotNetLib.Lib+CustomEntryPointDelegate, DotNetLib" /*delegate_type_name*/,
                    nullptr,
                    (void **) &custom);
            assert(rc == 0 && custom != nullptr && "Failure: load_assembly_and_get_function_pointer()");
#endif

            lib_args args
                    {
                            "from host!",
                            -1
                    };
            custom(args);


        }

        load_assembly_and_get_function_pointer_fn getDotnetLoadAssembly(const char *config_path) {
// Load .NET Core
            void *load_assembly_and_get_function_pointer = nullptr;
            hostfxr_handle cxt = nullptr;
            int rc = init_fptr(config_path, nullptr, &cxt);
            if (rc != 0 || cxt == nullptr) {
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
            return (load_assembly_and_get_function_pointer_fn) load_assembly_and_get_function_pointer;

        }
    };

    class DotnetHost : public singleton<DotnetHost> {
        const int MAX_PATH = 256;
        using string_t = std::basic_string<char_t>;

    public:

        void Load() {
            if (!self.loadHostFxr()) {
                assert(false && "Failure: load_hostfxr()");
                exit(255);
            }
        }

        DotnetHost() = default;

    private:
        void *loadLibrary(const char_t *path) {
#ifdef _WIN32
            HMODULE h = ::LoadLibraryW(path);
            assert(h != nullptr);
            return (void*)h;
#else
            void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
            assert(h != nullptr);
            return h;
#endif
        }

        void *getExport(void *h, const char *name) {
#ifdef _WIN32
            void *f = ::GetProcAddress((HMODULE)h, name);
            assert(f != nullptr);
            return f;
#else
            void *f = dlsym(h, name);
            assert(f != nullptr);
            return f;
#endif
        }

        bool loadHostFxr() {
            // Pre-allocate a large buffer for the path to hostfxr
            char_t buffer[MAX_PATH];
            size_t buffer_size = sizeof(buffer) / sizeof(char_t);
            int rc = get_hostfxr_path(buffer, &buffer_size, nullptr);
            if (rc != 0)
                return false;

            // Load hostfxr and get desired exports
            void *lib = loadLibrary(buffer);
            init_fptr = (hostfxr_initialize_for_runtime_config_fn) getExport(lib,
                                                                             "hostfxr_initialize_for_runtime_config");
            get_delegate_fptr = (hostfxr_get_runtime_delegate_fn) getExport(lib, "hostfxr_get_runtime_delegate");
            close_fptr = (hostfxr_close_fn) getExport(lib, "hostfxr_close");

            return (init_fptr && get_delegate_fptr && close_fptr);
        }


    };
}
#endif //EPSILON_DOTNET_HOST_HPP
