#pragma once

#include <pch.hpp>

#include <Core.hpp>

#include "IText.hpp"
#include "Font.hpp"

#include <Driver/API/OpenGL/VertexArrayObject.h>
#include <Shader.h>

namespace Epsilon
{
    class SDFText : public IText
    {   
        public:
            SDFText(){
                mVAO = std::make_unique<API::OpenGL::VertexArrayObject>();
                mVAO->addBuffer(GL_ARRAY_BUFFER, sizeof(CharacterVertex) * NUM_CHARACTERS_RESERVE, nullptr, GL_DYNAMIC_DRAW);
                mVAO->setAttribute(3, sizeof(glm::vec3), (GLvoid*) offsetof(CharacterVertex, position));
                mVAO->setAttribute(2, sizeof(glm::vec2), (GLvoid*) offsetof(CharacterVertex, texcoords));
                mVAO->setAttribute(4, sizeof(glm::vec4), (GLvoid*) offsetof(CharacterVertex, color));
            }

            void Print(const std::string &, const glm::vec3 &pos, const glm::vec4 &col) override;
            void PrintLn(const std::string &, const glm::vec3 &pos, const glm::vec4 &col) override;

            void Clear() override;
            void Destroy() override;

        private:
            std::unique_ptr<Font> mFont;
            std::unique_ptr<API::OpenGL::VertexArrayObject> mVAO;
            std::unique_ptr<Shader> mShader;
    };
} // namespace Epsilon