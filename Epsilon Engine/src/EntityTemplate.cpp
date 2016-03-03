#include <EntityTemplate.h>

EntityTemplate::EntityTemplate()
{

}


void EntityTemplate::Update()
{
    if(this->hasPlayerComponent){
            pC.Update();
    }

    if(this->hasRenderComponent){
            rC.Update();
            std::cout << "hasRenderComponent" << std::endl;
    }

    if(this->hasSpatialComponent){
            sC.Update();
    }
}
