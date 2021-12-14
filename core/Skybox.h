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
#include <Renderer/Primitives/Sphere.hpp>

namespace Epsilon
{
    class Skybox
    {
    public:
        Skybox(std::string);
        /* virtual ~Skybox()
        {
            //std::cout << "SkyBox Destroyed" << std::endl;
        }*/

    public:
        void Render(std::shared_ptr<Camera> &, std::shared_ptr<Shader>, float, bool);

        bool clouds = false;
        glm::vec3 UpperSky_color = glm::vec3(29,91,255); 
        glm::vec3 LowerSky_color = glm::vec3(125, 228, 247);
        glm::vec3 Horizon_color = glm::vec3(0.001,0.001,0.001);
        float Horizon_Height = 0.276f;

    private:
        
        std::shared_ptr<Renderer::Sphere> mSphere;
    };
} // namespace Epsilon
#endif /// SKYBOX_H_INCLUDED
