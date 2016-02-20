#include <ShadowMapping.h>

void ShadowMap::SetupShadowMap()
{
    glGenFramebuffers(1, &this->m_ShadowFBO);
    // - Create depth texture
    glGenTextures(1, &m_ShadowTexture);
    glBindTexture(GL_TEXTURE_2D, m_ShadowTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->m_SHADOW_WIDTH , this->m_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, this->m_ShadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAMEBUFFER);

}

void ShadowMap::SetupShadowMatrices()
{
        m_lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_NEAR_SHADOW_PLANE, m_FAR_SHADOW_PLANE);
        m_lightView = glm::lookAt(m_POSITION, m_POSITION + m_DIRECTION, glm::vec3(1.0));
        m_lightSpaceMatrix = m_lightProjection * m_lightView;
}
