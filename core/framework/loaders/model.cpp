#include "model.hpp"

#include <assimp/Importer.hpp>

namespace framework
{
    Model::Model()
    {
    }

    Model::Model(const char *filename)
    {
        Load(std::forward(filename));
    }

    void Model::Load(const char *filename)
    {
    }

    void Model::Destroy()
    {
    }

    auto Model::begin()
    {
        return m_pMeshes.begin();
    }

    auto Model::end()
    {
        return m_pMeshes.end();
    }
}