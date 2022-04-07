#include <pch.hpp>

#include "MaterialPBR.hpp"

namespace Epsilon::Renderer
{
    template <>
    [[maybe_unused]] bool MaterialPBR::setMaterial(MaterialBase::MaterialParameter t, std::shared_ptr<Texture2D> p)
    {
        switch (t)
        {
        case MaterialParameter::Albedo:
            use_albedo_color = false;
            if(mAlbedoTex) mAlbedoTex->decreaseRefCount();
            mAlbedoTex = p;
            mAlbedoTex->increaseRefCount();
            return true;
        case MaterialParameter::Roughness:
            use_roughness_color = false;
            if(mRoughnessTex) mRoughnessTex->decreaseRefCount();
            mRoughnessTex = p;
            mRoughnessTex->increaseRefCount();
            return true;
        case MaterialParameter::Metallic:
            use_metallic_color = false;
            if(mMetallicTex) mMetallicTex->decreaseRefCount();
            mMetallicTex = p;
            mMetallicTex->increaseRefCount();
            return true;
        case MaterialParameter::Normal:
            use_normal_color = false;
            if(mNormalTex) mNormalTex->decreaseRefCount();
            mNormalTex = p;
            mNormalTex->increaseRefCount();
            return true;
        default:
            return false;
        }
    }

    template <>
    [[maybe_unused]] bool MaterialPBR::setMaterial(MaterialBase::MaterialParameter t, glm::vec4 p)
    {
        switch (t)
        {
        case MaterialParameter::Albedo:
            use_albedo_color = true;
            albedo_color = p;
            return true;
        case MaterialParameter::Roughness:
            use_roughness_color = true;
            roughness_color = p;
            return true;
        case MaterialParameter::Metallic:
            use_metallic_color = true;
            metallic_color = p;
            return true;
        case MaterialParameter::Normal:
            use_normal_color = true;
            return true;
        default:
            return false;
        }
    }

    template <>
    [[maybe_unused]] std::shared_ptr<Texture2D> MaterialPBR::get(MaterialBase::MaterialParameter t)
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
    [[maybe_unused]] glm::vec4 MaterialPBR::get(MaterialBase::MaterialParameter t)
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
            return glm::vec4(0.0f);
        }
    }
}