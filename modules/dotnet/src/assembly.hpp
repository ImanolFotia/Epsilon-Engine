#pragma once

#include "common.hpp"

namespace dotnet
{

    class DotnetAssembly
    {
    public:
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
        DotnetAssembly() = default;
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
            const string_t dotnet_type = m_pName + STR(".Editor, ") + m_pName;
            wprintf(dotnet_type.c_str());
            printf("\n");
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
        void LoadDelegate(string_t method_name, string_t delegate_type_name, std::wstring namespaces, bool unmanagedcallersonly = false)
        {

            const string_t dotnet_type = m_pName + L"." + namespaces + L", " + m_pName;
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
                method_name.c_str(),                                                                         // L"CustomEntryPoint" /*method_name*/
                unmanagedcallersonly ? UNMANAGEDCALLERSONLY_METHOD : (m_pName + L"." + namespaces + STR("+") + delegate_type_name + STR(", ") + m_pName).c_str(), // L"DotNetLib.Lib+CustomEntryPointDelegate, DotNetLib" /*delegate_type_name*/
                nullptr,
                (void **)&custom);

            wprintf((m_pName + L"." + namespaces + STR("+") + delegate_type_name + STR(", ") + m_pName).c_str());

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
        ReturnType Invoke(std::wstring name, Args... args)
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
}