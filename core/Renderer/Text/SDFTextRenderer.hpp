#pragma once

#include <pch.hpp>

#include <Engine.hpp>
#include <Core.hpp>
#include <IO/IO.hpp>

#include "IText.hpp"
#include "Font.hpp"

#include <Driver/API/OpenGL/VertexArrayObject.h>
#include <Shader.h>

namespace Epsilon
{
    class SDFTextRenderer : public IText
    {   
        public:
            SDFTextRenderer(){
                //Reserve memory for the first batch of data
                mData.insert(mData.begin(), std::make_unique<CharacterGlyph[]>(NUM_CHARACTERS_RESERVE));
                IO::PrintLine("Allocated", sizeof(CharacterGlyph) * NUM_CHARACTERS_RESERVE, "bytes for text rendering");
                mVAO = std::make_unique<API::OpenGL::VertexArrayObject>();
                mVAO->addBuffer(sizeof(CharacterGlyph) * NUM_CHARACTERS_RESERVE, nullptr, GL_DYNAMIC_DRAW);
                mVAO->setAttribute(VERTEX_POINTER_INDEX::POSITION, 3, sizeof(glm::vec3), (GLvoid*) offsetof(CharacterVertex, position));
                mVAO->setAttribute(VERTEX_POINTER_INDEX::UV, 2, sizeof(glm::vec2), (GLvoid*) offsetof(CharacterVertex, texcoords));
                mVAO->setAttribute(VERTEX_POINTER_INDEX::NORMAL, 4, sizeof(glm::vec4), (GLvoid*) offsetof(CharacterVertex, color));
            }

            void Print(const std::string &, const glm::vec3 &pos, const glm::vec4 &col) override {
                
            }
            void PrintLn(const std::string &, const glm::vec3 &pos, const glm::vec4 &col) override {

            }

            void Clear() override{}
            void Destroy() override{}

        private: 
            std::unique_ptr<Font> mFont;
            std::unique_ptr<API::OpenGL::VertexArrayObject> mVAO;
            std::unique_ptr<Shader> mShader;
            std::vector<std::unique_ptr<CharacterGlyph[]>> mData;
    };
} // namespace Epsilon