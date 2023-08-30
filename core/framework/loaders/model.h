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
#include "gltf_animation.hpp"

namespace framework
{
    class ModelBase {
    protected:
        std::string filename{};
        std::vector<framework::Mesh> mMeshes;
        std::vector<common::AnimatedMesh> mAnimatedMeshes;

        Skeleton m_pSkeleton;
        std::vector<Animation> m_pAnimations;

        bool m_pHasAnimation = false;

        common::BoundingBox BoundingBox;
        common::MIN_MAX_POINTS MinMaxPoints;

        glm::vec3 m_pMin = glm::vec3(1000000.0);
        glm::vec3 m_pMax = glm::vec3(-1000000.0);

    public:
        ModelBase(const std::string& filename) : filename(filename){}

        std::vector<framework::Mesh>& Meshes()
        {
            return mMeshes;
        }

        const std::vector<common::AnimatedMesh>& AnimatedMeshes() {
            return mAnimatedMeshes;
        }

        virtual bool Load(const std::string& emlPath) = 0;

        Skeleton Skeleton() {
            return m_pSkeleton;
        }

        std::vector<Animation> Animations() {
            return m_pAnimations;
        }

        bool HasAnimation() {
            return m_pHasAnimation;
        }

    };

    class Model : public ModelBase
    {
        using MeshType = framework::Mesh;
    public:

        Model(const std::string &path = "");

        common::BoundingBox getMeshBoundingBox(unsigned int index, glm::vec3 position, glm::vec3 scale, glm::quat rotation);

        /// Structure to store the models bounding box for visibility and collision computation


        long toHash()
        {
            return std::hash<std::string>{}(filename);
        }

        bool Load(const std::string& emlPath) override;

    };

} // namespace Epsilon
