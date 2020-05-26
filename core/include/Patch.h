#pragma once

#include <memory>

#include <Physics/PhysicObject.h>
#include <Shader.h>
#include <Texture.h>

class Patch
{
public:
    /**
    * Constructor taking parameters for scale, width and height of the patch, and the corresponding texture
    */
    Patch(glm::vec3 position, float s, int w, int h, std::string texturePath);

    virtual ~Patch(){}

    /**
    * In case the vertex attributes change (Cloth simulation, etc), this function will update them
    * Takes an array of vertices as input
    */
    void updateVertexBuffers(std::vector<Physics::PhysicObject::t_ClothVertex>);

    /**
    * Function to generate patch geometry data
    */
    void generateGeometry();
    /**
    * Render function
    */
    void Render(Shader* inShader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    /**
    * Render function
    */
    void Render(Shader inShader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
    /**
    * Render function
    */
    void RenderShadows();

    /**
    * Destroy all current textures and buffers
    */
    void Destroy();

private:
    /**
    * Set up VAO And VBO for the patch
    * VBO is configured as a streaming vertex buffer
    */
    void setupVertexBuffers();

private:
    std::shared_ptr<eTexture> mTexture;
    std::shared_ptr<eTexture> mNormalTexture;
    std::shared_ptr<eTexture> mRoughtnessTexture;
    std::shared_ptr<eTexture> mMetallicTexture;

    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mTexCoords;
    std::vector<unsigned int> mIndices;

    glm::vec3 mPosition;
    int mWidth;
    int mHeight;
    float mScale;

    GLuint VAO, VBO, NBO, EBO, UVBO;

};
