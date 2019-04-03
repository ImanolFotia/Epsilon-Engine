#pragma once
#include <iostream>
#include <fstream>
#include <string>

#include <time.h>


namespace Global {

    // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    static const std::string currentDateTime() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

        return buf;
    }

    class Log
    {
    public:
        static void OpenFile(std::string path)
        {
            FILE.open(path.c_str());
        }

        static void WriteToLog(std::string logmesage)
        {
            FILE << currentDateTime() << ":  " << logmesage << std::endl;
        }

        static std::ofstream FILE;
    };
}

