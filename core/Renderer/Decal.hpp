#pragma once

#include <ResourceManager.h>

#include <Renderer/Drawable.hpp>
#include <Renderer/MaterialPBR.hpp>
#include <Primitives/Cube.hpp>

#include <Renderer/MaterialManager.hpp>

namespace Epsilon
{
    namespace Renderer
    {
        class Decal : public Drawable
        {
            using Material_ptr = std::shared_ptr<MaterialPBR>;

        public:
            Decal()
            {
                mCube = std::make_shared<Cube<double>>(1.0, 1.0, 1.0);
            }

            void Update() override
            {
            }

            void Render() override
            {
                mCube->Render();
            }

        private:
            Cube<double>::Cube_ptr mCube;
            MaterialManager::Material_id mMaterial;
        };
    } // namespace Renderer
} //namespace Epsilon