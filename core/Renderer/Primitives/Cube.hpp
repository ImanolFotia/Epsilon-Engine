#pragma once

#include <pch.hpp>
#include "Primitive.hpp"
#include "../MaterialManager.hpp"

namespace Epsilon::Renderer
{

    class Cube : public PrimitiveBase
    {
        float mUVMultiplier = 1.0f;
    public:
        Cube()
        {
            type = DrawableType::CUBE;
            using vtx = PrimitiveBase::Vertex;
            using v3 = glm::vec3;
            using v2 = glm::vec2;
            // Mesh.Vertices = {
            //  back face
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f))); // bottom-left
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)));   // top-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 0.0f)));  // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(1.0f, 1.0f)));   // top-right
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 0.0f))); // bottom-left
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 0.0f, -1.0f), v2(0.0f, 1.0f)));  // top-left

            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f))); // bottom-left
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 0.0f)));  // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)));   // top-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(1.0f, 1.0f)));   // top-right
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 1.0f)));  // top-left
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v2(0.0f, 0.0f))); // bottom-left

            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-right
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)));  // top-left
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-left
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-left
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)));  // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(-1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-right

            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-left
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 1.0f)));  // top-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 1.0f))); // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(1.0f, 0.0f)));   // top-left
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(1.0f, 0.0f, 0.0f), v2(0.0f, 0.0f)));  // bottom-left

            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f))); // top-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 1.0f)));  // top-left
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-left
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-left
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 0.0f)));  // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, -1.0f), v3(0.0f, -1.0f, 0.0f), v2(0.0f, 1.0f))); // top-right

            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f))); // top-left
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 1.0f)));  // top-right
            Mesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(1.0f, 0.0f)));   // bottom-right
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, -1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 1.0f))); // top-left
            Mesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v3(0.0f, 1.0f, 0.0f), v2(0.0f, 0.0f)));  // bottom-left
            //};
            // Generate tangent space vectors

            for (int i = 0; i < Mesh.Vertices.size(); i += 3)
            {
                Mesh.addTriangle(i, i + 1, i + 2);
            }

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
            mVertexArray->Bind();

            glDrawElements(GL_TRIANGLES, Mesh.Indices.size(), GL_UNSIGNED_INT, 0);

            mVertexArray->Unbind();
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

        void setUVMultiplier(float m) {
            mUVMultiplier = m;
        }

        float setUVMultiplier() {
            return mUVMultiplier;
        }
    };

} // namespace Epsilon