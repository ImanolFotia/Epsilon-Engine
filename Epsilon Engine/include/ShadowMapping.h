///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose: Produce Shadows by a Framebuffer depth texture sampler
///
///=============================================================================

#ifndef SHADOWMAPPING_H_INCLUDED
#define SHADOWMAPPING_H_INCLUDED

#include <Shader.h>
#include <Types.h>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ShadowMap
{

public:
    /**
        Constructor:
            In the first two arguments takes in the width and height of the shadow map texture
            The third and fourth argument are used for the shadow map bounding box near and far planes
    **/
    ShadowMap(GLfloat width /*! Width of the shadow map texture*/,
              GLfloat height /*! Height of the shadow map texture*/,
              GLfloat near /*! Shadow map bounding box near plane*/,
              GLfloat far /*! Shadow map bounding box far plane*/ )
        :

        m_SHADOW_WIDTH(width),
        m_SHADOW_HEIGHT(height),
        m_NEAR_SHADOW_PLANE(near),
        m_FAR_SHADOW_PLANE(far)

    {
        this->SetupShadowMap();
    }

    /**
        Destructor
    **/
    virtual ~ShadowMap()
    {
        //std::cout << "ShadowMap Destroyed" << std::endl;
    }

public:

    /**
        Binds the Shadow Map framebuffer as the current framebuffer, where the depth of the scene will be rendered to.
    */
    void BindShadowFrameBuffer(void)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, this->m_ShadowFBO);
        glViewport(0, 0,  this->m_SHADOW_WIDTH, this->m_SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    /**
        Binds the current framebuffer to the default framebuffer
    */
    void UnbindShadowFrameBuffer(void)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
        Setups a framebuffer for blurring the shadow map image to be used in variance shadow mapping
    */
    void SetupPingPongFBO();

    /**
        Set the Shadow Map Position
    **/

    void setShadowPosition(glm::vec3 newPos)
    {
        this->m_POSITION = newPos;
    }

    /**
        Set the Shadow Map Direction
    **/

    void setShadowDirection(glm::vec3 newDir)
    {
        this->m_DIRECTION = newDir;
    }

    /**
        Get the Shadow Map texture Id where the depth from the scene was rendered to.
    **/

    GLuint getShadowTextureID(void)
    {
        return this->m_ShadowTexture;
    }

    /**
        Get the Shadow Map LightSpace ProjectionView Matrix
    **/

    glm::mat4 getLightSpaceMatrix(void)
    {
        return m_lightSpaceMatrix;
    }

    /**
        Get the Light/Proj Matrix multiplied by the Bias Matrix
    **/

    glm::mat4 getBiasMatrix(void)
    {
        return m_biasMatrix;
    }

    /**
        Setups the matrices from where the light will be calculated
    */

    void SetupShadowMatrices(void);

private:

    /**
        Setups the OpenGL related stuff of the Shadowmap
    */
    void SetupShadowMap(void);

private:

    GLuint m_ShadowTexture; /*!< Texture where the depth will be rendered to. */
    GLuint m_ShadowFBO; /*!< Framebuffer object used to created the shadow texture */

    GLuint pingpongFBO[2];
    GLuint pingpongDepthbuffers[2];

    GLfloat m_SHADOW_WIDTH; /*!< Shadow map texture width */
    GLfloat m_SHADOW_HEIGHT; /*!< Shadow map texture height */

    GLfloat m_NEAR_SHADOW_PLANE; /*!< Near plane of the shadow map bounding box */
    GLfloat m_FAR_SHADOW_PLANE; /*!< Far plane of the shadow map bounding box */

    glm::vec3 m_POSITION;  /*!< Position of the light in World Space */
    glm::vec3 m_DIRECTION; /*!< Direction of the light */

    glm::mat4 m_lightProjection;
    glm::mat4 m_lightView;
    glm::mat4 m_lightSpaceMatrix;
    glm::mat4 m_biasMatrix;

    //std::unique_ptr<Shader> blurShader;
};
#endif // SHADOWMAPPING_H_INCLUDED
