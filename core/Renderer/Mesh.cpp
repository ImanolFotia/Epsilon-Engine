#include "Mesh.h"
#include <Resource/ResourceManager.h>

#include <IO/IO.hpp>
#include "MaterialPBR.hpp"

namespace Epsilon::Renderer
{

    Mesh::Mesh(std::vector<Renderer::t_Vertex> vertices, std::vector<GLuint> indices, std::vector<MeshTexture> textures, unsigned vOffset, unsigned iOffset, int CubeMapindex)
    {
        //std::cout << "Setting up mesh" << std::endl;
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        mData = {vertices.size(), indices.size(), 0, 0};

        mMaterial = MaterialManager::Get().createMaterial<MaterialPBR>();
        //mMaterial = std::make_shared<Renderer::MaterialPBR>();

        CubeMapIndex = CubeMapindex;
        mGIIndex = 0;
        mCubemapIndex = 1;
        finalCubemaps = false;

        //std::cout << "Finished setting up mesh" << std::endl;
    }
    /// Render the mesh
    void Mesh::Draw(std::shared_ptr<Shader> shader, glm::vec3 pos)
    {
        using Tex2D_ptr = std::shared_ptr<Texture2D>;
        if (ResourceManager::Get().cubemapsLoaded && finalCubemaps != true)
        {
            mPosition = pos;
            finalCubemaps = true;
            updateCubemaps = false;
            mCubemapIndex = ResourceManager::Get().getNearestCubemapIndex(pos);
            mGIIndex = ResourceManager::Get().NearestCubeMap(pos) - 1;
            //IO::PrintLine("Cubemap index registered: ", mGIIndex, " at position: ", pos.x, pos.y, pos.z);
        }

        if (isVisible && isSettedUp)
        {

            auto albedo = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Albedo);
            auto roughness = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Roughness);
            auto metallic = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Metallic);
            auto normal = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Normal);

            if (albedo != nullptr && !MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->usingAlbedoColor())
            {
                albedo->Bind(0);
                shader->PushUniform("texture_diffuse", 0);
                shader->PushUniform("using_color_diffuse", 0);
            }
            else
            {
                shader->PushUniform("texture_diffuse", 0);
                shader->PushUniform("using_color_diffuse", 1);
                shader->PushUniform("color_diffuse", glm::vec4(MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<glm::vec4>(MaterialBase::MaterialParameter::Albedo)));
            }
            if (roughness != nullptr && !MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->usingRoughnessColor())
            {
                roughness->Bind(1);
                shader->PushUniform("texture_specular", 1);
                shader->PushUniform("using_color_specular", 0);
            }
            else
            {
                shader->PushUniform("texture_specular", 1);
                shader->PushUniform("using_color_specular", 1);
                shader->PushUniform("color_specular", glm::vec4(MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<glm::vec4>(MaterialBase::MaterialParameter::Roughness)));
            }
            if (normal != nullptr && !MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->usingNormalColor())
            {
                normal->Bind(2);
                shader->PushUniform("texture_normal", 2);
                shader->PushUniform("using_color_normal", 0);
            }
            else
            {
                shader->PushUniform("texture_normal", 2);
                shader->PushUniform("using_color_normal", 1);
                shader->PushUniform("color_normal", glm::vec4(MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<glm::vec4>(MaterialBase::MaterialParameter::Normal)));
            }
            if (metallic != nullptr && !MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->usingMetallicColor())
            {
                metallic->Bind(3);
                shader->PushUniform("texture_height", 3);
                shader->PushUniform("using_color_height", 0);
            }
            else
            {
                shader->PushUniform("texture_height", 3);
                shader->PushUniform("using_color_height", 1);
                shader->PushUniform("color_height", glm::vec4(MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<glm::vec4>(MaterialBase::MaterialParameter::Metallic)));
            }

            glActiveTexture(GL_TEXTURE4);
            shader->PushUniform("skybox", 4);

            if (mPosition != pos)
            {
                mPosition = pos;
                mCubemapIndex = ResourceManager::Get().getNearestCubemapIndex(pos);
                mGIIndex = ResourceManager::Get().NearestCubeMap(pos) - 1;
            }

            if (finalCubemaps)
                glBindTexture(GL_TEXTURE_CUBE_MAP, ResourceManager::Get().useCubeMap(mCubemapIndex));

            shader->PushUniform("AmbientProbeID", (int)mGIIndex);

            /// Draw mesh
            //glBindVertexArray(this->VAO);
            mVAO->Bind();
            glCache::glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
            //glBindVertexArray(0);
            mVAO->Unbind();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE4);

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    }

    void Mesh::setupMesh()
    {
        using Tex2D_ptr = std::shared_ptr<Texture2D>;

        auto &ref = ResourceManager::Get();
        for (int j = 0; j < 4; ++j)
        {
            ResourceManager::Get().addTextureToQueue(textures[j].path);
        }

        mVAO = std::make_shared<API::OpenGL::VertexArrayObject>();

        mVAO->addBuffer(vertices.size() * sizeof(t_Vertex), &vertices[0], GL_STATIC_DRAW);
        mVAO->setAttribute(VERTEX_POINTER_INDEX::POSITION, 3, sizeof(t_Vertex), (GLvoid *)0);
        mVAO->setAttribute(VERTEX_POINTER_INDEX::UV, 2, sizeof(t_Vertex), (void *)offsetof(t_Vertex, texcoord));
        mVAO->setAttribute(VERTEX_POINTER_INDEX::NORMAL, 3, sizeof(t_Vertex), (void *)offsetof(t_Vertex, normal));
        mVAO->setAttribute(VERTEX_POINTER_INDEX::TANGENT, 3, sizeof(t_Vertex), (void *)offsetof(t_Vertex, tangent));
        mVAO->setAttribute(VERTEX_POINTER_INDEX::BITANGENT, 3, sizeof(t_Vertex), (void *)offsetof(t_Vertex, bitangent));

        mVAO->IndexBuffer(indices);

        MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->setMaterial(MaterialBase::MaterialParameter::Albedo, ref.Get<Tex2D_ptr>(textures[0].path));
        MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->setMaterial(MaterialBase::MaterialParameter::Roughness, ref.Get<Tex2D_ptr>(textures[1].path));
        MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->setMaterial(MaterialBase::MaterialParameter::Normal, ref.Get<Tex2D_ptr>(textures[2].path));
        MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->setMaterial(MaterialBase::MaterialParameter::Metallic, ref.Get<Tex2D_ptr>(textures[3].path));

        isSettedUp = true;
    }
/*
    void Mesh::setMaterial(MaterialPBR_ptr material)
    {
        mMaterial = material;
    }*/

    void Mesh::Destroy()
    {
        using Tex2D_ptr = std::shared_ptr<Texture2D>;
        mVAO->Destroy();

        auto albedo = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Albedo);
        auto roughness = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Roughness);
        auto metallic = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Metallic);
        auto normal = MaterialManager::Get().getMaterial<MaterialPBR>(mMaterial)->get<Tex2D_ptr>(MaterialBase::MaterialParameter::Normal);

        albedo->decreaseRefCount();
        roughness->decreaseRefCount();
        metallic->decreaseRefCount();
        normal->decreaseRefCount();
    }
} // namespace Epsilon