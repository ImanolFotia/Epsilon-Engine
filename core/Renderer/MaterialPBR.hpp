#pragma once

#include "Material.hpp"

#include <Texture.h>

namespace Epsilon
{
    namespace Renderer
    {
        class MaterialPBR : public MaterialBase
        {
        
        using Texture2D_ptr = std::shared_ptr<Texture2D>;

        public:
            MaterialPBR() {}
            ~MaterialPBR() {}

        private:
            Texture2D_ptr mAlbedoTex;
            Texture2D_ptr mRoughnessTex;
            Texture2D_ptr mMetallicTex;
            Texture2D_ptr mNormalTex;
        };
    } // namespace Renderer
} // namespace Epsilon