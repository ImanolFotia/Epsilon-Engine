#pragma once

namespace Epsilon::Editor {

    struct FileData {
        std::string Name;
        std::string Type;
        std::size_t Size;
        std::size_t NumItems = 0;
        std::size_t ItemsOffset = 0;
    };

    std::vector<FileData> Files;
}