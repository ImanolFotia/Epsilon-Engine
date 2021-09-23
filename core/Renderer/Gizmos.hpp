#pragma once

#include <pch.hpp>
#include <glm/glm.hpp>

#include <Driver/API/OpenGL/FrameBuffer.h>
#include <beacon/beacon.hpp>
#include "Model.h"
#include <IO/Mouse.h>

#include "MaterialPBR.hpp"

namespace Epsilon::Renderer
{
    class Gizmo
    {

    public:
        enum gizmo
        {
            scale = 0,
            rotate,
            translate,
            handle,
            none
        };

        Gizmo()
        {
            auto &ref = ResourceManager::Instance();
            ref.addModelToQueue(mModelName);

            {
                mat_x = std::make_shared<MaterialPBR>();
                mat_x->setMaterial(Material::MaterialParameter::Albedo, glm::vec3(0.0, 0.5, 0.0));

                mat_y = std::make_shared<MaterialPBR>();
                mat_y->setMaterial(Material::MaterialParameter::Albedo, glm::vec3(0.0, 0.0, 0.5));

                mat_z = std::make_shared<MaterialPBR>();
                mat_z->setMaterial(Material::MaterialParameter::Albedo, glm::vec3(0.5, 0.0, 0.0));
            }

            mShader = std::make_shared<Shader>("shaders/gizmo_vertex.glsl", "shaders/gizmo_fragment.glsl");
            Input::Mouse::MouseEventHandler += beacon::bind(&onClick, this);
        }

        void Render(gizmo type, glm::vec3 pos, glm::vec3 camPos)
        {
            mShader->Use();
            mShader->PushUniform("ModelMatrix", glm::translate(glm::mat4(1.0), pos) * glm::scale(glm::mat4(1.0), glm::vec3(glm::length(camPos - pos) * 0.08f)));
            mShader->PushUniform("EntityId", 2147483646);
            drawGuizmo(type);
            mShader->Free();
        }

        void Clear()
        {
        }

    private:
        void drawGuizmo(gizmo type) const
        {
            auto tModel = ResourceManager::Instance().Get<Model_ptr>(mModelName);
            int offset = 0;
            switch (type)
            {
            case translate:
                offset = translate_index_offset;
                break;
            case rotate:
                offset = rotate_index_offset;
                break;
            case scale:
                offset = scale_index_offset;
                break;
            case handle:
                offset = handle_index_offset;
                break;
            default:
                offset = 0;
                break;
            }

            mShader->PushUniform("subObjectId", offset);
            mShader->PushUniform("inColor", mat_x->get<glm::vec3>(Material::MaterialParameter::Albedo));
            tModel->Meshes().at(offset).DrawNoTexture();
            mShader->PushUniform("subObjectId", offset+1);
            mShader->PushUniform("inColor", mat_y->get<glm::vec3>(Material::MaterialParameter::Albedo));
            tModel->Meshes().at(offset + 1).DrawNoTexture();
            mShader->PushUniform("subObjectId", offset);
            mShader->PushUniform("inColor", mat_z->get<glm::vec3>(Material::MaterialParameter::Albedo));
            tModel->Meshes().at(offset + 2).DrawNoTexture();

            
            mShader->PushUniform("inColor", mat_x->get<glm::vec3>(Material::MaterialParameter::Albedo));
            tModel->Meshes().at(handle_index_offset).DrawNoTexture();
            mShader->PushUniform("inColor", mat_y->get<glm::vec3>(Material::MaterialParameter::Albedo));
            tModel->Meshes().at(handle_index_offset + 1).DrawNoTexture();
            mShader->PushUniform("inColor", mat_z->get<glm::vec3>(Material::MaterialParameter::Albedo));
            tModel->Meshes().at(handle_index_offset + 2).DrawNoTexture();
        }

        void onClick(beacon::sender *sender, beacon::args *args)
        {
            using Input::STATE;
            auto cArgs = args->to<Input::MouseArgs>();
            if (cArgs.Left().State == STATE::DOWN)
            {
            }
        }

    private:
        unsigned int scale_index_offset = 0;
        unsigned int rotate_index_offset = 3;
        unsigned int translate_index_offset = 6;
        unsigned int handle_index_offset = 9;

        glm::vec3 xColor = glm::vec3(1.0, 0.0, 0.0);
        glm::vec3 yColor = glm::vec3(0.0, 1.0, 0.0);
        glm::vec3 zColor = glm::vec3(0.0, 0.0, 1.0);

        bool isPressed;

        std::string mModelName = "models/UI/gizmos.eml";

        std::shared_ptr<Shader> mShader;

        std::shared_ptr<MaterialPBR> mat_x;
        std::shared_ptr<MaterialPBR> mat_y;
        std::shared_ptr<MaterialPBR> mat_z;

    protected:
    };
}