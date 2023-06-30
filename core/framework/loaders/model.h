///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#pragma once

/// GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"
#include "eml1_0.hpp"

#include <core/common/common.hpp>

namespace framework
{
    template<typename MeshType = framework::Mesh<common::Mesh>>
    class ModelBase {
    protected:
        std::string filename{};
        std::vector<MeshType> mMeshes;
    public:
        ModelBase(const std::string& filename) : filename(filename){}

        std::vector<MeshType>& Meshes()
        {
            return mMeshes;
        }

        virtual bool Load(const std::string& emlPath) = 0;
    };

    class Model : public ModelBase <framework::Mesh <common::Mesh>>
    {
        using MeshType = framework::Mesh<common::Mesh>;
        using InternalMeshType = common::Mesh;
    public:

        Model(const std::string &path = "");

        common::BoundingBox getMeshBoundingBox(unsigned int index, glm::vec3 position, glm::vec3 scale, glm::quat rotation);

        /// Structure to store the models bounding box for visibility and collision computation

        common::BoundingBox BoundingBox;
        common::MIN_MAX_POINTS MinMaxPoints;

        long toHash()
        {
            return std::hash<std::string>{}(filename);
        }

        bool Load(const std::string& emlPath) override;

    };

} // namespace Epsilon
