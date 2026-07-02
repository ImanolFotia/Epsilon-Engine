#pragma once

#include <string>
#include <vector>

#include "log.hpp"
namespace framework {
class StringUtils
{
public:
    static std::vector<std::string> split(std::string str, std::string token)
    {
        std::vector<std::string> output{};

        if (str.size() <= 2)
            return {};

        size_t pos = 0;

        while (pos != std::string::npos)
        {
            pos = str.find(token);
            output.push_back(str.substr(0, pos));
            if (pos + token.size() < str.size())
              str = str.substr(pos + token.size());
        }

        return output;
    }
};
}
