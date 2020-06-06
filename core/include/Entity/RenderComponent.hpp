#pragma once

#include <Entity/Component.h>

#include <Model.h>

namespace Component
{
/// Rendering Components
class RenderComponent : public Component
{
public:
    RenderComponent(std::string path, glm::vec3 pos, std::string shader) 
    : modelPath(path), mPosition(pos), shaderType(shader)
    {
        mType = RENDERCOMPONENT;
        hasModel = true;
        ResourceManager::Get().requestModel(modelPath, glm::vec3(0), glm::vec3(1), glm::quat(0, 0, 0, 0));
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
        return glm::vec3(1., 1., 1.);
    }

    void setPosition(glm::vec3 pos) {
        mPosition = pos;
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
        //glUniform1i(glGetUniformLocation(mResourceManager->getShader(shaderType).get(), "isTransparent"), this->isTransparent);
        ResourceManager::Get().useShader(shaderType)->PushUniform("isTransparent", this->isTransparent);
        if (isDoubleFaced())
            glDisable(GL_CULL_FACE);
        ResourceManager::Get().useModel(modelPath, ResourceManager::Get().useShader(shaderType).get(), mPosition);
        
        glEnable(GL_CULL_FACE);
    }
    
    void RenderShadows()
    {
        ResourceManager::Get().useModel(modelPath, ResourceManager::Get().useShader(shaderType).get(), mPosition);
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

    void CastsShadows(bool x) {
        mCastsShadows = x;
    }

    bool CastsShadows() {
        return mCastsShadows;
    }

    void setTransparency(bool x) { isTransparent = x; }

    bool hasModel = false;
    COMPONENT_TYPE mType;
    std::string modelPath;
    std::string shaderType;
    MODEL_TYPE ModelType;
    bool isTransparent;
    bool mIsDoubleFaced;
    bool mCastsShadows;
    glm::vec3 mPosition;
};

using RenderComponent_ptr = std::shared_ptr<RenderComponent>;
} // namespace Component