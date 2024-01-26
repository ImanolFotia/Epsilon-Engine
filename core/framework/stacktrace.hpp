#pragma once

#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include <core/framework/containers/static_array.hpp>

#ifdef __linux__
#include <zconf.h>
#include <execinfo.h>
#include <cerrno>
#include <unistd.h>
#include <linux/limits.h>
#endif

#ifdef _WIN32

#include <process.h>
#include <iostream>
#include <Windows.h>
#include "dbghelp.h"

#define PATH_MAX 256 // per spec
#endif

namespace framework
{

    static std::string get_path()
    {
#ifdef __linux__
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

        auto str = std::string(result, (count > 0) ? count : 0);
        std::cout << str;
        return "";
#endif

#ifdef _WIN32
        char result[PATH_MAX];
        size_t count = GetModuleFileName(NULL, result, PATH_MAX); // readlink("/proc/self/exe", result, PATH_MAX);

        auto str = std::string(result, (count > 0) ? count : 0);
        std::cout << str;
        return "";
#endif
    }

    static std::string sh(const std::string &cmd)
    {
#ifdef __linux__
        std::array<char, 128> buffer = {0};
        std::string result;
        std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe)
            throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get()))
        {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            {
                result += buffer.data();
            }
        }

        return result;
#endif

#ifdef _WIN32
        std::array<char, 128> buffer = {0};
        std::string result;
        std::shared_ptr<FILE> pipe(_popen(cmd.c_str(), "r"), _pclose);
        if (!pipe)
            throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get()))
        {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            {
                result += buffer.data();
            }
        }

        return result;
#endif
    }

    static std::vector<std::string> split(const std::string &s, char seperator)
    {
        std::vector<std::string> output;

        std::string::size_type prev_pos = 0, pos = 0;

        while ((pos = s.find(seperator, pos)) != std::string::npos)
        {
            std::string substring(s.substr(prev_pos, pos - prev_pos));

            output.push_back(substring);

            prev_pos = ++pos;
        }

        output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

        return output;
    }

    class entry
    {
        void *m_pAddress = nullptr;
        std::string m_pDescription;
        std::string m_pSourceFile;
        std::string m_pSourceLine;

    public:
        const std::string &description() { return m_pDescription; }
        const std::string &source_file() { return m_pSourceFile; }
        const std::string &source_line() { return m_pSourceLine; }
        void *address() { return m_pAddress; }

        friend class stacktrace;
    };

    class stacktrace
    {
    public:
        static stacktrace current()
        {
            stacktrace current;

#if defined(__linux__) && (!defined(ANDROID) && !defined(__ANDROID__))
            void *buffer[512];
            int numFrames = backtrace(buffer, 32);
            char **framesDescriptions = backtrace_symbols(buffer, numFrames);

            for (int i = 0; i < numFrames; i++)
            {

                void *frameAddress = buffer[i];
                size_t p = 0;

                while (framesDescriptions[i][p] != '(' &&
                       framesDescriptions[i][p] != ' ' &&
                       framesDescriptions[i][p] != 0)
                    ++p;

                char cmd[256];
                sprintf(cmd, "addr2line -i -C -e %.*s -f %p ", p, framesDescriptions[i], buffer[i]);

                auto message = sh(cmd);

                auto lines = split(message, '\n');
                auto data = split(lines[1], ':');

                if (std::string(lines[0]).find("??") != std::string::npos)
                    continue;

                entry e;
                e.m_pDescription = std::string(lines[0]);
                e.m_pSourceFile = data.at(0);
                e.m_pSourceLine = data.at(1);
                e.m_pAddress = buffer[i];

                current.m_pEntries.push_back(e);
            }

            free(framesDescriptions);
#endif

#if false && (!defined(ANDROID) && !defined(__ANDROID__))

        BOOL    result;
        HANDLE  process;
        HANDLE  thread;
        HMODULE hModule;

        STACKFRAME64        stack;
        ULONG               frame;
        DWORD64             displacement;

        DWORD disp;
        IMAGEHLP_LINE64* line;

        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        char name[PATH_MAX];
        char module[PATH_MAX];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

        // On x64, StackWalk64 modifies the context record, that could
        // cause crashes, so we create a copy to prevent it
        CONTEXT ctxCopy;
        memcpy(&ctxCopy, ctx, sizeof(CONTEXT));

        memset(&stack, 0, sizeof(STACKFRAME64));

        process = GetCurrentProcess();
        thread = GetCurrentThread();
        displacement = 0;
#if !defined(_M_AMD64)
        stack.AddrPC.Offset = (*ctx).Eip;
        stack.AddrPC.Mode = AddrModeFlat;
        stack.AddrStack.Offset = (*ctx).Esp;
        stack.AddrStack.Mode = AddrModeFlat;
        stack.AddrFrame.Offset = (*ctx).Ebp;
        stack.AddrFrame.Mode = AddrModeFlat;
#endif

        SymInitialize(process, NULL, TRUE); //load symbols

        for (frame = 0; ; frame++)
        {
            //get next call from stack
            result = StackWalk64
            (
#if defined(_M_AMD64)
                IMAGE_FILE_MACHINE_AMD64
#else
                IMAGE_FILE_MACHINE_I386
#endif
                ,
                process,
                thread,
                &stack,
                &ctxCopy,
                NULL,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                NULL
            );

            if (!result) break;

            //get symbol name for address
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            pSymbol->MaxNameLen = MAX_SYM_NAME;
            SymFromAddr(process, (ULONG64)stack.AddrPC.Offset, &displacement, pSymbol);

            line = (IMAGEHLP_LINE64*)malloc(sizeof(IMAGEHLP_LINE64));
            line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

            //try to get line
            if (SymGetLineFromAddr64(process, stack.AddrPC.Offset, &disp, line))
            {
                printf("\tat %s in %s: line: %lu: address: 0x%0X\n", pSymbol->Name, line->FileName, line->LineNumber, pSymbol->Address);
            }
            else
            {
                //failed to get line
                printf("\tat %s, address 0x%0X.\n", pSymbol->Name, pSymbol->Address);
                hModule = NULL;
                lstrcpyA(module, "");
                GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    (LPCTSTR)(stack.AddrPC.Offset), &hModule);

                //at least print module name
                if (hModule != NULL)GetModuleFileNameA(hModule, module, PATH_MAX);

                printf("in %s\n", module);
            }

            free(line);
            line = NULL;
        }
#endif
            return current;
        }

        [[nodiscard]] auto begin() { return m_pEntries.begin(); }

        [[nodiscard]] auto end() { return m_pEntries.end(); }

    private:
        std::list<entry> m_pEntries;
    };

}