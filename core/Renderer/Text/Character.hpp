#pragma once

namespace Epsilon
{
    class Character
    {
    public:
        int codePoint, x, y, width, height, originX, originY;
    };

    struct CharacterVertex {
        glm::vec3 position;
        glm::vec2 texcoords;
        glm::vec4 color;
    };

} // namespace Epsilon