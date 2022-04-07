#pragma once

#include <string>

namespace Epsilon::Renderer
{
    class MaterialBase
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
        std::string mName;

    public:
        MaterialBase() {}
        virtual ~MaterialBase() {}
    
        unsigned long Id() const { return mId; }
        
        void Name(const std::string& name) { mName = name; }

        const std::string& Name() const { return mName; }
    };
} // namespace Epsilon