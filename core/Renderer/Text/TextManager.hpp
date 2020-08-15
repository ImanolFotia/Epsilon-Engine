#pragma once

#include <pch.hpp>

#include "Command.hpp"
#include "IText.hpp"
#include "SDFTextRenderer.hpp"

namespace Epsilon
{
    namespace Text
    {   
        enum class TEXT_TYPE: uint8_t {
            TTF = 0,
            SDF,
        };

        class TextManager
        {
        public:
            TextManager(enum TEXT_TYPE type);

            void Print(const std::string &, const glm::vec3 &, const glm::vec4 &);
            void Print(std::string &&, glm::vec3 &&, glm::vec4 &&);

            void PrintLn(const std::string &, const glm::vec3 &, const glm::vec4 &);
            void PrintLn(std::string &&, glm::vec3 &&, glm::vec4 &&);

            void Flush();

            void Clear();

        private:
            std::queue<Command> mCommandQueue;
            std::unique_ptr<IText> mTextRenderer;
        };
    } // namespace Text
} // namespace Epsilon