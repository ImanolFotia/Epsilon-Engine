#pragma once
#include <Physics/Physics.h>
#include <Physics/PhysicObject.h>
#include<Physics/CollisionInfo.h>

namespace Physics {

    class PhysicObject;

    class ClothPhysicObject : public PhysicObject {
    public:
        ClothPhysicObject() = default;
        virtual ~ClothPhysicObject() {}
    public:
        virtual std::shared_ptr<btSoftBody> addObject(std::shared_ptr<btSoftBodyWorldInfo> softWorldInfo,
                const glm::vec3 position,
                const btScalar s,
                const int numX,
                const int numY,
                const int fixed);

                std::vector<t_ClothVertex> getVertices();

                float getScale();
                int getWidth();
                int getHeight();

                void setWind(btVector3 direction, float speed);

                void setconstraint(int x, int y);

                std::shared_ptr<btSoftBody> m_BodyCloth;
                CollisionInfo CollInfo;

    private:
        float mScale;
        int mWidth;
        int mHeight;
    };
}
