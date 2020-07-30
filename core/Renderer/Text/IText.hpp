#pragma once

namespace Epsilon
{
    static constexpr uint32_t NUM_CHARACTERS_RESERVE = 500;
    class IText
    {
    public:
        IText() = default;

        virtual void Print(const std::string &, const glm::vec3 &pos, const glm::vec4 &col) = 0;
        virtual void PrintLn(const std::string &, const glm::vec3 &pos, const glm::vec4 &col) = 0;
        virtual void Clear() = 0;

        virtual void Destroy() = 0;
    };
} // namespace Epsilon