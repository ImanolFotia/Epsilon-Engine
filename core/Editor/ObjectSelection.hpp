#pragma once

#include <Shader.h>
#include <camera.h>
#include <Entity/Entity.h>

namespace Epsilon::Editor
{

    struct selection_t
    {
        enum SelectionType
        {
            NONE,
            ENTITY,
            CAMERA,
            SHADER,
            LIGHT,
            SIZE
        };

        using entity_type = std::shared_ptr<EntityBase>;
        using camera_type = std::shared_ptr<Camera>;
        using shader_type = std::shared_ptr<Shader>;
        using light_type = int32_t;

        entity_type selected_entity;
        camera_type selected_camera;
        shader_type selected_shader;
        light_type selected_light;

        SelectionType Type;

        template <typename T>
        T getSelection() { return 0; }

        SelectionType currentSelection() { return Type;}
    };

}