#pragma once 

#include <engine/types.hpp>

namespace engine {

    struct Material {
        uint32_t material_id;
    };
    
    struct IndirectPack {
        Mesh* mesh;
        Material* material;
        uint32_t first;
        uint32_t count;
    }
}