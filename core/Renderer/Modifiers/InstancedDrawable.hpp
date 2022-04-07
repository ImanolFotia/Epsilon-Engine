#pragma once

#include <Transform.hpp>
#include "../ModelBase.hpp"
#include <Driver/API/OpenGL/VertexArrayObject.h>

namespace Epsilon::Renderer
{
    class InstancedDrawable
    {
        using Drawable_ptr = std::shared_ptr<Drawable>;

        Drawable_ptr drawable;

    public:
        InstancedDrawable(Drawable_ptr d, std::vector<Transform> transforms) : drawable(d)
        {
            num_instances = transforms.size();
            switch (d->getType())
            {
            case DrawableType::SPHERE:
            {
                auto p = std::static_pointer_cast<Sphere>(d);
                auto geometry = p->getGeometry();
                createVertexBuffersForPrimitive(geometry, transforms);
            }
            break;
            case DrawableType::CUBE:
                break;
            case DrawableType::CYLINDER:
                break;
            case DrawableType::TRIANGLE_MESH:
                break;
            case DrawableType::PLANE:
                break;
            case DrawableType::TOROID:
                break;
            default:
                throw("Type not defined");
            }
        }

        void createVertexBuffersForPrimitive(const PrimitiveBase::Mesh_t &mesh_data, std::vector<Transform> transforms)
        {

            num_indices = mesh_data.Indices.size();

            glm::mat4 *modelMatrices = new glm::mat4[transforms.size()];
            glm::mat4 model;
            for (unsigned int i = 0; i < transforms.size(); ++i)
            {
                model = glm::mat4(1.0);
                model = glm::translate(glm::mat4(1.0), transforms[i].getPosition());
                model = glm::scale(model, glm::vec3(1.0));
                modelMatrices[i] = model;
            }

            mVAO = std::make_shared<API::OpenGL::VertexArrayObject>();

            mVAO->IndexBuffer(mesh_data.Indices);

            auto a = mVAO->addBuffer(sizeof(PrimitiveBase::Vertex) * mesh_data.Vertices.size(), &mesh_data.Vertices[0], GL_STATIC_DRAW);
            mVAO->bindBuffer(a);
            mVAO->setAttribute(VERTEX_POINTER_INDEX::POSITION, 3, sizeof(PrimitiveBase::Vertex), (GLvoid *)offsetof(PrimitiveBase::Vertex, position));
            mVAO->setAttribute(VERTEX_POINTER_INDEX::UV, 2, sizeof(PrimitiveBase::Vertex), (GLvoid *)offsetof(PrimitiveBase::Vertex, uv));
            mVAO->setAttribute(VERTEX_POINTER_INDEX::NORMAL, 3, sizeof(PrimitiveBase::Vertex), (GLvoid *)offsetof(PrimitiveBase::Vertex, normal));
            mVAO->setAttribute(VERTEX_POINTER_INDEX::TANGENT, 3, sizeof(PrimitiveBase::Vertex), (GLvoid *)offsetof(PrimitiveBase::Vertex, bitangent));
            mVAO->setAttribute(VERTEX_POINTER_INDEX::BITANGENT, 3, sizeof(PrimitiveBase::Vertex), (GLvoid *)offsetof(PrimitiveBase::Vertex, tangent));

            auto b = mVAO->addBuffer(transforms.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
            mVAO->bindBuffer(b);
            mVAO->setAttribute(5, 4, sizeof(glm::mat4), (GLvoid *)0);
            mVAO->setAttribute(6, 4, sizeof(glm::mat4), (GLvoid *)(     sizeof(glm::vec4)));
            mVAO->setAttribute(7, 4, sizeof(glm::mat4), (GLvoid *)(2 *  sizeof(glm::vec4)));
            mVAO->setAttribute(8, 4, sizeof(glm::mat4), (GLvoid *)(3 *  sizeof(glm::vec4)));
            mVAO->setAttributeDivisor(5, 1);
            mVAO->setAttributeDivisor(6, 1);
            mVAO->setAttributeDivisor(7, 1);
            mVAO->setAttributeDivisor(8, 1);
            mVAO->Unbind();
            delete[] modelMatrices;
        }

        void Render()
        {

            mVAO->Bind();

            glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0, num_instances);

            mVAO->Unbind();
        }

    private:
        void Init()
        {
        }

        uint32_t num_instances = 0;
        uint32_t num_indices = 0;

        GLuint VAO, VBO, EBO;

        std::shared_ptr<API::OpenGL::VertexArrayObject> mVAO;
        std::shared_ptr<API::OpenGL::VertexArrayObject> mMatrixBuffer;
    };
}