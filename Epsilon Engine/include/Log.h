#pragma once
#include <iostream>
#include <fstream>
#include <string>


namespace Global {
    class Log
    {
    public:

        static void OpenFile(std::string path)
        {
            FILE.open(path.c_str());
        }

        static void WriteToLog(std::string logmesage)
        {
            FILE << logmesage << std::endl;
        }

        static std::ofstream FILE;
    };
}

