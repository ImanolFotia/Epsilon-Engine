#pragma once

#include <pch.hpp>

#include "Material.hpp"

#include "MaterialPBR.hpp"

namespace Epsilon::Renderer
{
    class MaterialManager
    {
    public:
        using Material_ptr = std::shared_ptr<MaterialBase>;
        using Material_id = std::string;

    public:
        static MaterialManager &Get()
        {
            return instance;
        }

        MaterialManager(const MaterialManager &) = delete;
        MaterialManager(MaterialManager &&) = delete;

        const Material_id &addMaterial(const Material_id &id, Material_ptr material)
        {
            if (material != nullptr)
            {
                mMaterials[id] = material;
            }

            return id;
        }

        template <typename T>
        Material_id createMaterial()
        {
            int index = 0;
            std::string candidate = std::string("material_") + std::to_string(mMaterials.size() + index);
            while (mMaterials.contains(candidate))
            {
                index++;
                candidate = std::string("material_") + std::to_string(mMaterials.size() + index);
            }
            std::cout << "found candidate: " << candidate << std::endl;
            mMaterials[candidate] = std::make_shared<T>();

            return candidate;
        }

        template <typename T>
        const Material_id &emplaceMaterial(const Material_id &id)
        {
            mMaterials[id] = std::make_shared<T>();
            return id;
        }

        Material_ptr getMaterial(const Material_id &id)
        {
            return mMaterials.at(id);
        }

        template <typename T>
        std::shared_ptr<T> getMaterial(const Material_id &id)
        {
            return std::static_pointer_cast<T>(mMaterials.at(id));
        }

        const std::unordered_map<Material_id, Material_ptr>& getMaterials() {
            return mMaterials;
        }

    private:
        static MaterialManager instance;
        MaterialManager() = default;

        std::unordered_map<Material_id, Material_ptr> mMaterials;
    };
}