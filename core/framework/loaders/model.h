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
    class Model
    {

        std::string path{};
        std::vector<Mesh> mMeshes;

    public:


        Model(const std::string &path = "");



        common::BoundingBox getMeshBoundingBox(unsigned int index, glm::vec3 position, glm::vec3 scale, glm::quat rotation);

        /// Structure to store the models bounding box for visibility and collision computation

        common::BoundingBox BoundingBox;
        common::MIN_MAX_POINTS MinMaxPoints;

        long toHash()
        {
            return std::hash<std::string>{}(path);
        }

        std::vector<Mesh> &Meshes()
        {
            return mMeshes;
        }

        std::vector<unsigned int> mVisibleMeshes;

        bool loadModel(std::string emlPath);



    };

} // namespace Epsilon
