#pragma once

#include <pch.hpp>

#include <nlohmann/json.hpp>

namespace Epsilon
{
    namespace IO
    {
        class Json
        {
        public:
            enum MODE
            {
                INPUT = 0x00000001,
                OUTPUT = 0x00000010
            };

        public:
            Json()
            {
                mJsonObject = std::make_shared<nlohmann::json>();
            }

            Json(const char *path, MODE mode = INPUT)
            {
                mJsonObject = std::make_shared<nlohmann::json>();
                Open(path, mode);
            }

            bool Open(const char *path, MODE mode = INPUT)
            {
                if (mode == INPUT)
                {
                    mInputFile = std::make_shared<std::ifstream>();
                    mInputFile->open(path, std::ios::in);
                    *mInputFile >> *mJsonObject;
                } else if(mode == OUTPUT) {
                    mOutputFile = std::make_shared<std::ofstream>();
                    mOutputFile->open(path, std::ios::out);
                }
                return true;
            }

            bool Close()
            {
                return false;
            }

            bool Save()
            {
                *mOutputFile << *mJsonObject;
                return true;
            }

            template<typename T>
            const auto & property(T t) {
                return &mJsonObject[t];
            }

            template<typename T>
            const auto & operator [](T t) noexcept {
                return &mJsonObject[t];
            }

        private:
            std::shared_ptr<nlohmann::json> mJsonObject;
            std::shared_ptr<std::ifstream> mInputFile;
            std::shared_ptr<std::ofstream> mOutputFile;
        };
    } // namespace IO
} // namespace Epsilon