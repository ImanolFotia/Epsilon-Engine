#pragma once

#include <Entity/Component.h>
#include <Physics/ClothPhysicObject.h>
#include <Patch.h>

namespace Component {
    
class ClothComponent : public Component
{
public:
    ClothComponent(std::shared_ptr<Camera> &inPointerToCamera, std::shared_ptr<Physics::PhysicObject> PhysicBodyPointer, std::shared_ptr<ResourceManager> rm)
    {
        mResourceManager = rm;
        PointerToCamera = inPointerToCamera;
        updateIfOutOfView = false;
        mType = CLOTHCOMPONENT;
        
        std::shared_ptr<Physics::ClothPhysicObject> cloth = std::static_pointer_cast<Physics::ClothPhysicObject>(PhysicBodyPointer);
        mPatch = (std::shared_ptr<Patch>)new Patch(glm::vec3(0.0), cloth->getScale(), cloth->getWidth(), cloth->getHeight(), "sponza/sponza_curtain_diff.tga");
        RigidBodyPointer = PhysicBodyPointer;
    }

    COMPONENT_TYPE getType() { return mType; }

    ~ClothComponent()
    {
        std::cout << "ClothComponent Destructor" << std::endl;
    }

public:

    void Render()
    {
        mPatch->Render(mResourceManager->useShader(shaderType), PointerToCamera->getViewMatrix(), PointerToCamera->getProjectionMatrix());
    }

    virtual void RenderShadows()
    {
        mPatch->RenderShadows();
    }

    void Update()
    {
        //std::cout << "Llega Update cloth" <<std::endl;
        std::shared_ptr<Physics::ClothPhysicObject> cloth = std::static_pointer_cast<Physics::ClothPhysicObject>(RigidBodyPointer);
        mPatch->updateVertexBuffers(cloth->getVertices());
    }

    void setShader(std::string sh)
    {
        shaderType = sh;
    }

    std::shared_ptr<Patch> mPatch;
    std::shared_ptr<Physics::PhysicObject> RigidBodyPointer = nullptr;
    std::shared_ptr<Camera> PointerToCamera;
    bool updateIfOutOfView;
    virtual void setTransparency(bool x) {}
    std::string shaderType;
    COMPONENT_TYPE mType;
    std::shared_ptr<ResourceManager> mResourceManager;
};
}