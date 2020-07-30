#pragma once

#include <pch.hpp>

#include "Command.hpp"
#include "IText.hpp"
#include "SDFText.hpp"

namespace Epsilon
{
    namespace Text
    {
        class TextManager
        {
        public:
            TextManager() = default;

            void Print(const std::string &str, const glm::vec3 &pos, const glm::vec4 &col) {
                mCommandQueue.emplace(str, pos, col, false);
            }
            void PrintLn(const std::string &str, const glm::vec3 &pos, const glm::vec4 &col) {
                mCommandQueue.emplace(str, pos, col, true);
            }

            void Flush() {
                while(!mCommandQueue.empty()){
                    auto & front = mCommandQueue.front();
                    if(front.NewLine()) {
                        mText->PrintLn(front.Text(), front.Position(), front.Color());
                    } else {
                        mText->Print(front.Text(), front.Position(), front.Color());
                    }
                    mCommandQueue.pop();
                }
            }

            void Clear() {
                while(!mCommandQueue.empty()) mCommandQueue.pop();
                mText->Clear();
            }

        private:
            std::queue<Command> mCommandQueue;
            std::unique_ptr<IText> mText;
        };
    } // namespace Text
} // namespace Epsilon