#pragma once

#ifndef __linux__
#error This functionality is not available on linux yet
#endif // __linux__

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <string>
#include <vector>

namespace EFS
{
    static const std::vector<std::string> getFilenamesInDirectory(std::string path, std::string ext = "*")
    {

        std::vector<std::string> names;
        std::string search_path = path + "/*." + ext;
        WIN32_FIND_DATA fd;
        HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
        if(hFind != INVALID_HANDLE_VALUE) {
            do {
                // read all (real) files in current folder
                // , delete '!' read other 2 default folder . and ..
                if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                    names.push_back(fd.cFileName);
                }
            } while(::FindNextFile(hFind, &fd));
            ::FindClose(hFind);
        }

        return names;
    }
}

