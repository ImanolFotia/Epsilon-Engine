
#if !defined(ANDROID) && !defined(__ANDROID__) && DMONO_ENABLED == true
#include <mono/jit/jit.h>
#include <mono/metadata/environment.h>
#include <mono/utils/mono-publib.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <stdlib.h>

#include <iostream>
#include <list>

/* Assembly compiling info:
 * For compiling a module: mcs -out:<name>.dll -target:module <name>.cs
 * For compiling an executable embedding a module: mcs -out:<name>.dll -addmodule:<module_name>.dll <name>.cs
 * */
namespace modules::mono
{
    class Method
    {
    public:
        MonoMethodDesc *m_pMethodDesc;
        MonoMethod *m_pMethod;
    };

    class Domain
    {
    public:
        std::unordered_map<std::string, Method> m_pMethods;

        MonoDomain *m_pDomain;
        MonoAssembly *m_pAssembly;
        MonoImage *m_pImage;

        int32_t m_pExitCode;
        std::string m_pFile;
        Domain(const std::string &name, const std::string &filename)
        {
            m_pFile = filename;
            m_pDomain = mono_jit_init(filename.c_str());
            m_pAssembly = mono_domain_assembly_open(m_pDomain, m_pFile.c_str());
            if (!m_pAssembly)
                exit(2);
            m_pImage = mono_assembly_get_image(m_pAssembly);
        }

        void Execute(const std::string &method)
        {
        }

        // Executes from the main function
        void Execute()
        {
            char *argv[] = {
                (char *)m_pFile.data(),
                NULL};
            mono_jit_exec(m_pDomain, m_pAssembly, 1, argv);
            int retval = mono_environment_exitcode_get();
            mono_jit_cleanup(m_pDomain);
        }

        void Destroy()
        {
            m_pExitCode = mono_environment_exitcode_get();
            mono_jit_cleanup(m_pDomain);
        }
    };
    class MonoInstance : public singleton<MonoInstance>
    {

        static int malloc_count;
        MonoAllocatorVTable mem_vtable;
        std::list<Domain> m_pAssemblies;

        static void *custom_malloc(size_t bytes)
        {
            return malloc(bytes);
        }

    public:
        MonoInstance()
        {
            mem_vtable = {MONO_ALLOCATOR_VTABLE_VERSION, custom_malloc, NULL, NULL, NULL};
            mono_set_allocator_vtable(&mem_vtable);
            mono_config_parse(NULL);
        }

        const Domain &addDomain(const std::string &name, const std::string &filename)
        {
            m_pAssemblies.emplace_back(std::forward<const std::string &>(name), std::forward<const std::string &>(filename));

            return m_pAssemblies.back();
        }

        template <class Caller>
        void mapMethod(std::string &name, Domain &domain, Caller *caller)
        {

            domain.m_pMethods[name] = {};
            domain.m_pMethods[name].m_pMethodDesc = mono_method_desc_new(name.c_str(), true);

            domain.m_pMethods[name].m_pMethod = mono_method_desc_search_in_image(domain.m_pMethods[name].m_pMethodDesc, domain.m_pImage);

            uint64_t ptr = reinterpret_cast<uint64_t>(caller);
            void *args[1];
            args[0] = &ptr;

            // Invoke C# code
            mono_runtime_invoke(domain.m_pMethods[name].m_pMethod, nullptr, args, nullptr);
        }
        void mapInternalCall(const std::string &name, const void *func)
        {
            mono_add_internal_call(name.c_str(), func);
        }
    };
}
#endif