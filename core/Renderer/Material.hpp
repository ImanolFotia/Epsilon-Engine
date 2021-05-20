#pragma once

namespace Epsilon::Renderer
{
    class Material
    {
    public:
        enum class MaterialParameter
        {
            //Identifiers for when a texture path or object is passed
            Albedo,
            Metallic,
            Roughness,
            Normal,
            SubsurfaceScattering,
            Specular,
            Emission
        };

        enum class ParameterType {
            Color,
            Texture
        };

        enum class MaterialParameterTexture
        {
        };

        unsigned long mId;

    public:
        Material() {}
        virtual ~Material() {}
    
        unsigned long Id() const { return mId; }
    };
} // namespace Epsilon