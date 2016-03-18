#ifndef ENTITYTEMPLATE_H_INCLUDED
#define ENTITYTEMPLATE_H_INCLUDED

#include <Component.h>
#include <iostream>
#include <typeinfo>
#include <memory>
class EntityTemplate
{
public:
    EntityTemplate();

    ~EntityTemplate()
    {
        /*
        delete rC;
        rC = nullptr;

        delete pC;
        pC = nullptr;

        delete sC;
        sC = nullptr;
*/
        std::cout << "Entity Destructor" << std::endl;
    }

    void addComponent(bool t)
    {
        std::cout << "isBool" << std::endl;
    }

    void addComponent(std::shared_ptr<Component::RenderComponent> t)
    {
        std::cout <<typeid(Component::RenderComponent).name() << " Added." << std::endl;
        hasRenderComponent = true;
        rC = t;
    }

    void addComponent(std::shared_ptr<Component::PlayerComponent> t)
    {
        std::cout << "is" <<typeid(Component::PlayerComponent).name() << std::endl;
        hasPlayerComponent = true;
    }

    void addComponent(std::shared_ptr<Component::SpatialComponent> t)
    {
        std::cout << "is" <<typeid(Component::SpatialComponent).name() << std::endl;
        hasSpatialComponent = true;
        sC = t;
    }


    void Update();

    std::shared_ptr<Component::RenderComponent> rC;
    std::shared_ptr<Component::PlayerComponent> pC;
    std::shared_ptr<Component::SpatialComponent> sC;

private:

    bool hasPlayerComponent = false;
    bool hasRenderComponent = false;
    bool hasSpatialComponent = false;



protected:

};

#endif // ENTITYTEMPLATE_H_INCLUDED
