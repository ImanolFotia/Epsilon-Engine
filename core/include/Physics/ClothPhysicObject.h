#pragma once
#include <Physics/PhysicObject.h>
#include <Physics/CollisionInfo.h>

namespace Physics
{

class ClothPhysicObject : public PhysicObject
{
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

    const std::vector<PhysicObject::t_ClothVertex>& getVertices();

    float getScale();
    int getWidth();
    int getHeight();

    void setWind(btVector3 direction, float speed);

    void setconstraint(int x, int y);

    std::shared_ptr<btSoftBody> m_BodyCloth;
    CollisionInfo CollInfo;

private:
    std::vector<PhysicObject::t_ClothVertex> mVertices;
    float mScale;
    int mWidth;
    int mHeight;
};
} // namespace Physics
