#pragma once
#include <pch.hpp>

#include <Core.hpp>

#include <glm/glm.hpp>

namespace Epsilon
{
    namespace Text
    {
        class Command
        {
        public:
            Command(const std::string_view & str, const glm::vec3 pos, const glm::vec4 col, bool newline)
            : mText(str), mPosition(pos), mColor(col), mNewLine(newline) { }

            const std::string & Text() {
                return mText;
            }

            const glm::vec3 & Position() {
                return mPosition;
            }

            const glm::vec4 & Color() {
                return mColor;
            }

            const bool & NewLine() {
                return mNewLine;
            }

        private:
            std::string mText;
            glm::vec3 mPosition;
            glm::vec4 mColor;
            bool mNewLine;
        };
    } // namespace Text
} // namespace Epsilon