#pragma once 

#include <ResourceManager.h>

#include <RenderObject.hpp>
#include <Primitives/Cube.hpp>

namespace Epsilon {
class Decal : public RenderObject {
    public:
        Decal() {
            mCube = std::make_shared<Cube<double>>(1.0, 1.0, 1.0);
        }

        void Update() override {
            
        }

        void Render() override {
            mCube->Render();
        }

    private:
        Cube<double>::Cube_ptr mCube;
};
} //namespace Epsilon