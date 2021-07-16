#pragma once 

#include <string>
#include <memory>

namespace Epsilon::Filesystem {
    struct MountingPoint {
        std::string Name;
        std::string AbsolutePath;

    };

    using MountPoint_ptr = std::shared_ptr<MountingPoint>;
}