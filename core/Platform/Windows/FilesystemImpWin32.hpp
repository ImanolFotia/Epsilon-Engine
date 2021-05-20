#pragma once

#ifdef _WIN32 

#include <beacon/beacon.hpp>

namespace Epsilon::Platform {

    class File {

    };

    class Watcher {
        //beacon::single_handler<>
        public:
            void onCreate() {}
            void OnChange() {}
            void onDelete() {}
    };

    class Filesystem {
        
    };
}

#endif