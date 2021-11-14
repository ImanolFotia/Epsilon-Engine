#include "ObjectSelection.hpp"

namespace Epsilon::Editor {
    
    template<>
    selection_t::entity_type selection_t::getSelection() {
        return selected_entity;
    }
    
    template<>
    selection_t::camera_type selection_t::getSelection() {
        return selected_camera;
    }
    
    template<>
    selection_t::shader_type selection_t::getSelection() {
        return selected_shader;
    }
    
    template<>
    selection_t::light_type selection_t::getSelection() {
        return selected_light;
    }

}