#pragma once

#include <iostream>
#include <list>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#ifdef __linux__
#include <zconf.h>
#include <execinfo.h>
#include <cerrno>
#include <unistd.h>
#include <linux/limits.h>
#endif

namespace framework {

#ifdef __linux__
static std::string get_path()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    auto str = std::string(result, (count > 0) ? count : 0);
    std::cout << str;
    return "";
}

static std::string sh(const std::string& cmd)
{
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
}

#endif

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

#ifdef __linux__
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

        return current;
    }

    [[nodiscard]] auto begin() { return m_pEntries.begin(); }

    [[nodiscard]] auto end() { return m_pEntries.end(); }

private:
    std::list<entry> m_pEntries;
};

}