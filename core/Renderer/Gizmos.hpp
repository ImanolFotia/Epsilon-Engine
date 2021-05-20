#pragma once

#include <pch.hpp>
#include <glm/glm.hpp>

#include <Driver/API/OpenGL/FrameBuffer.h>
#include <beacon/beacon.hpp>
#include "Model.h"
#include <IO/Mouse.h>

namespace Epsilon::Renderer { 
    class Gizmo {

        enum gizmo {
            translate = 0,
            rotate,
            scale,
            none
        };

        public:
            Gizmo() {
                
                ResourceManager::Instance().requestModel(mModelName);
                Input::Mouse::MouseEventHandler += beacon::bind(&onClick, this);
            }

            void Render(gizmo type, glm::vec3 pos) {
                auto tModel = ResourceManager::Instance().Get<Model_ptr>(mModelName);
                tModel->Draw(mShader);
            }

            void Clear() {
                
            }

            private:
                void onClick(beacon::sender *sender, beacon::args* args) {
                    using Input::STATE;
                    auto cArgs = args->to<Input::MouseArgs>();
                    if(cArgs.Left().State == STATE::DOWN) {

                    }
                }

        private:
            unsigned int translate_index_offset = 0;
            unsigned int rotate_index_offset = 0;
            unsigned int scale_index_offset = 0;

            glm::vec3 xColor = glm::vec3(1.0, 0.0, 0.0);
            glm::vec3 yColor = glm::vec3(0.0, 1.0, 0.0);
            glm::vec3 zColor = glm::vec3(0.0, 0.0, 1.0);

            bool isPressed;

            std::string mModelName = "resources/gizmos.eml";

            std::shared_ptr<Shader> mShader;

        protected:
    };
}