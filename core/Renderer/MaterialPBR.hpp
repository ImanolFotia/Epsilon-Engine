#pragma once

#include "Material.hpp"

#include "Texture2D.hpp"

namespace Epsilon
{
    namespace Renderer
    {
        class MaterialPBR : public Material
        {
            
        using Texture2D_ptr = std::shared_ptr<Texture2D>;
        public:
            MaterialPBR() = default;

            ~MaterialPBR() {}

            template <typename Param>
            [[maybe_unused]] bool setMaterial(Material::MaterialParameter, Param p);

            template <typename T>
            T get(Material::MaterialParameter);

            bool usingAlbedoColor() { return use_albedo_color; }
            bool usingRoughnessColor() { return use_roughness_color; }
            bool usingMetallicColor() { return use_metallic_color; }
            bool usingNormalColor() { return use_normal_color; }

        private:
            Texture2D_ptr mAlbedoTex;
            Texture2D_ptr mRoughnessTex;
            Texture2D_ptr mMetallicTex;
            Texture2D_ptr mNormalTex;

            bool use_albedo_color = true;
            bool use_roughness_color = true;
            bool use_metallic_color = true;
            bool use_normal_color = true;

            glm::vec3 albedo_color = glm::vec3(0.0f);
            glm::vec3 roughness_color = glm::vec3(0.0f);
            glm::vec3 metallic_color = glm::vec3(0.0f);

            const glm::vec3 normal_color = glm::vec3(0.5f, 0.5f, 1.0f);
        };
        using MaterialPBR_ptr = std::shared_ptr<MaterialPBR>;

    } // namespace Renderer
} // namespace Epsilon