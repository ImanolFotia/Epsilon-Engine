#pragma once

#include <stdexcept>
#include "stacktrace.hpp"

namespace framework {
    class NotImplemented : public std::logic_error {
    public:
        NotImplemented(const std::string &file, const std::string &funcName)
                : std::logic_error("On file: " + file + "\nFunction not yet implemented: " + funcName + "\n") {

            int depth = 0;
            std::cout << "\033[1;31m------------begin stacktrace------------\033[0m\n";
            int i = 0;
            auto stacktrace = stacktrace::current();
            for (auto &entry: stacktrace) {
                if (depth < 2) {
                    depth++;
                    continue;
                }
                std::string dashes = std::string(depth, '-');

                std::cout << "\t"
                          << "\033[1;34m" << entry.address()
                          << "\033[0m at \033[1;33m" << entry.description()
                          << "\033[0m in (\033[4;37m" << entry.source_file()
                          << ":" << entry.source_line() << "\033[0m)\n";
                depth++;
            }

            std::cout << "\033[1;31m------------end stacktrace------------\033[0m\n";
        };
    };
}