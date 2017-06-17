#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

#include <Texture.h>
#include <OpenGL/HelperFunctions/FullScreenQuad.h>
#include <OpenGL/FrameBuffer.h>
#include <Shader.h>
#include <Types.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class CubeMap
{
public:
    CubeMap(int ID, glm::vec3 Pos) {
        mPassThroughShader = (std::shared_ptr<Shader>) new Shader("shaders/PassThrough.vglsl", "shaders/PassThrough.fglsl");
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

        Position = Pos;
        this->ID = ID;
        type = DYNAMIC;

        hdrFBO = (std::shared_ptr<FrameBuffer<std::string> >) new FrameBuffer<std::string>(256, 256, true);

        hdrFBO->addRenderTarget("cubemap", GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true, GL_TEXTURE_CUBE_MAP);
        hdrFBO->FinishFrameBuffer();

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
        captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
        captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

        }

    CubeMap(std::vector<std::string> paths, int ID, glm::vec3 Pos)
    {
        texture = std::move((shared_ptr<eTexture>)(new eTexture(paths)));
        std::cout << "CubeMapID: " << texture->getTextureID() << std::endl;

        Position = Pos;
        this->ID = ID;
        type = STATIC;
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    }

    virtual ~CubeMap() {
        /*std::cout << "Deleted CubeMap" << std::endl;*/
    }

    virtual void CaptureEnvironment(GLuint index, GLuint inputTexture)
    {/*
        hdrFBO->bindFramebuffer();
        m_Quad = (std::shared_ptr<OpenGLHelpers::FullScreenQuad>) new OpenGLHelpers::FullScreenQuad();
        glUseProgram(mPassThroughShader->getProgramID());
        glBindTexture(GL_TEXTURE_2D, inputTexture);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, hdrFBO->getRenderTargetHandler("cubemap"), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Quad->Render();
        hdrFBO->unbindFramebuffer();
        glUseProgram(0);*/
    }

    glm::mat4 getViewMatrixbyIndex(int index)
    {
        return captureViews.at(index);
    }

    glm::mat4 getProjectionMatrix()
    {
        return this->captureProjection;
    }

    virtual glm::vec3 getPosition()
    {
        return this->Position;
    }

    virtual GLuint getTextureID()
    {
        if(type == STATIC)
        return texture->getTextureID();
        else if(type == DYNAMIC)return hdrFBO->getRenderTargetHandler("cubemap");
        else{}
    }

    virtual GLuint getCubemapFace(int index)
    {
        return m_CubemapSides.at(index);
    }

    CUBEMAP_TYPE getType()
    {
        return type;
    }

private:
    int ID = 0;
    glm::vec3 Position;
    std::shared_ptr<eTexture> texture;
    std::vector<glm::vec3> m_CubeMapsDirections;
    std::vector<GLuint> m_CubemapSides;
    std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_Quad;
    std::shared_ptr<Shader> mPassThroughShader;
    std::shared_ptr<FrameBuffer<std::string> > hdrFBO;
    glm::mat4 captureProjection;
    std::vector<glm::mat4> captureViews;

    CUBEMAP_TYPE type;

protected:
};

class PCCB : public CubeMap
{

public:

    PCCB();

    ~PCCB() {}

    std::vector<glm::vec3> getBoxMinMaxPoints()
    {
        return BoxMinMaxPoints;
    }


    std::vector<glm::vec3> BoxMinMaxPoints;
};
