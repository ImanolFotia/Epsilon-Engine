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
#include <cubemapRenderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <exception>

class CubeMap {
public:
    CubeMap(int ID, glm::vec3 Pos) {

        Position = Pos;
        this->ID = ID;
        type = DYNAMIC;

        mMainShader = (std::shared_ptr<Shader>) new Shader("shaders/vertex.glsl", "shaders/fragment.frag");

        genFrameBuffer();

        captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);

        captureViews[0] = glm::lookAt(Position, glm::vec3( 1.0f,  0.0f,  0.0f) + Position, glm::vec3(0.0f, -1.0f,  0.0f));
                          captureViews[1] = glm::lookAt(Position, glm::vec3(-1.0f,  0.0f,  0.0f) + Position, glm::vec3(0.0f, -1.0f,  0.0f));
                                            captureViews[2] = glm::lookAt(Position, glm::vec3( 0.0f,  1.0f,  0.0f) + Position, glm::vec3(0.0f,  0.0f,  1.0f));
                                                    captureViews[3] = glm::lookAt(Position, glm::vec3( 0.0f, -1.0f,  0.0f) + Position, glm::vec3(0.0f,  0.0f, -1.0f));
                                                            captureViews[4] = glm::lookAt(Position, glm::vec3( 0.0f,  0.0f,  1.0f) + Position, glm::vec3(0.0f, -1.0f,  0.0f));
                                                                    captureViews[5] = glm::lookAt(Position, glm::vec3( 0.0f,  0.0f, -1.0f) + Position, glm::vec3(0.0f, -1.0f,  0.0f));


    }

    CubeMap(std::vector<std::string> paths, int ID, glm::vec3 Pos) {
        texture = std::move((shared_ptr<eTexture>)(new eTexture(paths)));
        std::cout << "CubeMapID: " << texture->getTextureID() << std::endl;

        Position = Pos;
        this->ID = ID;
        type = STATIC;
    }

    virtual ~CubeMap() {
        /*std::cout << "Deleted CubeMap" << std::endl;*/
    }

    virtual void CaptureEnvironment(int index) {

        glClearColor(0.05,0.08,0.2, 1.0);
        glViewport(0,0,512,512);
        glEnable(GL_DEPTH_TEST);

        mMainShader->Use();
        mMainShader->PushUniform("projection", captureProjection);

        glUniformMatrix4fv(glGetUniformLocation(mMainShader->getProgramID(), "view"), 1, GL_FALSE, (const float*)&captureViews[index]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+index, cubemapTex, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }

    void endCapturingEnvironment() {
        //renderer->End();
        //this->mMainShader->Free();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void genFrameBuffer() {

        glGenFramebuffers(1, &captureFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glGenTextures(1, &cubemapTex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


        glGenRenderbuffers(1, &captureRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    }

    /*
        void setUniforms(glm::mat4 model)
        {
            renderer->mMainShader->PushUniform("model", model);

        }
    */
    glm::mat4 getViewMatrixbyIndex(int index) {
        return captureViews[index];
    }

    glm::mat4 getProjectionMatrix() {
        return this->captureProjection;
    }

    virtual glm::vec3 getPosition() {
        return this->Position;
    }

    virtual GLuint getTextureID() {
        if(type == STATIC)
            return texture->getTextureID();
        else if(type == DYNAMIC)return cubemapTex;
        else {}
    }

    virtual GLuint getCubemapFace(int index) {
        return m_CubemapSides.at(index);
    }

    int getID()
    {
        return this->ID;
    }

    CUBEMAP_TYPE getType() {
        return type;
    }

    glm::mat4 captureViews[6];
    GLuint cubemapTex;
    std::shared_ptr<Shader> mMainShader;
    glm::mat4 captureProjection;

    GLuint captureFBO = 0;
    GLuint captureRBO = 0;
private:
    int ID = 0;
    glm::vec3 Position;
    std::shared_ptr<eTexture> texture;
    std::vector<glm::vec3> m_CubeMapsDirections;
    std::vector<GLuint> m_CubemapSides;
    std::vector<GLuint> m_renderTargets;
    std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_Quad;
    std::shared_ptr<Shader> mPassThroughShader;
    std::shared_ptr<FrameBuffer<std::string> > hdrFBO;
    CUBEMAP_TYPE type;

protected:
};

class PCCB : public CubeMap {

public:

    PCCB();

    ~PCCB() {}

    std::vector<glm::vec3> getBoxMinMaxPoints() {
        return BoxMinMaxPoints;
    }


    std::vector<glm::vec3> BoxMinMaxPoints;
};
