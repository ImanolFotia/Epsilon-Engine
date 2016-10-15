#pragma once

namespace Renderer
{
class Renderer
{
public:
    Renderer() {}
    virtual ~Renderer(){}

public:
    virtual Render(GLuint VAO, GLuint TextureID, GLuint shader) = 0;
};
}
