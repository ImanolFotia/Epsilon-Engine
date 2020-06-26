#pragma once 

#include <nlohmann/json.hpp>

namespace Epsilon {
    namespace IO {
        class Json {
            public:
                Json() {
                    mJsonObject = std::make_shared<nlohmann::json>();
                }

                bool Open() {

                }

                private:
                    std::shared_ptr<nlohmann::json> mJsonObject;
        };
    }
}