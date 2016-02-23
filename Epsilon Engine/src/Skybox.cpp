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

using namespace std;

Skybox::Skybox(string SkyboxTex)
{

    eTexture tex("Cliff_height.png");

    CubeMapID = tex.getTextureID();


    skydome = new Model("models/sphere.obj");

}

void Skybox::Render(Camera* & camera, Shader* SkyShader)
{

    glDepthFunc(GL_LEQUAL);
    SkyShader->Use();

    glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix()));
    float rotation = 0.5 * glfwGetTime();
    glm::mat4 RotationMatrix = glm::rotate(glm::mat4(),glm::radians(rotation), glm::vec3(0,1,0));
    view = view * RotationMatrix;
    glm::mat4 projection = glm::mat4(camera->getProjectionMatrix());
    glm::mat4 model = glm::mat4();

    glm::mat4 ScaleMatrix = glm::scale(model, glm::vec3(1,1,1));
    glm::mat4 TranslationMatrix = glm::translate(model, glm::vec3(0,0,0));
    model = model * ScaleMatrix * TranslationMatrix;
    glUniformMatrix4fv(glGetUniformLocation(SkyShader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(SkyShader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(SkyShader->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform1f(glGetUniformLocation(SkyShader->getProgramID(), "time"), glfwGetTime());
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(SkyShader->getProgramID(), "tex"), 0);
    glBindTexture(GL_TEXTURE_2D, CubeMapID);
    skydome->Draw(SkyShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glDepthFunc(GL_LESS);
}
