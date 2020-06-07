#pragma once

#include "TextureBase.hpp"

namespace Epsilon
{
    namespace API
    {
        namespace OpenGL
        {

            class Texture2D : public TextureBase
            {
            public:
                Texture2D(int w, int h)
                {
                }

                ~Texture2D() {}
            };
        } // namespace OpenGL
    }     // namespace API
} // namespace Epsilon