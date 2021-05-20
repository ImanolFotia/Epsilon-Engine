#include <pch.hpp>

#include "MaterialPBR.hpp"

namespace Epsilon::Renderer
{
    template <>
    [[maybe_unused]] bool MaterialPBR::setMaterial(Material::MaterialParameter t, std::shared_ptr<Texture2D> p)
    {
        switch (t)
        {
        case MaterialParameter::Albedo:
            mAlbedoTex = p;
            return true;
        case MaterialParameter::Roughness:
            mRoughnessTex = p;
            return true;
        case MaterialParameter::Metallic:
            mMetallicTex = p;
            return true;
        case MaterialParameter::Normal:
            mNormalTex = p;
            return true;
        default:
            return false;
        }
    }

    template <>
    [[maybe_unused]] bool MaterialPBR::setMaterial(Material::MaterialParameter t, glm::vec3 p)
    {
        switch (t)
        {
        case MaterialParameter::Albedo:
            albedo_color = p;
            return true;
        case MaterialParameter::Roughness:
            roughness_color = p;
            return true;
        case MaterialParameter::Metallic:
            metallic_color = p;
            return true;
        default:
            return false;
        }
    }

    template <>
    [[maybe_unused]] std::shared_ptr<Texture2D> MaterialPBR::get(Material::MaterialParameter t)
    {
        switch (t)
        {
        case MaterialParameter::Albedo:
            return mAlbedoTex;
        case MaterialParameter::Roughness:
            return mRoughnessTex;
        case MaterialParameter::Metallic:
            return mMetallicTex;
        case MaterialParameter::Normal:
            return mNormalTex;
        default:
            return nullptr;
        }
    }

    template <>
    [[maybe_unused]] glm::vec3 MaterialPBR::get(Material::MaterialParameter t)
    {
        switch (t)
        {
        case MaterialParameter::Albedo:
            return albedo_color;
        case MaterialParameter::Roughness:
            return roughness_color;
        case MaterialParameter::Metallic:
            return metallic_color;
        case MaterialParameter::Normal:
            return normal_color;
        default:
            return glm::vec3(0.0f);
        }
    }
}