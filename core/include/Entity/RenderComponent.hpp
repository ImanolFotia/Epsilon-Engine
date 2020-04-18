#pragma once

#include <Entity/Component.h>

#include <Model.h>

namespace Component
{
/// Rendering Components
class RenderComponent : public Component
{
public:
    RenderComponent(std::string path, glm::vec3 pos, std::shared_ptr<ResourceManager> rm, std::string shader) 
    : modelPath(path), mPosition(pos), mResourceManager(rm), shaderType(shader)
    {
        mType = RENDERCOMPONENT;
        hasModel = true;
        mResourceManager->requestModel(modelPath, rm, glm::vec3(0), glm::vec3(1), glm::quat(0, 0, 0, 0));
    }

    COMPONENT_TYPE getType() { return mType; }
    ~RenderComponent()
    {
        std::cout << "RenderComponent Destructor" << std::endl;
    }

    
    virtual glm::vec3 getPosition(){
        return mPosition;
    };

    virtual glm::quat getRotation(){
        return glm::quat(1.0, 0.0, 0.0, 0.0);
    }

    virtual glm::vec3 getScale(){
        glm::vec3(1., 1., 1.);
    }

    std::string getModelPath() {
        return modelPath;
    }

public:
    void Update()
    {
    }

    void Render()
    {
        glUniform1i(glGetUniformLocation(mResourceManager->getShaderID(shaderType), "isTransparent"), this->isTransparent);

        if (isDoubleFaced())
            glDisable(GL_CULL_FACE);
        mResourceManager->useModel(modelPath, mResourceManager->getShaderID(shaderType), mPosition);
        
        glEnable(GL_CULL_FACE);
    }
    
    void RenderShadows()
    {
        mResourceManager->useModel(modelPath, mResourceManager->getShaderID(shaderType), mPosition);
    }

    void setShader(std::string sh)
    {
        shaderType = sh;
    }

    bool isDoubleFaced() {
        return mIsDoubleFaced;
    }

    void isDoubleFaced(bool x) {
        mIsDoubleFaced = x;
    }

    void setTransparency(bool x) { isTransparent = x; }

    bool hasModel = false;
    COMPONENT_TYPE mType;
    std::string modelPath;
    std::string shaderType;
    MODEL_TYPE ModelType;
    bool isTransparent;
    bool mIsDoubleFaced;
    std::shared_ptr<ResourceManager> mResourceManager;
    glm::vec3 mPosition;
};
} // namespace Component