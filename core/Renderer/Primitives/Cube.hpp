#pragma once

#include <pch.hpp>
#include "Primitive.hpp"

namespace Epsilon::Renderer
{

    class Cube : public PrimitiveBase
    {
    public:
        Cube()
        {
            using vtx = PrimitiveBase::Vertex;
            using v3 = glm::vec3;
            using v2 = glm::vec2;
            Mesh.Vertices = {
                // back face
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f)), // bottom-left
                vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 0.0f)),  // bottom-right
                vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f)), // bottom-left
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 1.0f)),  // top-left
                // front face
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f)), // bottom-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 0.0f)),  // bottom-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)),   // top-right
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 1.0f)),  // top-left
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f)), // bottom-left
                // left face
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-right
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)),  // top-left
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-left
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-left
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)),  // bottom-right
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-right
                // right face
                vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-left
                vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-right
                vtx(v3(1.0f, 1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)),  // top-right
                vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f)), // bottom-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)),   // top-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)),  // bottom-left
                // bottom face
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f)), // top-right
                vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 1.0f)),  // top-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-left
                vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-left
                vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 0.0f)),  // bottom-right
                vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f)), // top-right
                // top face
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f)), // top-left
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-right
                vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 1.0f)),  // top-right
                vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-right
                vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f)), // top-left
                vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 0.0f))   // bottom-left
            };
        }

        void Render() override {}
        void Update() override {}
        void Destroy() override {}
        
        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) override
        {
            mVertexArray->Bind();

            glDrawElements(GL_TRIANGLES, Mesh.Indices.size(), GL_UNSIGNED_INT, 0);

            mVertexArray->Unbind();
        }
        
        virtual std::shared_ptr<MaterialPBR> getMaterial() override
        {
            return mMaterial;
        }
    };

} // namespace Epsilon