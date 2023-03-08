#pragma once

#include <core/engine/renderer/drawables/mesh.hpp>

namespace framework
{
    class Model
    {
        std::vector<engine::Mesh> m_pMeshes;

    public:
        Model() = default;
        Model(const char *filename);

        void Load(const char *filename);
        void Destroy();

        auto begin();
        auto end();

    private:
    };
}