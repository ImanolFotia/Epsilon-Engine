///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <Skybox.h>
#include <SkyboxData.h>
#include <ResourceManager.h>
using namespace std;

Skybox::Skybox(string SkyboxTex)
{
    skydome = (std::unique_ptr<Model>)new Model("models/sphere.eml");

}

void Skybox::Render(std::shared_ptr<Camera> & camera, Shader* SkyShader, float exposure, bool state)
{
    glDepthFunc(GL_LEQUAL);
    SkyShader->Use();
    glUniform1f(glGetUniformLocation(SkyShader->getProgramID(), "exposure"), exposure);
    glUniform1f(glGetUniformLocation(SkyShader->getProgramID(), "time"), glfwGetTime());
    glUniform1i(glGetUniformLocation(SkyShader->getProgramID(), "renderDepth"), state);
    skydome->DrawNoTexture();
    glUseProgram(0);
    glDepthFunc(GL_LESS);
}
