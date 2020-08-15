#include <pch.hpp>

#include "TextManager.hpp"

namespace Epsilon
{
    namespace Text
    {
        TextManager::TextManager(enum TEXT_TYPE type)
        {
            switch (type)
            {
            case TEXT_TYPE::SDF:
                mTextRenderer = std::make_unique<SDFTextRenderer>();
                break;
            default:
                IO::PrintLine("Error: Text type not supported");
                break;
            }
        }
        void TextManager::Print(const std::string &str, const glm::vec3 &pos, const glm::vec4 &col)
        {
            mCommandQueue.emplace(str, pos, col, false);
        }
        
        void TextManager::Print(std::string &&str, glm::vec3 &&pos, glm::vec4 &&col)
        {
            mCommandQueue.emplace(str, pos, col, false);
        }

        void TextManager::PrintLn(const std::string &str, const glm::vec3 &pos, const glm::vec4 &col)
        {
            mCommandQueue.emplace(str, pos, col, true);
        }

        void TextManager::PrintLn(std::string &&str, glm::vec3 &&pos, glm::vec4 &&col)
        {
            mCommandQueue.emplace(str, pos, col, true);
        }
        
        void TextManager::Flush()
        {
            while (!mCommandQueue.empty())
            {
                auto &front = mCommandQueue.front();
                if (front.NewLine())
                {
                    mTextRenderer->PrintLn(front.Text(), front.Position(), front.Color());
                }
                else
                {
                    mTextRenderer->Print(front.Text(), front.Position(), front.Color());
                }
                mCommandQueue.pop();
            }
        }
        void TextManager::Clear()
        {
            while (!mCommandQueue.empty())
                mCommandQueue.pop();
            mTextRenderer->Clear();
        }
    } // namespace Text
} // namespace Epsilon