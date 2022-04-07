#pragma once

#include <pch.hpp>
#include <iomanip>

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

            Json(const char *path, MODE mode = INPUT) : mMode(mode), mFileName(path)
            {
                mJsonObject = std::make_shared<nlohmann::json>();
                Open(path, mode);
            }

            bool Open(const char *path, MODE mode = INPUT)
            {

                std::cout << "Attempting to open " << path << std::endl;
                if (mode == INPUT)
                {
                    mInputFile = std::make_shared<std::ifstream>();
                    mInputFile->open(path, std::ios::in);
                    (*mInputFile) >> (*mJsonObject);
                }
                return true;
            }

            bool Close()
            {
                mOutputFile->close();
                return true;
            }

            bool Save()
            {
                auto o = std::ofstream(mFileName);
                o << std::setw(4) << (*mJsonObject) << std::endl;
                return true;
            }

            template<typename T>
            const auto & property(T t) {
                return (*mJsonObject)[t];
            }
            
            template<typename T, typename V>
            void property(T t, V v) {
                mJsonObject->emplace(t, v);
            }
            
            template<typename T>
            void property(T t, nlohmann::json j) {
                mJsonObject->emplace(t, j);
            }

            template<typename T>
            auto operator [](T t) noexcept {
                return (*mJsonObject)[t];
            }

        private:
            std::shared_ptr<nlohmann::json> mJsonObject;
            std::shared_ptr<std::ifstream> mInputFile;
            std::shared_ptr<std::ofstream> mOutputFile;
            MODE mMode;
            std::string mFileName;
        };
    } // namespace IO
} // namespace Epsilon