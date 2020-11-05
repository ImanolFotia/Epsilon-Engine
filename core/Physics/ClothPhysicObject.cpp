
#include <pch.hpp>
#include <Physics/ClothPhysicObject.h>

namespace Epsilon
{
    namespace Physics
    {
        std::shared_ptr<btSoftBody> ClothPhysicObject::addObject(std::shared_ptr<btSoftBodyWorldInfo> softWorldInfo,
                                                                 const glm::vec3 position,
                                                                 const btScalar s,
                                                                 const int numX,
                                                                 const int numY,
                                                                 const int fixed)
        {

            try
            {
                this->m_BodyCloth = std::shared_ptr<btSoftBody>(btSoftBodyHelpers::CreatePatch(*softWorldInfo.get(),
                                                                                               btVector3(-s / 2, s, 0) + btVector3(position.x, position.y, position.z),
                                                                                               btVector3(+s / 2, s, 0) + btVector3(position.x, position.y, position.z),
                                                                                               btVector3(-s / 2, s, +s) + btVector3(position.x, position.y, position.z),
                                                                                               btVector3(+s / 2, s, +s) + btVector3(position.x, position.y, position.z),
                                                                                               numX, numY,
                                                                                               fixed, false));

                this->mScale = s;
                this->mWidth = numX;
                this->mHeight = numY;

                this->m_BodyCloth->getCollisionShape()->setMargin(0.05f);
                this->m_BodyCloth->generateBendingConstraints(2.0, this->m_BodyCloth->appendMaterial());
                this->m_BodyCloth->setTotalMass(5.0);
                this->m_BodyCloth->m_cfg.piterations = 5;
                this->m_BodyCloth->m_cfg.viterations = 5;
                this->m_BodyCloth->m_cfg.diterations = 5;
                this->m_BodyCloth->setFriction(0.0);
                this->m_BodyCloth->setWindVelocity(btVector3(5, -50.0, -60.0));
                this->m_BodyCloth->m_cfg.kLF = 0.05; //Feels lighter
                this->m_BodyCloth->m_cfg.kDG = 0.01; // High values act weird
                this->m_BodyCloth->m_cfg.kDF =	0.1; //Friction, get's sticky
                this->m_BodyCloth->m_cfg.kCHR = 1.0; //Low values makes it go though things
                this->m_BodyCloth->m_cfg.kMT = 1.0; 
                this->m_BodyCloth->m_cfg.kDP = 0.005f; //high values acts like slow motion
                this->m_BodyCloth->m_cfg.aeromodel = btSoftBody::eAeroModel::V_TwoSidedLiftDrag;
                this->m_BodyCloth->m_cfg.collisions |= btSoftBody::fCollision::SDF_RS;     
                this->m_BodyCloth->m_cfg.kVCF = 0.0;      
                this->m_BodyCloth->m_cfg.kKHR = 1.0;     
                this->m_BodyCloth->m_cfg.kSHR = 1.0;  
                this->m_BodyCloth->m_cfg.kVC = 0.0;
                //this->m_BodyCloth->m_cfg.kAHR = 0.0; //can't figure what this does
                //this->m_BodyCloth->setMass(5, 0);
                this->m_BodyCloth->randomizeConstraints();
                //this->m_BodyCloth->m_cfg.collisions	=	btSoftBody::fCollision::CL_SS+
                //                           btSoftBody::fCollision::CL_RS;
                this->m_BodyCloth->generateClusters(2);
                CollInfo.setName("Cloth");
                //btSoftBodyHelpers::ReoptimizeLinkOrder(this->m_BodyCloth.get());
                this->m_BodyCloth->setUserPointer(&CollInfo);
                std::cout << this->m_BodyCloth << std::endl;
                mVertices.resize(m_BodyCloth->m_nodes.size());
                return this->m_BodyCloth;
            }
            catch (std::exception e)
            {
                std::cout << "EXCEPTION OCURRED: " << e.what() << std::endl;
            }

            return nullptr;
        }

        const std::vector<PhysicObject::t_ClothVertex> &ClothPhysicObject::getVertices()
        {

            btSoftBody *cloth = (std::dynamic_pointer_cast<btSoftBody>(this->m_BodyCloth)).get();
            //cloth->addForce(cloth->getWindVelocity());
            //cloth->applyForces();

            btSoftBody::tNodeArray &nodes(cloth->m_nodes);
            //std::vector<PhysicObject::t_ClothVertex> outVertices;

            //std::cout << nodes.size() << std::endl;

            for (auto i = 0; i < nodes.size(); i++)
            {
                mVertices.at(i).position = nodes[i].m_x;
                mVertices.at(i).normal = nodes[i].m_n;
            }

            return mVertices;
        }

        void ClothPhysicObject::setWind(btVector3 direction, float speed)
        {
            btSoftBody *cloth = (std::dynamic_pointer_cast<btSoftBody>(this->m_BodyCloth)).get();
            cloth->setWindVelocity(direction * speed);
        }

        float ClothPhysicObject::getScale()
        {
            return mScale;
        }

        int ClothPhysicObject::getWidth()
        {
            return mWidth;
        }

        int ClothPhysicObject::getHeight()
        {
            return mHeight;
        }
    } // namespace Physics
} // namespace Epsilon