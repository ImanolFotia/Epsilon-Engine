///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <pch.hpp>
#include <Skybox.h>
#include <SkyboxData.h>
#include <Resource/ResourceManager.h>

#include <glm/glm.hpp>

namespace Epsilon
{

    Skybox::Skybox(std::string SkyboxTex)
    {
        mSphere = std::make_shared<Renderer::Sphere>(10);

        clouds = false;
        UpperSky_color = glm::vec3(29,91,0) / 255.0f; 
        LowerSky_color = glm::vec3(125, 228, 247) / 255.0f;
        Horizon_color = glm::vec3(1,1,1) / 255.0f;
        Horizon_Height = 0.276f;
    }

    void Skybox::Render(std::shared_ptr<Camera> &camera, std::shared_ptr<Shader> SkyShader, float exposure, bool state)
    {
        glDepthFunc(GL_LEQUAL);
        SkyShader->Use();
        glUniform1f(glGetUniformLocation(SkyShader->getProgramID(), "exposure"), exposure);
        glUniform1f(glGetUniformLocation(SkyShader->getProgramID(), "time"), glfwGetTime());
        glUniform1i(glGetUniformLocation(SkyShader->getProgramID(), "renderDepth"), state);

        SkyShader->PushUniform("UpperSkyColor", UpperSky_color);
        SkyShader->PushUniform("LowerSkyColor", LowerSky_color);
        SkyShader->PushUniform("HorizonColor", Horizon_color);
        SkyShader->PushUniform("RenderClouds", clouds); 
        SkyShader->PushUniform("HorizonHeight", Horizon_Height);
        SkyShader->PushUniform("cameraPosition", camera->getPosition());
        mSphere->Render();
        glCache::glUseProgram(0);
        glDepthFunc(GL_LESS);
    }
} // namespace Epsilon
