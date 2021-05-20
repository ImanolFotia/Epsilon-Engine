#pragma once 

#include <string>
#include <memory>

namespace Epsilon::Filesystem {
    class MountingPoint {
        std::string mName;
        std::string mAbsolutePath;

    };

    using MountPoint_ptr = std::shared_ptr<MountingPoint>;
}