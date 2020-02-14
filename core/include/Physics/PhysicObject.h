#pragma once
#include <Physics/Physics.h>


namespace Physics {
    class PhysicObject {
    public:
        struct t_ClothVertex {
            btVector3 position;
            btVector3 normal;
        };

        struct t_Face{
            t_Vertex vextex0;
            t_Vertex vextex1;
            t_Vertex vextex2;
        };

    public:
        PhysicObject() = default;
        virtual ~PhysicObject() {}

    public:
        virtual btRigidBody_ptr addObject(glm::vec3 /* Position */, float /* Mass */, float /* Size */) {return nullptr;}
        virtual btRigidBody_ptr addObject(float /* Position X */, float /* Position Y */, float /* Position Z */, float /* Mass */, float /* Size*/) {return nullptr;}
        virtual btRigidBody_ptr addObject(float /* Radius */, float /* Mass */) {return nullptr;}
        virtual btRigidBody_ptr addObject(glm::vec3 /* Position */, float /* Mass */, MIN_MAX_POINTS /* Bounding Box */, float /* Scale*/) {return nullptr;}
        virtual btRigidBody_ptr addObject(std::vector<glm::vec3> /* Vertices */, std::vector<unsigned int> /* Indices */, float) {return nullptr;}
        btRigidBody_ptr Body;
    };
}

#include <Physics/ClothPhysicObject.h>
#include <Physics/CubePhysicObject.h>
#include <Physics/SpherePhysicObject.h>
#include <Physics/TriangleMeshPhysicObject.h>
