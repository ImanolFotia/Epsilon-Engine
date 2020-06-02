#pragma once 

#include <ResourceManager.h>

#include <RenderObject.hpp>
#include <Primitives/Cube.hpp>

namespace Epsilon {
class Decal : public RenderObject {
    public:
        Decal() {
        }

        void Update() override {
            
        }

        void Render() override {
            mCube.Render();
        }

    private:
        Cube mCube;
};
} //namespace Epsilon