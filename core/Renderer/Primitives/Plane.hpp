#pragma once

#include <pch.hpp>
#include "Primitive.hpp"

namespace Epsilon::Renderer
{
    class Plane : public PrimitiveBase
    {
    public:
        Plane()
        {
            type = DrawableType::PLANE;
            using vtx = PrimitiveBase::Vertex;
            using v3 = glm::vec3;
            using v2 = glm::vec2;
            Mesh.Vertices = {
                // bottom face
                vtx(v3(-0.5f, 0.0f, -0.5f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f)), // top-right
                vtx(v3(-0.5f, 0.0f, 0.5f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 1.0f)),  // top-left
                vtx(v3(0.5f, 0.0f, 0.5f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)),   // bottom-left
                vtx(v3(0.5f,0.0f, -0.5f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 0.0f)),  // bottom-right
            };


            Mesh.addTriangle(0, 1, 2);
            Mesh.addTriangle(0, 2, 3);

            generateTangentSpaceVectors();

            createVertexBuffers();

            mMaterial = MaterialManager::Get().createMaterial<MaterialPBR>();
        }

        void Render() override
        {
            mVertexArray->Bind();

            glDrawElements(GL_TRIANGLES, Mesh.Indices.size(), GL_UNSIGNED_INT, 0);

            mVertexArray->Unbind();
        }

        void Update() override {}
        void Destroy() override {}

        virtual void Draw(std::shared_ptr<Shader> shader, bool force_draw = false, std::initializer_list<unsigned int> mesh_index_list = {}) override
        {
            Render();
        }
        
        virtual std::shared_ptr<MaterialPBR> getMaterial() override
        {
            return MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial);
        }

        virtual MaterialManager::Material_id getMaterialId() override
        {
            return mMaterial;
        }

        void setMaterial(MaterialManager::Material_id mat_id) override
        {
            mMaterial = mat_id;
        }
    };

} // namespace Epsilon