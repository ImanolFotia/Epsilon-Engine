#pragma once

#include <Entity/Component.h>
#include <Physics/ClothPhysicObject.h>
#include <Patch.h>

#include <random>
namespace Epsilon
{
    namespace Component
    {

        class ClothComponent : public Component
        {
        public:
            ClothComponent(glm::vec3 pos, float sc, glm::quat rot, std::shared_ptr<Camera> inPointerToCamera)
            {
                PointerToCamera = inPointerToCamera;
                updateIfOutOfView = false;
                mType = CLOTHCOMPONENT;

                SoftBodyPointer = std::make_shared<Physics::ClothPhysicObject>();

                SoftBodyPointer->addObject(ResourceManager::Get().getPhysicsWorld()->softBodyWorldInfo, pos, sc, 30, 30, 1 + 2);
                mPatch = (std::shared_ptr<Patch>)new Patch(glm::vec3(0.0), SoftBodyPointer->getScale(), SoftBodyPointer->getWidth(), SoftBodyPointer->getHeight(), "cloth/worn-blue-burlap-albedo.png");

                ResourceManager::Get().getPhysicsWorld()->getSoftDynamicsWorld()->addSoftBody(SoftBodyPointer->m_BodyCloth.get());

                gen.seed(rd());
                std::uniform_real_distribution<> d(-1.0, 0.5);
                dis = d;
            }

            COMPONENT_TYPE getType() { return mType; }

            ~ClothComponent()
            {
                std::cout << "ClothComponent Destructor" << std::endl;
            }

        public:
            void Render()
            {
                mPatch->Render(ResourceManager::Get().useShader(shaderType).get(), PointerToCamera->getViewMatrix(), PointerToCamera->getProjectionMatrix());
            }

            virtual void RenderShadows()
            {
                mPatch->RenderShadows();
            }

            void Update()
            {
                double ts = ResourceManager::Get().getPhysicsWorld()->getTimeStep();
                double time = glfwGetTime();
                double intensity = glm::max((sin(time) * 0.5 + 0.5), 0.999);
                mLastUpdate += ts;
                if (mLastUpdate >= 1.5)
                {
                    btVector3 wVelocity = SoftBodyPointer->m_BodyCloth->getWindVelocity();
                    btVector3 rVector = btVector3(0.1, dis(gen), dis(gen));
                    btVector3 nVelocity = rVector + wVelocity;
                    wVelocity.lerp(nVelocity, 1.0);
                    SoftBodyPointer->m_BodyCloth->setWindVelocity(rVector * btVector3(30.0, 15.0, 40.0));
                }

                mPatch->updateVertexBuffers(SoftBodyPointer->getVertices());
            }

            void setShader(std::string sh)
            {
                shaderType = sh;
            }

            glm::vec3 getPosition()
            {
                return glm::vec3(0.0);
            }

            glm::vec3 getScale()
            {
                return glm::vec3(0.0);
            }

            glm::quat getRotation()
            {
                return glm::quat(0.0, 0.0, 0.0, 0.0);
            }

            std::shared_ptr<Patch> mPatch;
            std::shared_ptr<Physics::ClothPhysicObject> SoftBodyPointer = nullptr;
            std::shared_ptr<Camera> PointerToCamera;
            bool updateIfOutOfView;
            virtual void setTransparency(bool x) {}
            std::string shaderType;
            COMPONENT_TYPE mType;

        private:
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_real_distribution<> dis;

            float mLastUpdate = 0.0f;
        };
    } // namespace Component
} // namespace Epsilon