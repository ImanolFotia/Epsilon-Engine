#pragma once

#include "common.hpp"
#include "assembly.hpp"
#include "delegate.hpp"

namespace dotnet
{
    class DotnetHost
    {
#ifndef MAX_PATH
        const int MAX_PATH = 256;
#endif
        using string_t = std::basic_string<char_t>;

    public:
        DotnetAssembly assembly;
        void Load(std::wstring argv)
        {

            assembly = DotnetAssembly(argv.c_str(), L"EpsilonSharp");
        }

        static void errorFunc(const char_t* msg) {
            wprintf(L"%s", msg);
        }

        DotnetHost()
        {

            if (!loadHostFxr())
            {
                assert(false && "Failure: load_hostfxr()");
                exit(255);
            }

            std::cout << "C++: hostfxr loaded...\n";

            error_writer = errorFunc;
        }

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
            init_fptr = (hostfxr_initialize_for_runtime_config_fn)getExport(lib, "hostfxr_initialize_for_runtime_config");
            get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)getExport(lib, "hostfxr_get_runtime_delegate");
            get_runtime_properties = (hostfxr_get_runtime_properties_fn)getExport(lib, "hostfxr_get_runtime_properties");
            close_fptr = (hostfxr_close_fn)getExport(lib, "hostfxr_close");
            error_writer = (hostfxr_error_writer_fn)getExport(lib, "hostfxr_set_error_writer");
            //FreeLibrary((HMODULE)lib);

            return (init_fptr && get_delegate_fptr && close_fptr && get_runtime_properties && error_writer);
        }
    };
}