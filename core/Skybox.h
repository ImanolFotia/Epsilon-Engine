///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef SKYBOX_H_INCLUDED
#define SKYBOX_H_INCLUDED
#include <Core.hpp>
#include <camera.h>
#include <Shader.h>
#include <Renderer/Model.h>

namespace Epsilon
{
    class Skybox
    {
    public:
        Skybox(std::string);
        virtual ~Skybox()
        {
            //std::cout << "SkyBox Destroyed" << std::endl;
        }

    public:
        void Render(std::shared_ptr<Camera> &, std::shared_ptr<Shader>, float, bool);

    private:
        std::shared_ptr<Model> skydome;
    };
} // namespace Epsilon
#endif /// SKYBOX_H_INCLUDED
