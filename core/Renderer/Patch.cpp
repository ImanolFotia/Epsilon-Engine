#include "Patch.h"
#include <Resource/ResourceManager.h>


namespace Epsilon
{
    Patch::Patch(glm::vec3 position, float s, int w, int h, const std::string& texturePath)
    {
        using Renderer::Texture2D;
        
        API::Texture::TextureData TextureData;
        TextureData.MakeDefaultGL();
        TextureData.Compressed = false;
        TextureData.SRGB = false;

        mScale = s;
        mWidth = w;
        mHeight = h;
        mPosition = position;
        
        std::string path_suffix = "materials/";

        int outwidth = 0, outheight = 0, outchannels = 0;
        unsigned char* data = stbi_load("materials/old-soiled-cloth1-albedo.png", &outwidth, &outheight, &outchannels, 4);
        mTexture = std::make_shared<Texture2D>();
        mTexture->Create(outwidth, outheight);
        mTexture->setData(data, 0);
        stbi_image_free(data);
        data = nullptr;

        data = stbi_load(std::string(path_suffix + "old-soiled-cloth1-Normal-dx.png").c_str(), &outwidth, &outheight, &outchannels, 4);
        TextureData.Width = outwidth;
        TextureData.Height = outheight;
        mNormalTexture = std::make_shared<Texture2D>();
        mNormalTexture->Create(TextureData);
        mNormalTexture->setData(data, 0);
        stbi_image_free(data);
        data = nullptr;

        data = stbi_load(std::string(path_suffix + "old-soiled-cloth1-Roughness.png").c_str(), &outwidth, &outheight, &outchannels, 4);
        TextureData.Width = outwidth;
        TextureData.Height = outheight;
        mRoughtnessTexture = std::make_shared<Texture2D>();
        mRoughtnessTexture->Create(TextureData);
        mRoughtnessTexture->setData(data, 0);
        stbi_image_free(data);
        data = nullptr; 

        data = stbi_load(std::string(path_suffix + "old-soiled-cloth1-Metallic.png").c_str(), &outwidth, &outheight, &outchannels, 4);
        TextureData.Width = outwidth;
        TextureData.Height = outheight;
        mMetallicTexture = std::make_shared<Texture2D>();
        mMetallicTexture->Create(TextureData);
        mMetallicTexture->setData(data, 0);
        stbi_image_free(data);
        data = nullptr;

        generateGeometry();
        setupVertexBuffers();
    }

    void Patch::Destroy()
    {
        glDeleteBuffers(1, &this->VBO);
        glDeleteBuffers(1, &this->NBO);
        glDeleteBuffers(1, &this->UVBO);
        glDeleteBuffers(1, &this->EBO);
        glDeleteVertexArrays(1, &this->VAO);

        mTexture->Destroy();
        mNormalTexture->Destroy();
        mRoughtnessTexture->Destroy();
        mMetallicTexture->Destroy();
    }

    void Patch::Render(Shader *inShader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
    {

        glDisable(GL_CULL_FACE);
        inShader->Use();
        glm::mat4 model = glm::mat4(1.0); //glm::translate(glm::mat4(1.0), this->mPosition);
        glm::mat4 MVP = projectionMatrix * viewMatrix * model;
        glBindVertexArray(this->VAO);
        inShader->PushUniform("MVP", MVP);
        inShader->PushUniform("Model", model);
        inShader->PushUniform("view", viewMatrix);

        //glActiveTexture();
        mTexture->Bind(0);
        inShader->PushUniform("DiffuseSampler", 0);

        //glActiveTexture();
        mNormalTexture->Bind(1);
        inShader->PushUniform("NormalSampler", 1);
        
        mRoughtnessTexture->Bind(2);
        inShader->PushUniform("RoughnessSampler", 2);
        
        mMetallicTexture->Bind(3);
        inShader->PushUniform("MetallicSampler", 3);
        
        glActiveTexture(GL_TEXTURE4);
        inShader->PushUniform("skybox", 4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ResourceManager::Get().useCubeMap(ResourceManager::Get().getNearestCubemapIndex(mPosition)));
        inShader->PushUniform("AmbientProbeID", ResourceManager::Get().NearestCubeMap(mPosition) - 1);

        glCache::glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);

        inShader->Free();
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);

        mTexture->Unbind();
        mNormalTexture->Unbind();
        mRoughtnessTexture->Unbind();
        mMetallicTexture->Unbind();
    }

    void Patch::RenderShadows()
    {
        glDisable(GL_CULL_FACE);
        glBindVertexArray(this->VAO);
        glCache::glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);
        glEnable(GL_CULL_FACE);
        glBindVertexArray(0);
    }

    void Patch::Render(Shader inShader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
    {

        glDisable(GL_CULL_FACE);
        inShader.Use();
        glm::mat4 model = glm::mat4(1.0); //glm::translate(glm::mat4(1.0), this->mPosition);
        glm::mat4 MVP = projectionMatrix * viewMatrix * model;
        glBindVertexArray(this->VAO);
        inShader.PushUniform("MVP", MVP);
        inShader.PushUniform("Model", model);

        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, this->mTexture->getTextureID());
        mTexture->Bind();
        inShader.PushUniform("DiffuseSampler", 0);

        glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, this->mNormalTexture->getTextureID());
        mNormalTexture->Bind();
        inShader.PushUniform("NormalSampler", 1);

        glCache::glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);

        inShader.Free();
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }

    void Patch::updateVertexBuffers(std::vector<Physics::PhysicObject::t_ClothVertex> inVertices)
    {

        for (auto i = 0; i < inVertices.size(); i++)
        {
            glm::vec3 pos = glm::vec3(inVertices[i].position.x(), inVertices[i].position.y(), inVertices[i].position.z());
            glm::vec3 nor = glm::vec3(inVertices[i].normal.x(), inVertices[i].normal.y(), inVertices[i].normal.z());
            mVertices.at(i) = pos;
            mNormals.at(i) = nor;
        }

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * inVertices.size(), &mVertices[0]);
        glBindBuffer(GL_ARRAY_BUFFER, this->NBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * inVertices.size(), &mNormals[0]);

        glBindVertexArray(0);
    }

    void Patch::generateGeometry()
    {
        int numVertices = this->mWidth * this->mHeight;

        float x = 0;
        float y = 0;

        for (int i = 0; i < numVertices; ++i)
        {
            x = i % mWidth;
            y = glm::floor(i / mWidth);

            float s = x / (this->mWidth - 1);
            float t = y / (this->mHeight - 1);

            this->mVertices.push_back(glm::vec3(x, y, 0));
            this->mTexCoords.push_back(glm::vec2(s, t));
            this->mNormals.push_back(glm::vec3(0, 0, 1));
        }

        int counter = 0;
        for (int i = 0; i < (this->mWidth * this->mHeight) - mWidth; i++)
        {
            if (counter == this->mWidth - 1)
            {
                counter = 0;
                continue;
            }

            this->mIndices.push_back(i + 1);
            this->mIndices.push_back(i + this->mWidth);
            this->mIndices.push_back(i + this->mWidth - this->mWidth);
            this->mIndices.push_back(i + 1);
            this->mIndices.push_back(i + this->mWidth + 1);
            this->mIndices.push_back(i + this->mWidth);

            counter++;
        }

        std::cout << "Created new patch" << std::endl;
        std::cout << "\tVertices: " << this->mVertices.size() << std::endl;
        std::cout << "\tIndices: " << this->mIndices.size() << std::endl;
    }

    void Patch::setupVertexBuffers()
    {
        /// Create buffers/arrays
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->NBO);
        glGenBuffers(1, &this->UVBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);
        /// Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->mVertices.size() * sizeof(glm::vec3), &this->mVertices[0], GL_DYNAMIC_DRAW );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);

        glBindBuffer(GL_ARRAY_BUFFER, this->UVBO);
        glBufferData(GL_ARRAY_BUFFER, this->mTexCoords.size() * sizeof(glm::vec2), &this->mTexCoords[0], GL_DYNAMIC_DRAW );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid *)0);

        glBindBuffer(GL_ARRAY_BUFFER, this->NBO);
        glBufferData(GL_ARRAY_BUFFER, this->mNormals.size() * sizeof(glm::vec3), &this->mNormals[0], GL_DYNAMIC_DRAW );
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mIndices.size() * sizeof(unsigned int), &this->mIndices[0], GL_DYNAMIC_DRAW );

        glBindVertexArray(0);
    }
} // namespace Epsilon