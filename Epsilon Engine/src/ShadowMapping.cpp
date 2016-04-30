#include <ShadowMapping.h>
#include <iostream>
#include <cmath>

void ShadowMap::SetupShadowMap()
{
    glGenFramebuffers(1, &this->m_ShadowFBO);
    // - Create depth texture
    glGenTextures(1, &m_ShadowTexture);
    glBindTexture(GL_TEXTURE_2D, m_ShadowTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, this->m_SHADOW_WIDTH, this->m_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_ShadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::SetupShadowMatrices()
{

    m_lightProjection = glm::ortho(-65.0f, 65.0f, -65.0f, 65.0f, -20.0f, 80.0f);
    //glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.5f, 100.0f);
    //m_lightProjection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.5f, 100.0f);

    m_lightView = glm::lookAt(this->m_POSITION, this->m_POSITION - this->m_DIRECTION, glm::vec3(1.0));
    //m_lightView = glm::lookAt(this->m_POSITION, this->m_POSITION + this->m_DIRECTION, glm::vec3(1.0));
    m_lightSpaceMatrix = m_lightProjection * m_lightView;
}
/*
void ShadowMap::SetupPingPongFBO()
{
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongDepthbuffers);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongDepthbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 320, 240, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // Also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
}
*/
/*
GLuint ShadowMap::blurImage(GLuint Buffer)
{

    GLboolean horizontal = true, first_iteration = true;
    GLuint amount = 5;
    blurShader->Use();

    for(int i = 0 ; i < amount ; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glViewport(0,0,320, 240);
        glUniform1i(glGetUniformLocation(blurShader->getProgramID(), "horizontal"), horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? Buffer : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        RenderQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0,0,width, height);

    return pingpongColorbuffers[horizontal];
}
*/
