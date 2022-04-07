#pragma once

#include <glm/glm.hpp>
#include <Resource/ResourceManager.h>
#include "../ShaderStorage.hpp"

namespace Epsilon::Renderer {
    
    constexpr unsigned MAX_SURFELS_PER_NODE = 20;
    constexpr unsigned MAX_SURFELS_NODES = 100;

    enum TREE_CUADRANT {
        TOP_BACK_LEFT = 0,
        TOP_BACK_RIGHT,
        TOP_FRONT_LEFT,
        TOP_FRONT_RIGHT,
        BOTTOM_BACK_LEFT,
        BOTTOM_BACK_RIGHT,
        BOTTOM_FRONT_LEFT,
        BOTTOM_FRONT_RIGHT,

    };

    struct Surfel {
        glm::vec3 position;     float padding0;
        glm::vec3 normal;       float padding1;
        glm::vec3 irradiance;   float padding2;
        float radius;
        unsigned int entity_id;
    };

    struct SurfelNode {
        Surfel surfels[MAX_SURFELS_PER_NODE];
        glm::vec3 median;
        unsigned children[8];
    };

    struct SurfelTree {
        SurfelNode Nodes[MAX_SURFELS_NODES];
        unsigned max_depth;
    };

    class SurfelSolver {
        public:
            SurfelSolver() {
                mSurfelStorage = std::make_shared<Renderer::ShaderStorage>(sizeof(SurfelTree), 1);
            }

        std::shared_ptr<Renderer::ShaderStorage> mSurfelStorage;
    };
}