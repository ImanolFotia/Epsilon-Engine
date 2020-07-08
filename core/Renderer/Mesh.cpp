#include "Mesh.h"
#include <ResourceManager.h>

#include <IO/IO.hpp>

namespace Epsilon
{
    /// Render the mesh
    void Mesh::Draw(std::shared_ptr<Shader> shader, glm::vec3 pos)
    {
        if(ResourceManager::Get().cubemapsLoaded && finalCubemaps != true)
        {
            finalCubemaps = true;
            mCubemapIndex = ResourceManager::Get().getNearestCubemapIndex(pos);
            mGIIndex = ResourceManager::Get().NearestCubeMap(pos) - 1;
            IO::PrintLine("Cubemap index registered: ", mGIIndex, " at position: ", pos.x, pos.y, pos.z);
        }

        if (isVisible)
        {

            glActiveTexture(GL_TEXTURE0);
            shader->PushUniform("texture_diffuse", 0);
            ResourceManager::Get().bindTexture(textures[0].path);

            glActiveTexture(GL_TEXTURE1);
            shader->PushUniform("texture_specular", 1);
            ResourceManager::Get().bindTexture(textures[1].path);

            glActiveTexture(GL_TEXTURE2);
            shader->PushUniform("texture_normal", 2);
            ResourceManager::Get().bindTexture(textures[2].path);

            glActiveTexture(GL_TEXTURE3);
            shader->PushUniform("texture_height", 3);
            ResourceManager::Get().bindTexture(textures[3].path);

            glActiveTexture(GL_TEXTURE4);
            shader->PushUniform("skybox", 4);
/*
            mCubemapIndex = ResourceManager::Get().getNearestCubemapIndex(pos);
            mGIIndex = ResourceManager::Get().NearestCubeMap(pos) - 1;*/
            
            //mCubemapIndex = ResourceManager::Get().getNearestCubemapIndex(pos);
            //mGIIndex = ResourceManager::Get().NearestCubeMap(pos) - 1;
            

            if(finalCubemaps)
                glBindTexture(GL_TEXTURE_CUBE_MAP, ResourceManager::Get().useCubeMap(mCubemapIndex));

            shader->PushUniform("AmbientProbeID", (int)mGIIndex);

            /// Draw mesh
            glBindVertexArray(this->VAO);
            glCache::glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

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
} // namespace Epsilon