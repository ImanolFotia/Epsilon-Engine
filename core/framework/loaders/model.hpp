///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#pragma once

/// GL Includes
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"
#include "eml1_0.hpp"

namespace framework
{
    class Model
    {


    public:

        std::string path{};

        Model(const std::string &path = "");

        // Model(const char *path, glm::vec3 pos = glm::vec3(0, 0, 0), glm::vec3 sc = glm::vec3(0, 0, 0), glm::quat rot = glm::quat(0, 0, 0, 1));

        std::string getPath()
        {
            return this->path;
        }


        common::MIN_MAX_POINTS getMeshBoundingBox(unsigned int index, glm::vec3 position, glm::vec3 scale, glm::quat rotation);

        /// Structure to store the models bounding box for visibility and collision computation
        struct BOUNDING_BOX
        {
            float FRONT_TOP_LEFT = 0.f;
            float FRONT_TOP_RIGHT = 0.f;
            float FRONT_BOTTOM_LEFT = 0.f;
            float FRONT_BOTTOM_RIGHT = 0.f;

            float BACK_TOP_LEFT = 0.;
            float BACK_TOP_RIGHT = 0.f;
            float BACK_BOTTOM_LEFT = 0.f;
            float BACK_BOTTOM_RIGHT = 0.f;
        };

        common::MIN_MAX_POINTS MinMaxPoints;

        BOUNDING_BOX BoundingBox;
        bool updateCubemaps = false;

        std::vector<common::MIN_MAX_POINTS> mMeshesBoundingBoxes;
        std::vector<glm::vec3> mMeshesPositions;
        std::vector<glm::vec3> mMeshesPositionsRelative;


        long toHash()
        {
            return std::hash<std::string>{}(path);
        }

        std::vector<Mesh> &Meshes()
        {
            return mMeshes;
        }



        std::unordered_map<int, std::string> mMeshesNames;

    private:
        std::vector<Mesh> mMeshes;
    };

    using Model_ptr = std::shared_ptr<Model>;

} // namespace Epsilon
